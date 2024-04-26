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
void set_car_pos(int &X, int &Y, Orientation orientation, int **map);
int ** create_map(char **map_str, int height, int width, std::pair<int, int> &red_car_position, Orientation &orientation);


Direction oposite_direction(Direction direction){
    switch (direction) {
        case UP:
            return DOWN;
        case DOWN:
            return UP;
        case LEFT:
            return RIGHT;
        case RIGHT:
            return LEFT;
    }
    throw std::invalid_argument("no such direction");
}

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

Orientation get_car_orient(int** map, int car_id, int X, int Y){
    if (car_id == map[Y + 1][X] || car_id == map[Y - 1][X]) return HORIZONTAL;
    if (car_id == map[Y][X + 1] || car_id == map[Y][X - 1]) return VERTICAL;
    throw std::runtime_error("cannot find car orientation");
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

void print_map(int **map, int height, int width) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++)
            std::cout << map[i][j] << "\t";
        printf("\n");
    }
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
    if (map_str[red_car_position.second][red_car_position.first - 1] == 'o')
        orientation = VERTICAL;
    else if (map_str[red_car_position.second - 1][red_car_position.first] == 'o')
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
        if (curr_place_id == -2)
            throw std::runtime_error("car hit obstacle");
        set_car_pos(X, Y, get_car_orient(map , map[Y][X], X, Y), map);
        return {X, Y};
    }
    return {-1, -1};
}

int car_len(Orientation orient, int X, int Y, int **map) {
    int car_id = map[Y][X];
    int res = 0;
    if (orient == HORIZONTAL) {
        while (map[--Y][X] == car_id);
        Y++;
        while (map[Y++][X] == car_id) res++;
    } else if (orient == VERTICAL) {
        while (map[Y][--X] == car_id);
        X++;
        while (map[Y][X++] == car_id) res++;
    }
    return res;
}

// can optimize
std::list<std::pair<Direction, int>> get_possible_moves(
        int **map, int height, int width, int X, int Y
) {
    int car_id = map[Y][X];
    Orientation orient = get_car_orient(map , car_id, X, Y);
    std::list<std::pair<Direction, int>> moves;
    Direction dir;

    int car_length = car_len(orient, X, Y, map);
    if (orient == VERTICAL) {
        dir = LEFT;
        for (int i = 1; i < width - car_length; i++) {
            if (i == X) {
                dir = RIGHT;
                continue;
            }
            if (map[Y][i] == -2) {
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
    } else if (orient == HORIZONTAL) {
        dir = UP;
        for (int i = 1; i < height - car_length; i++) {
            if (i == Y) {
                dir = DOWN;
                continue;
            }
            if (map[i][X] == -2) {
                switch (dir) {
                    case UP:
                        moves.clear();
                        continue;
                    case DOWN:
                        return moves;
                }
            }
            moves.emplace_back(dir, abs(i - Y));
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

void _make_move(int **map, int X, int Y, Direction dir, int L, int car_id) {
    std::pair<int, int> iterator = get_iterator(dir);
    int fut_X = X + L * iterator.first;
    int fut_Y = Y + L * iterator.second;
    int car_size = 0;
    while (map[Y][X] == car_id) {
        car_size++;
        map[Y][X] = 0;
        X -= iterator.first;
        Y -= iterator.second;
    }
    for (int i = 0; i < car_size; i++) {
        if (map[fut_Y][fut_X] != 0) throw std::runtime_error("car hit something");
        map[fut_Y][fut_X] = car_id;
        fut_X -= iterator.first;
        fut_Y -= iterator.second;
    }
}

void set_car_pos(int &X, int &Y, Orientation orientation, int **map) {
    int car_id = map[Y][X];
    if (orientation == HORIZONTAL)
        while (map[Y - 1][X] == car_id) Y--;
    else if (orientation == VERTICAL)
        while (map[Y][X - 1] == car_id) X--;
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
    set_car_pos(X, Y, orient, map);
    std::cout << X << "," << Y << "\t" << get_char(dir) << " " << L << std::endl;
    std::pair<int, int> car_cleared;
    auto next_car = get_car_to_move(map_source, height, width, X, Y, dir, L);
    if (next_car.first == -1) map_score = 0;
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

        car_cleared = next_car;
        next_car = get_car_to_move(map, height, width, X, Y, dir, L);
        if (car_cleared == next_car) return {nullptr, INT16_MAX, moves};
    }
//    making move
    _make_move(map, X, Y, dir, L, this_car_id);
    moves.push_back(
            "" + std::to_string(X) + " " + std::to_string(Y) + " " + get_char(dir) + " " + std::to_string(L)
    );
    map_score++;
    print_map(map, height, width);
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
    Direction dir_to_move;
    if (start_orient == HORIZONTAL) {
        if (map[height - 1][start_pos.first] == 0) {
            L = height - start_pos.second;
            dir_to_move = DOWN;
        }
        while (map[start_pos.second - 1][start_pos.first] == -1) start_pos.second--;
        if (dir_to_move != DOWN && map[0][start_pos.first] == 0) {
            L = start_pos.second;
            dir_to_move = UP;
        }
    } else if (start_orient == VERTICAL) {
        if (map[start_pos.second][width - 1] == 0) {
            L = width - start_pos.first;
            dir_to_move = RIGHT;
        }
        while (map[start_pos.second][start_pos.first - 1] == -1) start_pos.first--;
        if (dir_to_move != RIGHT && map[start_pos.second][0] == 0) {
            L = start_pos.first;
            dir_to_move = LEFT;
        }
    }
    std::cout << "L=" << L << std::endl;
    auto res = make_move(map, height, width, start_pos.first, start_pos.second, dir_to_move, L, moves);
    print_map(map, height, width);
    for (const auto &move: std::get<2>(res))
        std::cout << "\033[34m" << move << "\033[0m" << std::endl;
    clear_map(map, height);
}
