#include <variant>
#include <queue>
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

void QuadNode::removeEmptyNode() {
    if (_isLeaf) { return; }

    size_t numParticles = 0;
    int nonEmptyChildCount = 0;
    QuadNode* nonEmptyChild = nullptr;

    for (const auto& child: children) {
        if (child) {
            if (!child->_isLeaf || !child->particles.empty()) {
                nonEmptyChildCount++;
                numParticles += child->particles.size();
                nonEmptyChild = child.get();
            }
        }
    }

    if (nonEmptyChildCount == 0)
    {
        for (auto& child : children) {
            child.reset();
        }
        _isLeaf = true;
        return;
    }

    if (numParticles > 0 && nonEmptyChildCount == 1)
    {
        particles = std::move(nonEmptyChild->particles);
        for (auto& child : children) {
            child.reset();
        }
        _isLeaf = true;
    }

    return;
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
        removeEmptyNode();
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

struct KNNElement {
    std::variant<QuadNode*, std::shared_ptr<Particle>> element;

    KNNElement(QuadNode* node) : element(node) {}
    KNNElement(std::shared_ptr<Particle> particle) : element(particle) {}
    
    bool isNode() const { return std::holds_alternative<QuadNode*>(element); }

    NType distance(Point2D& query) const {
        if (isNode()) { return std::get<QuadNode*>(element)->getBoundary().distance(query); }
        else { return query.distance(std::get<std::shared_ptr<Particle>>(element)->getPosition()); }
    }
};

struct CompareKNNElement {
    Point2D query;
    CompareKNNElement(Point2D query): query(query) {}

    bool operator()(const KNNElement& element1, const KNNElement& element2){
        return element1.distance(query) > element2.distance(query);
    }
};

std::vector<std::shared_ptr<Particle>> QuadTree::knn(Point2D query, size_t k) {
    std::vector<std::shared_ptr<Particle>> knnParticles;
    auto comparator = CompareKNNElement(query);
    std::priority_queue<KNNElement, std::vector<KNNElement>, CompareKNNElement> pq(comparator);

    pq.push(KNNElement(root.get()));
    
    while (!pq.empty() && knnParticles.size() < k) {
        KNNElement element = pq.top();
        pq.pop();

        if (element.isNode()) {
            QuadNode* node = std::get<QuadNode*>(element.element);
            if (node->isLeaf()) {
                for (auto& particle : node->getParticles()) {
                    pq.push(KNNElement(particle));
                }
            } else {
                for (auto& child : node->getChildren()) {
                    pq.push(KNNElement(child.get()));
                }
            }
        } else {
            std::shared_ptr<Particle> particle = std::get<std::shared_ptr<Particle>>(element.element);
            knnParticles.push_back(particle);
        }
    }

    return knnParticles;
}