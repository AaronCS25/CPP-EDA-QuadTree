#include "QuadTree.h"

// QuadNode
void QuadNode::addToBucket(const std::shared_ptr<Particle>& particle) {
    particles.push_back(particle);
}

bool QuadNode::propagate(const std::shared_ptr<Particle>& particle) {
    // TODO: Implement propagate function
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
}

void QuadNode::relocateParticle(const std::shared_ptr<Particle>& particle) {
    // TODO: Implement relocateParticle function
}

void QuadNode::removeEmptyNode(QuadNode* emptyChild) {
    // TODO: Implement removeEmptyNode function
}

bool QuadNode::insert(const std::shared_ptr<Particle>& particle) {
    // TODO: Implement insert function
    return false;
}

void QuadNode::updateNode() {
    // TODO: Implement updateNode function
}