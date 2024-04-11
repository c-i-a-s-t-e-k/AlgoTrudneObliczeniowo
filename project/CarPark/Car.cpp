//
// Created by ciastek on 21/03/24.
//
#include "Car.h"

Car::Car(char **string_map, Point place) {
    int x = place.getX();
    int y = place.getY();
    char car_part = string_map[x][y];
    if('a' <= car_part && car_part <= 'd'){
        this->orientation = VERTICAL;
        while (string_map[x][y] != 'a')
            x--;
        this->start_point = Point(x, y);
        this->length = 0;
        while ('a' <= string_map[x + length][y] && string_map[x+length][y] <= 'd')
            length++;
    } else if('w'<= car_part && car_part <= 'z'){
        this->orientation = HORIZONTAL;
        while (string_map[x][y] != 'x')
            y--;
        this->start_point = Point(x, y);
        this->length = 0;
        while ('w' <= string_map[x][y+length] && string_map[x][y+length] <= 'z')
            length++;
    } else if(car_part == 'o'){
        while (string_map[--x][y] == 'o');
        x++;
        while (string_map[x][--y] == 'o');
        y++;
        this->start_point = Point(x, y);
        if(string_map[x+1][y] == 'o'){
            this->orientation = VERTICAL;
            this->length = 0;
            while (string_map[x+length][y] == 'o') length ++;
        } else if(string_map[x][y+1] == 'o') {
            this->orientation = HORIZONTAL;
            this->length = 0;
            while (string_map[x][y + length] == 'o') length++;
        }
    } else{
        std::cerr << "this place:" << place.getX() << "," << place.getY() << " do not contains car" << std::endl;
    }
}
