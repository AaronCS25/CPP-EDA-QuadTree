#ifndef QUADTREE_H
#define QUADTREE_H

#include "Particle.h"
#include "Rect.h"
#include <vector>
#include <memory>
#include <array>

class Counter {
public:
    static size_t superCounter;
};

class QuadTree;

class QuadNode {
private:
    std::vector<std::shared_ptr<Particle>> particles;
    std::array<std::unique_ptr<QuadNode>, 4> children; // NW, NE, SW, SE
    Rect boundary;
    QuadNode* parent;
    bool _isLeaf;

    void addToBucket(const std::shared_ptr<Particle>& particle);
    bool propagate(const std::shared_ptr<Particle>& particle);
    void subdivide();

    void relocateParticle(const std::shared_ptr<Particle>& particle);
    void removeEmptyNode();

public:
    QuadNode(NType xmin, NType ymin, NType xmax, NType ymax, QuadNode* parent = nullptr)
        : boundary(Point2D(xmin,ymin),Point2D(xmax,ymax)), parent(parent), _isLeaf(true) {}
    QuadNode(const Rect& boundary, QuadNode* parent = nullptr)
        : boundary(boundary), parent(parent), _isLeaf(true) {}

    bool insert(const std::shared_ptr<Particle>& particle);
    void updateNode();

    // Getters
    const std::vector<std::shared_ptr<Particle>>& getParticles() const { return particles; }
    const std::unique_ptr<QuadNode>& getChild(size_t index) const { return children[index]; }
    const std::array<std::unique_ptr<QuadNode>, 4>& getChildren() const { return children; }
    const Rect& getBoundary() const { return boundary; }
    const QuadNode* getParent() const { return parent; }

    // Setters
    void setParent(QuadNode* parent) { this->parent = parent; }

    bool isLeaf() const { return _isLeaf; }
};


class QuadTree {
private:
    std::unique_ptr<QuadNode> root;

public:
    static size_t bucketSize;

    // Constructors
    QuadTree(NType xmin, NType ymin, NType xmax, NType ymax, size_t bucketSize) 
        : root(std::make_unique<QuadNode>(Rect(Point2D(xmin,ymin),Point2D(xmax,ymax)))) {
        QuadTree::bucketSize = bucketSize; 
    }
    QuadTree(const Rect& boundary, size_t bucketSize) 
        : root(std::make_unique<QuadNode>(boundary)) {
        QuadTree::bucketSize = bucketSize; 
    }
    QuadTree(NType xmin, NType ymin, NType xmax, NType ymax) 
        : root(std::make_unique<QuadNode>(Rect(Point2D(xmin,ymin),Point2D(xmax,ymax)))) {}
    QuadTree(const Rect& boundary) 
        : root(std::make_unique<QuadNode>(boundary)) {}

    void insert(const std::vector<std::shared_ptr<Particle>>& particles) {
        for (const auto& particle : particles) {
            root->insert(particle);
        }
    }

    const std::unique_ptr<QuadNode>& getRoot() const { return root; }

    void updateTree() {
        root->updateNode();
    }

    std::vector<std::shared_ptr<Particle>> knn(Point2D query, size_t k);
};

#endif // QUADTREE_H
