#include <iostream>
#include "carPark.h"
#include <cstdlib>

int main(int argc, char *argv[]) {
    std::cout << "Hello, World! "<< argc << std::endl;
    int width, height, moves;
//    width = atoi(argv[0]);
//    height = atoi(argv[1]);
//    moves = atoi(argv[2]);
    char *map = argv[5];
    std::cout<<map<<std::endl;
    return 0;
}
