//
// Created by ciastek on 19/03/24.
//

#ifndef PROJECT_POINT_H
#define PROJECT_POINT_H
#include <cstddef>
#include <iostream>
class Point{
private:
    int x,y;
public:

    Point(int xValue, int yValue) :x(xValue), y(yValue){}

    int getX(){
        return x;
    }
    int getY(){
        return  y;
    }
    void set(int xValue, int yValue);
    friend std::ostream& operator<<(std::ostream& os, const Point& p);

    Point(std::nullptr_t pVoid);
};
#endif //PROJECT_POINT_H
