//
// Created by ciastek on 19/03/24.
//

#ifndef PROJECT_MAP_H
#define PROJECT_MAP_H
#include <list>
#include <iostream>
#include "Car.h"

class Map{
private:
    size_t height, width;
    std::list<Car> cars;
    Car* red_car; //deans car
    Point exit = nullptr;
    char ** stringMap;

    void get_exit(char *park[]);
public:
    friend char ** solve1();
    Map(char **string_map, size_t height, size_t width);
};
#endif //PROJECT_MAP_H
