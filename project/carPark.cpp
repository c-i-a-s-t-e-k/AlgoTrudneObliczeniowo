//
// Created by ciastek on 13/03/24.
//
#include "Point.cpp"
#include<iostream>
template<size_t rows, size_t cols>
Point get_exit(char (&park)[rows][cols]) {
    Point exit = Point(-1, -1);

    for (int i = 0; i < cols; i++) {
        if (park[0][i] == '.') {
            exit.set(0, i);
            break;
        }
        if (park[rows - 1][i] == '.') {
            exit.set(cols - 1, i);
            break;
        }
    }
    if (exit.getX() == -1)
        for (int i = 0; i < rows; ++i) {
            if (park[i][0] == '.') {
                exit.set(i, 0);
                break;
            }
            if (park[i][cols - 1] == '.') {
                exit.set(i, cols - 1);
                break;
            }
        }
    return exit;
}

template<size_t rows, size_t cols>
void carPark(char (&park)[rows][cols], int n){
    Point exit = get_exit(park);

    std::cout << "exit: "<< exit << "\n";
}
