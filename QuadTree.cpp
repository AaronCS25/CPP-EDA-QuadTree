#include "QuadTree.h"
size_t Counter::superCounter = 0;
// QuadNode
void QuadNode::addToBucket(const std::shared_ptr<Particle>& particle) {
    particles.push_back(particle);
}

bool QuadNode::propagate(const std::shared_ptr<Particle>& particle) {
    if (_isLeaf && boundary.contains(particle->getPosition()))
    {
        return insert(particle); 
    }

    if (boundary.contains(particle->getPosition()))
    {
        for (const auto& child : children) {
            if (child && child->getBoundary().contains(particle->getPosition())) {
                return child->propagate(particle);
            }
        }
    }

    return false;
}

void QuadNode::subdivide() {
    Point2D Pmin = boundary.getPmin();
    Point2D Pmax = boundary.getPmax();
    Point2D centerP = boundary.getCenter();
    children[0] = std::make_unique<QuadNode>(Pmin.getX(), centerP.getY(), centerP.getX(), Pmax.getY(), this); // NW
    children[1] = std::make_unique<QuadNode>(centerP.getX(), centerP.getY(), Pmax.getX(), Pmax.getY(), this); // NE
    children[2] = std::make_unique<QuadNode>(Pmin.getX(), Pmin.getY(), centerP.getX(), centerP.getY(), this); // SW
    children[3] = std::make_unique<QuadNode>(centerP.getX(), Pmin.getY(), Pmax.getX(), centerP.getY(), this); // SE
    _isLeaf = false;
}

void QuadNode::relocateParticle(const std::shared_ptr<Particle>& particle) {
    if (!boundary.contains(particle->getPosition()))
    {
        if (parent) { parent->relocateParticle(particle); }
        return;
    }

    propagate(particle);
    return;
}

void QuadNode::removeEmptyNode(QuadNode* emptyChild) {
    // Si el nodo es una hoja, no debería llamar a esta función
    if (_isLeaf) { 
        return; 
    }

    size_t numParticles = 0;
    int nonEmptyChildCount = 0;
    QuadNode* nonEmptyChild = nullptr;

    // Contar partículas y verificar los hijos no vacíos
    for (const auto& child: children) {
        if (child) {
            if (!child->_isLeaf || !child->particles.empty()) {
                nonEmptyChildCount++;
                numParticles += child->particles.size();
                nonEmptyChild = child.get();
            }
        }
    }

    // Si no hay partículas y no hay hijos no vacíos, vaciar el nodo actual
    if (numParticles == 0 && nonEmptyChildCount == 0) {
        for (auto& child : children) {
            child.reset();
        }
        _isLeaf = true;
    }
    // Consolidar si solo hay un hijo no vacío y el número total de partículas es menor o igual al tamaño del bucket
    else if (nonEmptyChildCount == 1 && numParticles <= QuadTree::bucketSize) {
        particles = std::move(nonEmptyChild->particles);
        for (auto& child : children) {
            child.reset();
        }
        _isLeaf = true;
    }
}


bool QuadNode::insert(const std::shared_ptr<Particle>& particle) {
    if (!boundary.contains(particle->getPosition())) { return false; }

    if (_isLeaf && particles.size() < QuadTree::bucketSize)
    {
        addToBucket(particle);
        return true;
    }

    if (_isLeaf) {
        subdivide();
        for (const auto& p: particles) { relocateParticle(p); }
        particles.clear();
    }
    
    return propagate(particle);
}

void QuadNode::updateNode() {
    if (!_isLeaf) {
        for (const auto& child: children)
        {
            child->updateNode();
        }
        // removeEmptyNode(this);
        return;
    }

    std::vector<std::shared_ptr<Particle>> particlesToRelocate;

    for (auto it = particles.begin(); it != particles.end(); ) {
        if (!boundary.contains((*it)->getPosition())) {
            particlesToRelocate.push_back(*it);
            it = particles.erase(it);
        } else {
            ++it;
        }
    }

    for (const auto& p : particlesToRelocate) {
        relocateParticle(p);
    }

    return;
}