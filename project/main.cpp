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

int **
create_map(char **map_str, int height, int width, std::pair<int, int> &red_car_position, Orientation &orientation);

std::list<std::tuple<int **, int, std::list<std::string>>> get_posible_paths(
        const std::list<std::tuple<int **, int, std::list<std::string>>> &roots, int height,
        int width, int X, int Y, Direction dir, int L, int in_moves
);

std::tuple<int **, int, std::list<std::string>> make_move(
        int **map_source, int height, int width, int X, int Y, Direction dir, int L, int in_moves
);


Direction oposite_direction(Direction direction) {
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

Orientation get_car_orient(int **map, int X, int Y) {
    int car_id = map[Y][X];
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

std::pair<int, int> get_car_to_move(int **map, int X, int Y, Direction dir, int L) {
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
        set_car_pos(X, Y, get_car_orient(map, X, Y), map);
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
        int **map, int height, int width, int X, int Y, std::pair<int, int> hit_point
) {
    int car_id = map[Y][X];
    Orientation orient = get_car_orient(map, X, Y);
    int hit = orient == VERTICAL ? hit_point.first : hit_point.second;
    int axis = orient == VERTICAL ? X : Y;
    Direction prev_dir = orient == VERTICAL ? LEFT : UP;
    Direction post_dir = orient == VERTICAL ? RIGHT : DOWN;
    std::list<std::pair<Direction, int>> moves;
    Direction dir;
    std::pair<int, int> obstacle_bound = {0, -1};

    int car_length = car_len(orient, X, Y, map);
//    calculating bounds
    if (orient == VERTICAL) {
        obstacle_bound.second = width - 1;
        dir = LEFT;
        for (int i = 1; i < width - 1; i++) {
            if (i == X) {
                i += car_length - 1;
                dir = RIGHT;
                continue;
            }
            if (map[Y][i] == -2) {
                if (dir == LEFT) {
                    obstacle_bound.first = i;
                    continue;
                } else {
                    obstacle_bound.second = i;
                    break;
                }
            }
            if (map[Y][i] != 0 && get_car_orient(map, i, Y) == VERTICAL) {
                int other_car_len = car_len(VERTICAL, i, Y, map);
                i += other_car_len - 1;
                if (dir == LEFT)
                    obstacle_bound.first += other_car_len;
                else obstacle_bound.second -= other_car_len;
            }
        }
    } else {
        obstacle_bound.second = height - 1;
        dir = UP;
        for (int i = 1; i < height - 1; i++) {
            if (i == Y) {
                i += car_length - 1;
                dir = DOWN;
                continue;
            }
            if (map[i][X] == -2) {
                if (dir == UP) {
                    obstacle_bound.first = i;
                    continue;
                } else {
                    obstacle_bound.second = i;
                    break;
                }
            }
            if (map[i][X] != 0 && get_car_orient(map, i, Y) == HORIZONTAL) {
                int other_car_len = car_len(HORIZONTAL, X, i, map);
                i += other_car_len - 1;
                if (dir == UP)
                    obstacle_bound.first += other_car_len;
                else obstacle_bound.second -= other_car_len;
            }
        }
    }
//    collecting moves
    for (int i = obstacle_bound.first + 1; i < hit - car_length + 1; i++)
        moves.emplace_back(prev_dir, axis - i);
    for (int i = hit+1; i < obstacle_bound.second - car_length + 1; i++)
        moves.emplace_back(post_dir, i - axis);

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
        X += abs(iterator.first);
        Y += abs(iterator.second);
    }
    for (int i = 0; i < car_size; i++) {
        if (map[fut_Y][fut_X] != 0) throw std::runtime_error("car hit something");
        map[fut_Y][fut_X] = car_id;
        fut_X += abs(iterator.first);
        fut_Y += abs(iterator.second);
    }
}

std::pair<int, int> get_hit_point(int** map, int X, int Y, std::pair<int, int> next_car){
    Orientation prev_orient = get_car_orient(map, X, Y);
    Orientation next_orient = get_car_orient(map, next_car.first, next_car.second);
    if (prev_orient == next_orient) throw std::runtime_error("you will try do invalid moves");
    return prev_orient == VERTICAL ? std::pair{X, next_car.second} : std::pair{next_car.first, Y};
}

void set_car_pos(int &X, int &Y, Orientation orientation, int **map) {
    int car_id = map[Y][X];
    if (orientation == HORIZONTAL)
        while (map[Y - 1][X] == car_id) Y--;
    else if (orientation == VERTICAL)
        while (map[Y][X - 1] == car_id) X--;
}

std::list<std::tuple<int **, int, std::list<std::string>>> get_posible_paths
        (std::list<std::tuple<int **, int, std::list<std::string>>> &roots, int height, int width, int X, int Y,
         Direction dir, int L, int moves) {
    std::list<std::tuple<int **, int, std::list<std::string>>> paths;
    int **map;
    int score, car_id;
    std::list<std::string> moves_list;
    for (auto root: roots) {
        if (std::get<1>(root) <= moves) {
            map = std::get<0>(root);
            score = std::get<1>(root);
            moves_list = std::get<2>(root);
            car_id = map[Y][X];
            auto next_car = get_car_to_move(map, X, Y, dir, L);
            if (next_car.first != -1) {
                auto hit_point = get_hit_point(map, X, Y, next_car);
                for (auto possible_move: get_possible_moves(map, height, width, next_car.first, next_car.second, hit_point)) {
                    std::list<std::tuple<int **, int, std::list<std::string>>> new_root;
                    std::list<std::string> moves_list_copy;
                    std::copy(moves_list.begin(), moves_list.end(), std::back_inserter(moves_list_copy));
                    new_root.emplace_back(map_copy(map, height, width), score, moves_list_copy);
                    auto new_paths = get_posible_paths(new_root, height, width, next_car.first, next_car.second,
                                                       possible_move.first, possible_move.second, moves);
                    paths.splice(paths.end(), new_paths);
                }
                clear_map(map, height);
                moves_list.clear();
            } else {
                _make_move(map, X, Y, dir, L, car_id);
                score += 1;
                moves_list.push_back(
                        "" + std::to_string(X) + " " + std::to_string(Y) + " " + get_char(dir) + " " +
                        std::to_string(L)
                );
                paths.emplace_back(map, score, moves_list);
            }
        } else {
            clear_map(std::get<0>(root), height);
            std::get<2>(root).clear();
        }
    }
    roots.clear();

    return paths;
}

std::tuple<int **, int, std::list<std::string>> make_move(
        int **map_source, int height, int width, int X, int Y, Direction dir, int L, int in_moves
) {
    std::list<std::string> moves;
    if (in_moves <= 0) return {nullptr, INT16_MAX, moves};
    int **map = map_copy(map_source, height, width);
    int map_score = 0;
    int this_car_id = map_source[Y][X];
    Orientation orient = get_orient(dir);
    set_car_pos(X, Y, orient, map);
    std::cout << "car:" << map[Y][X] << "\t" << get_char(dir) << " " << L << std::endl;
    std::pair<int, int> car_cleared;
    auto next_car = get_car_to_move(map_source, X, Y, dir, L);
    std::list<std::string> tmp_moves;
    int tmp_score = INT16_MAX;
    int **tmp_map = nullptr;
//    clear road
    while (next_car.first != -1) {
        in_moves--;
//        find best move
        auto hit_point = get_hit_point(map, X, Y, next_car);
        for (std::pair<Direction, int> move: get_possible_moves(
                map, height, width, next_car.first, next_car.second, hit_point
        )) {
            auto tmp_tuple = make_move(
                    map, height, width, next_car.first, next_car.second, move.first, move.second, in_moves
            );
            if (std::get<1>(tmp_tuple) < INT16_MAX) {
                if (tmp_map != nullptr)
                    clear_map(tmp_map, height);
                tmp_map = std::get<0>(tmp_tuple);
                tmp_score = std::get<1>(tmp_tuple);
                tmp_moves = std::get<2>(tmp_tuple);

                auto tmp_pos = find_car(tmp_map, height, width, this_car_id, {X, Y}, orient);
                if (orient == HORIZONTAL)
                    L -= (tmp_pos.second - Y);
                else if (orient == VERTICAL)
                    L -= (tmp_pos.first - X);
                int tmp_X = tmp_pos.first;
                int tmp_Y = tmp_pos.second;
            } else {
                clear_map(std::get<0>(tmp_tuple), height);
                std::get<2>(tmp_tuple).clear();
            }
        }

        clear_map(map, height);
        map = tmp_map;
        map_score += tmp_score;
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
        next_car = get_car_to_move(map, X, Y, dir, L);
        if (car_cleared == next_car) return {nullptr, INT16_MAX, moves};

    }
//    making move
    _make_move(map, X, Y, dir, L, this_car_id);
    moves.push_back(
            "" + std::to_string(X) + " " + std::to_string(Y) + " " + get_char(dir) + " " + std::to_string(L)
    );
    map_score++;
    printf("------------------------------\n");
    for (const auto &move: moves)
        std::cout << "\033[31m" << move << "\033[0m" << " :: ";
    printf("\n");
    print_map(map, height, width);
    printf("------------------------------\n");
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
    std::cout << "\033[34m";
    std::cout << width << " " << height << " " << moves << std::endl;
    print_map(map, height, width);
    std::cout << "\033[0m";
    auto res = make_move(map, height, width, start_pos.first, start_pos.second, dir_to_move, L - 1, moves);
    std::cout << "\033[34m" << (std::get<1>(res)) << "\033[0m" << std::endl;
    for (const auto &move: std::get<2>(res))
        std::cout << "\033[34m" << move << "\033[0m" << std::endl;
//        std::cout << move << std::endl;

    clear_map(map, height);
}
