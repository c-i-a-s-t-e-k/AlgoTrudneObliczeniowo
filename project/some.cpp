//
// Created by ciastek on 29/04/24.
//

#include <iostream>

int main(int argc, char *argv[]) {
    int width, height, moves;
    std::cin >> width >> height >> moves;
    char **map = new char *[height];
    for (int i = 0; i < height; i++)
        map[i] = new char[width];
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            std::cin >> map[i][j];
    std::cout << 3 << std::endl;
    std::cout << width << height << moves << map[0] << std::endl;
//    width = atoi(argv[1]);
//    height = atoi(argv[2]);
//    moves = atoi(argv[3]);
}