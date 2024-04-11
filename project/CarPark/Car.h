//
// Created by ciastek on 19/03/24.
//

#ifndef PROJECT_CAR_H
#define PROJECT_CAR_H
#include "Point.h"
#include <iostream>

enum Orientation {
    VERTICAL,
    HORIZONTAL
};


class Car{
private:
    Orientation orientation;
    Point start_point = Point(0,0);
    int length;
public:
    Car(char ** string_map, Point place);
    [[nodiscard]] int getLength() const{return length;}
};




#endif //PROJECT_CAR_H
