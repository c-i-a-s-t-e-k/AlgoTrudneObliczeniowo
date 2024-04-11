//
// Created by ciastek on 19/03/24.
//
#include "Map.h"
void Map::get_exit(char *park[]){
    for (int i = 0; i < width; i++) {
        if (park[0][i] == '.') {
            exit.set(0, i);
            break;
        }
        if (park[height - 1][i] == '.') {
            exit.set(width - 1, i);
            break;
        }
    }
    if (exit.getX() == -1)
        for (int i = 0; i < height; ++i) {
            if (park[i][0] == '.') {
                exit.set(i, 0);
                break;
            }
            if (park[i][width - 1] == '.') {
                exit.set(i, width - 1);
                break;
            }
        }
}

Map::Map(char **string_map, size_t height, size_t width) : width(width), height(height) {
    get_exit(string_map);
    Point place = nullptr;
    for(int i=0; i< height; i++)
        for(int j=0; j < width; j++){
            switch (string_map[i][j]) {
                case 'o':
                    place.set(i,j);
                    red_car = new Car(string_map, place);
                    cars.push_back(*red_car);
                    break;
                case 'a':
                    place.set(i,j);
                    cars.emplace_back(string_map, place);
                    j += (cars.back().getLength() - 1);
                    break;
                case 'x':
                    place.set(i,j);
                    cars.emplace_back(string_map, place);
                    break;
            }
        }
    std::cout << "cars number" << cars.size() << std::endl;
}
