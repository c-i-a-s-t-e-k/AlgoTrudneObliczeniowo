//
// Created by ciastek on 13/03/24.
//
#include "Point.h"

std::ostream &operator<<(std::ostream &os, const Point &p) {
    os << "Point(" << p.x << ", " << p.y << ")";
    return os;
}

void Point::set(int xValue, int yValue) {
        x = xValue;
        y = yValue;
}

Point::Point(std::nullptr_t pVoid) {
    set(0,0);
}
