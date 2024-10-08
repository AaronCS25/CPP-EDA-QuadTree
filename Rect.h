#ifndef RECT_H
#define RECT_H

#include "Point.h"

class Rect
{
private:
    Point2D pmin, pmax;
public:
    Rect() = default;
    Rect(const Point2D& pmin, const Point2D& pmax): pmin(pmin), pmax(pmax) {}
    ~Rect() = default;

    Point2D getPmin() const { return pmin; }
    Point2D getPmax() const { return pmax; }
    Point2D getCenter() const { return (pmin + pmax) / 2.0f; }

    bool contains(const Point2D& point) const {
        return point.getX() >= pmin.getX() && point.getX() <= pmax.getX() && point.getY() >= pmin.getY() && point.getY() <= pmax.getY();
    }

    bool intersects(const Rect& other) const {
        return pmin.getX() < other.pmax.getX() && pmax.getX() > other.pmin.getX() &&
               pmin.getY() < other.pmax.getY() && pmax.getY() > other.pmin.getY();
    }

    bool isWithin(const Rect& other) const {
    return pmin.getX() >= other.pmin.getX() && pmax.getX() <= other.pmax.getX() &&
           pmin.getY() >= other.pmin.getY() && pmax.getY() <= other.pmax.getY();
    }

    bool isValid() const {
        return pmin.getX() <= pmax.getX() && pmin.getY() <= pmax.getY();
    }

    NType distance(Point2D& point) const {
        if (contains(point)) { return 0; }

        NType distX = 0, distY = 0;

        if (point.getX() < pmin.getX()) { distX = pmin.getX() - point.getX(); }
        else if (point.getX() > pmax.getX()) { distX = point.getX() - pmax.getX(); }

        if (point.getY() < pmin.getY()) { distY = pmin.getY() - point.getY(); }
        else if (point.getY() > pmax.getY()) { distY = point.getY() - pmax.getY(); }

        return sqrt(distX * distX + distY * distY);
    }

    bool operator==(const Rect& rect) const {
        return pmin == rect.pmin && pmax == rect.pmax;
    }

    bool operator!=(const Rect& rect) const {
        return !(*this == rect);
    }

    // Print
    friend std::ostream& operator<<(std::ostream& os, const Rect& rect) {
        os << "[Pmin:" << rect.pmin << ", Pmax:" << rect.pmax << "]";
        return os;
    }
};

#endif // RECT_H