#include <iostream>
#include <cstdlib>
#include <list>
#include <utility>
#include <stdexcept>
#include<tuple>
#include <string>


enum Orientation {
    VERTICAL,
    HORIZONTAL
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

char get_char(Direction direction) {
    switch (direction) {
        case UP:
            return 'U';
        case DOWN:
            return 'D';
        case LEFT:
            return 'L';
        case RIGHT:
            return 'R';
    }
    throw std::invalid_argument("no such direction");
}
char get_orient(Direction direction) {
    switch (direction) {
        case UP:
        case DOWN:
            return HORIZONTAL;
        case LEFT:
        case RIGHT:
            return VERTICAL;
    }
    throw std::invalid_argument("no such direction");
}
std::pair<int, int> get_iterator(Direction direction) {
    switch (direction) {
        case UP:
            return std::pair(0, -1);
        case DOWN:
            return std::pair(0, 1);
        case LEFT:
            return std::pair(-1, 0);
        case RIGHT:
            return std::pair(1, 0);
    }
    throw std::invalid_argument("no such direction");
}

int **
create_map(char **map_str, int height, int width, std::pair<int, int> &red_car_position, Orientation &orientation) {
    std::list<std::pair<int, int>> vertical;
    std::list<std::pair<int, int>> horizontal;
    int **map = new int *[height];
//    initialize map
    for (int i = 0; i < height; i++) {
        map[i] = new int[width];
        for (int j = 0; j < width; j++) {
            switch (map_str[i][j]) {
                case '#':
                    map[i][j] = -2;
                    break;
                case '.':
                    map[i][j] = 0;
                    break;
                case 'o':
                    map[i][j] = -1;
                    red_car_position = std::pair(i, j);
                    break;
                case 'a':
                    vertical.emplace_back(i, j);
                    break;
                case 'x':
                    horizontal.emplace_back(i, j);
                default:
                    continue;
            }
        }
    }

//    check red car orientation
    if (map_str[red_car_position.first][red_car_position.second - 1] == 'o')
        orientation = VERTICAL;
    else if (map_str[red_car_position.first - 1][red_car_position.second] == 'o')
        orientation = HORIZONTAL;

//    fill map with cars
    int current_map_index = 1;
    for (const auto &position: vertical) {
        int i = position.first;
        int j = position.second;
        while (j < width && 'a' <= map_str[i][j] && map_str[i][j] <= 'd')
            map[i][j++] = current_map_index;
        current_map_index++;
    }
    for (const auto &position: horizontal) {
        int i = position.first;
        int j = position.second;
        while (i < height && 'w' <= map_str[i][j] && map_str[i][j] <= 'z')
            map[i++][j] = current_map_index;
        current_map_index++;
    }
//    print map
//    for (int i = 0; i < height; i++) {
//        for (int j = 0; j < width; j++)
//            std::cout<<map[i][j]<<" ";
//        std::cout<<std::endl;
//    }

    return map;
}

void clear_map(int **map, int height) {
    if (map != nullptr) {
        for (int i = 0; i < height; i++) {
            if (map[i] != nullptr) {
                delete[] map[i];
                map[i] = nullptr;
            }
        }
        delete[] map;
        map = nullptr;
    }
}

int **map_copy(int **map_source, int height, int width) {
    int **map = new int *[height];
    for (int i = 0; i < height; i++) {
        map[i] = new int[width];
        for (int j = 0; j < width; j++) {
            map[i][j] = map_source[i][j];
        }
    }return map;
}

std::pair<int, int> get_car_to_move(int **map, int height, int width, int X, int Y, Direction dir, int L) {
    int car_id = map[Y][X];
    auto iterator = get_iterator(dir);
    int curr_place_id;
    for (int i = 0; i < L; i++) {
        X += iterator.first;
        Y += iterator.second;
        curr_place_id = map[Y][X];
        if (curr_place_id == car_id) {
            L++;
            continue;
        }
        if (curr_place_id == 0) continue;
        if (curr_place_id == -2) throw std::runtime_error("car hit obstacle");
        return std::pair(X, Y);
    }
    return std::pair(-1, -1);
}

std::list<std::pair<Direction, int>> get_possible_moves(
        int **map, int height, int width, int X, int Y
);

std::tuple<int **, int, std::list<std::string>>
make_move(int **map_source, int height, int width, int X, int Y, Direction dir, int L, int in_moves) {
    std::list<std::string> moves;
    if (in_moves <= 0) return {nullptr, INT16_MAX, moves};
    int **map = nullptr;
    int map_score = INT16_MAX;
    auto next_car = get_car_to_move(map_source, height, width, X, Y, dir, L);

    while (next_car.first != -1) {
        std::list<std::string> tmp_moves;
//        find best move
        for(std::pair<Direction, int> move : get_possible_moves(
                map_source, height, width, next_car.first, next_car.second
                )){
            auto tmp_tuple = make_move(
                    map_source, height, width, next_car.first, next_car.second, move.first, move.second, in_moves -1
                    );
            if(std::get<1>(tmp_tuple) < map_score) {
                clear_map(map, height);
                map = std::get<0>(tmp_tuple);
                map_score = std::get<1>(tmp_tuple);
                tmp_moves = std::get<2>(tmp_tuple);
            } else{
                clear_map(std::get<0>(tmp_tuple), height);
                std::get<2>(tmp_tuple).clear();
            }
        }
        throw std::runtime_error("do move not implemented");

    }
    moves.push_back(
            "" + std::to_string(X) + " " + std::to_string(Y) + " " + get_char(dir) + " " + std::to_string(L)
    );
    return {nullptr, 0, moves};
}

int main(int argc, char *argv[]) {
    std::cout << "Hello, World! " << argc << std::endl;
    int width, height, moves;
    width = atoi(argv[1]);
    height = atoi(argv[2]);
    moves = atoi(argv[3]);
    char **map_str = &argv[4];
    std::pair<int, int> start_pos;
    Orientation start_orient;
    int **map = create_map(map_str, height, width, start_pos, start_orient);
//    std::cout<<start_pos.first<<","<<start_pos.second<<std::endl;

    clear_map(map, height);
}
