//
// Created by ciastek on 19/03/24.
//

#ifndef PROJECT_POINT_H
#define PROJECT_POINT_H
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
    friend std::ostream& operator<<(std::ostream& os, const Point& p){
        os << "Point(" << p.x << ", " << p.y << ")";
        return os;
    }
};
#endif //PROJECT_POINT_H
