#include <iostream>
#include <cstdlib>
#include <list>
#include <utility>
#include <stdexcept>
#include<tuple>
#include <string>
#include <cmath>


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

Orientation get_orient(Direction direction) {
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
            return {0, -1};
        case DOWN:
            return {0, 1};
        case LEFT:
            return {-1, 0};
        case RIGHT:
            return {1, 0};
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
                    red_car_position = std::pair(j, i);
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
    }
    return map;
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
        return {X, Y};
    }
    return {-1, -1};
}

// can optimize
std::list<std::pair<Direction, int>> get_possible_moves(
        int **map, int height, int width, int X, int Y
) {
    Orientation orient;
    int car_id = map[Y][X];
    if(car_id == map[Y+1][X] || car_id == map[Y-1][X]) orient = HORIZONTAL;
    if(car_id == map[Y][X+1] || car_id == map[Y][X-1]) orient = VERTICAL;
    std::list<std::pair<Direction, int>> moves;
    Direction dir;

    if(orient == VERTICAL){
        dir = LEFT;
        for(int i = 1; i < width - 1; i++){
            if(i == X){
                dir = RIGHT;
                continue;
            }
            if(map[Y][i] == -2){
                switch (dir) {
                    case LEFT:
                        moves.clear();
                        continue;
                    case RIGHT:
                        return moves;
                }
            }
            moves.emplace_back(dir, abs(i - X));
        }
    }
    return moves;
}

std::pair<int, int>
find_car(int **map, int height, int width, int car_id, std::pair<int, int> prev_pos, Orientation orient) {
    if (map[prev_pos.second][prev_pos.first] == car_id) return prev_pos;
    int X = prev_pos.first;
    int Y = prev_pos.second;
    if (orient == HORIZONTAL)
        for (int i = 0; i < height; i++)
            if (map[i][X] == car_id) return {i, X};
    if (orient == VERTICAL)
        for (int i = 0; i < width; i++)
            if (map[Y][i] == car_id) return {Y, i};
    throw std::runtime_error("car not found");
}

void _make_move(int **map, int X, int Y, Direction dir, int L, int car_id){
    std::pair<int,int> iterator = get_iterator(dir);
    int fut_X = X + L * iterator.first;
    int fut_Y = Y + L * iterator.second;
    int car_size = 0;
    while (map[Y][X] == car_id){
        car_size++;
        map[Y][X] = 0;
        X += iterator.first;
        Y += iterator.second;
    }
    for(int i =0 ;i< car_size; i++){
        map[fut_Y][fut_X] = car_id;
        fut_X += iterator.first;
        fut_Y += iterator.second;
    }
}

std::tuple<int **, int, std::list<std::string>> make_move(
        int **map_source, int height, int width, int X, int Y, Direction dir, int L, int in_moves
        ) {
    std::list<std::string> moves;
    if (in_moves <= 0) return {nullptr, INT16_MAX, moves};
    int **map = map_copy(map_source, height, width);
    int map_score = INT16_MAX;
    int this_car_id = map_source[Y][X];
    Orientation orient = get_orient(dir);
    auto next_car = get_car_to_move(map_source, height, width, X, Y, dir, L);

//    clear road
    while (next_car.first != -1) {
        std::list<std::string> tmp_moves;
//        find best move
        for (std::pair<Direction, int> move: get_possible_moves(
                map_source, height, width, next_car.first, next_car.second
        )) {
            auto tmp_tuple = make_move(
                    map_source, height, width, next_car.first, next_car.second, move.first, move.second, in_moves - 1
            );
            if (std::get<1>(tmp_tuple) < map_score) {
                clear_map(map, height);
                map = std::get<0>(tmp_tuple);
                map_score = std::get<1>(tmp_tuple);
                tmp_moves = std::get<2>(tmp_tuple);
            } else {
                clear_map(std::get<0>(tmp_tuple), height);
                std::get<2>(tmp_tuple).clear();
            }
        }

        for (const auto &move: tmp_moves) moves.push_back(move);
        tmp_moves.clear();
        auto tmp_pos = find_car(map, height, width, this_car_id, {X, Y}, orient);
        if (orient == HORIZONTAL)
            L -= (tmp_pos.second - Y);
        else if (orient == VERTICAL)
            L -= (tmp_pos.first - X);
        X = tmp_pos.first;
        Y = tmp_pos.second;

        next_car = get_car_to_move(map, height, width, X, Y, dir, L);
    }
//    making move
    _make_move(map, X, Y, dir, L, this_car_id);
    moves.push_back(
            "" + std::to_string(X) + " " + std::to_string(Y) + " " + get_char(dir) + " " + std::to_string(L)
    );
    map_score++;
    return {map, map_score, moves};
}

int main(int argc, char *argv[]) {
    std::cout << "Hello, World! " << argc << std::endl;
    int width, height, moves;
    width = atoi(argv[1]);
    height = atoi(argv[2]);
    moves = atoi(argv[3]);
    char **map_str = &argv[4];
    int L;
    std::pair<int, int> start_pos;
    Orientation start_orient;
    int **map = create_map(map_str, height, width, start_pos, start_orient);
    if (start_orient == HORIZONTAL) {
        if(map[height-1][start_pos.first] == 0) L = height - start_pos.second;
        while (map[start_pos.second - 1][start_pos.first] == -1) start_pos.second--;
        if(map[0][start_pos.first] == 0) L = start_pos.second;
    } else if (start_orient == VERTICAL) {
        if(map[start_pos.second][width-1] == 0) L = width - start_pos.first;
        while (map[start_pos.second][start_pos.first - 1] == -1) start_pos.first--;
        if(map[start_pos.second][0] == 0) L = start_pos.first;
    }
    std::cout << "L=" << L << std::endl;
    clear_map(map, height);
}
