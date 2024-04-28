#include <iostream>
#include <cstdlib>
#include <list>
#include <stdexcept>
#include <string>
#include <set>
#include <map>
#include <vector>

enum Orientation {
    VERTICAL,
    HORIZONTAL
};

class map_scored {
public:
    int **map;
    int score;
    std::list<std::string> moves_list;

public:
    map_scored(int **map, int initial_score, std::list<std::string> other_move_list) : map(map), score(initial_score) {
        moves_list.splice(moves_list.begin(), other_move_list);
    }

    map_scored(int **map, int initial_score) : map(map), score(initial_score) {}
};


class pair {
public:
    int first;
    int second;

    pair(int first, int second) {
        this->first = first;
        this->second = second;
    }
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class pair_dir_int {
public:
    Direction first;
    int second;

    pair_dir_int(Direction first, int second) {
        this->first = first;
        this->second = second;
    }
};

class cars_info {
private:
    std::map<int, Orientation> cars_orientation;
    std::map<int, int> car_lengths;
    std::map<int, std::list<pair>> safe_palaces;

    static int car_len(Orientation orient, int X, int Y, int **map) {
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

    static Orientation get_car_orient(int **map, int X, int Y) {
        int car_id = map[Y][X];
        if (car_id == map[Y + 1][X] || car_id == map[Y - 1][X]) return HORIZONTAL;
        if (car_id == map[Y][X + 1] || car_id == map[Y][X - 1]) return VERTICAL;
        throw std::runtime_error("cannot find car orientation");
    }

public:
    Orientation get_car_orient(int car_id) {
        return cars_orientation[car_id];
    }

    int get_car_len(int car_id) {
        return car_lengths[car_id];
    }

    void add_cars(int **map, int height, int width) {
        std::map<int, std::set<int>> car_on_horizontal_line;
        std::map<int, std::set<int>> car_on_vertical_line;
        std::set<int> cars;
        int car_id;
        Orientation car_orient;
//        calculating cars orient and car len
        for (int y = 1; y < height - 1; y++)
            for (int x = 1; x < width - 1; x++) {
                car_id = map[y][x];
                if (car_id != -2 && car_id != 0 && cars.find(car_id) == cars.end()) {
                    cars.insert(car_id);
                    car_orient = get_car_orient(map, x, y);
                    cars_orientation[car_id] = car_orient;
                    car_lengths[car_id] = car_len(car_orient, x, y, map);
                    if (car_orient == HORIZONTAL)
                        car_on_vertical_line[x].insert(car_id);
                    else car_on_horizontal_line[y].insert(car_id);
                }
            }
//        calculating cars safe places == places where no car can make a collision with that car
        std::vector<std::vector<std::set<int>>> presence_table(height - 2, std::vector<std::set<int>>(width - 2));
        for (int y_ = 0; y_ < height - 2; y_++) {
            for (int x_ = 0; x_ < width - 2; x_++) {
                for (auto id: car_on_vertical_line[x_ + 1])
                    presence_table[y_][x_].insert(id);
                for (auto id: car_on_horizontal_line[y_ + 1])
                    presence_table[y_][x_].insert(id);
            }
        }
        car_on_horizontal_line.clear();
        car_on_vertical_line.clear();
        bool flag;
        for (int y_ = 0; y_ < height - 2; y_++) {
            for (int x_ = 0; x_ < width - 2; x_++) {
                if (presence_table[y_][x_].size() == 1) {
                    flag = true;
                    car_id = *presence_table[y_][x_].begin();
                    if (cars_orientation[car_id] == VERTICAL)
                        for (int x = x_ + 1; flag && x < x_ + car_lengths[car_id]; x++) {
                            flag = flag && x<width - 2&&
                                   (presence_table[y_][x].size() == 1 && *presence_table[y_][x].begin() == car_id);
                        }
                    else
                        for (int y = y_ + 1; flag && y < y_ + car_lengths[car_id]; y++) {
                            flag = flag && y < height -2 &&
                                   (presence_table[y][x_].size() == 1 && *presence_table[y][x_].begin() == car_id);
                        }
                    if (flag) safe_palaces[car_id].emplace_back(x_ + 1, y_ + 1);
                }
            }
        }
    }

    void print_save_places() {
        for (auto l: safe_palaces) {
            std::cout << l.first << "\t\t";
            for (auto safe_place: l.second)
                std::cout << safe_place.first << ',' << safe_place.second << "  ";
            std::cout << std::endl;
        }
    }
};

void set_car_pos(int &X, int &Y, Orientation orientation, int **map);

void print_map(int **map, int height, int width);

void print_status(int **map, int height, int width, const std::list<std::string> &moves_list) {
    printf("------------------------------\n");
    for (const auto &move: moves_list)
        std::cout << "\033[31m" << move << "\033[0m" << " :: ";
    printf("\n");
    print_map(map, height, width);
    printf("------------------------------\n");
}

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

pair get_iterator(Direction direction) {
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
create_map(char **map_str, int height, int width, pair *red_car_position, Orientation &orientation) {
    std::list<pair> vertical;
    std::list<pair> horizontal;
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
                    red_car_position->first = j;
                    red_car_position->second = i;
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
    if (map_str[red_car_position->second][red_car_position->first - 1] == 'o')
        orientation = VERTICAL;
    else if (map_str[red_car_position->second - 1][red_car_position->first] == 'o')
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

pair get_car_to_move(int **map, int X, int Y, Direction dir, int L, cars_info info) {
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
        set_car_pos(X, Y, info.get_car_orient(map[Y][X]), map);
        return {X, Y};
    }
    return {-1, -1};
}

// can optimize
std::list<pair_dir_int> get_possible_moves(
        int **map, int height, int width, int X, int Y, pair hit_point, cars_info info
) {
    int car_id = map[Y][X];
    Orientation orient = info.get_car_orient(car_id);
    int hit = orient == VERTICAL ? hit_point.first : hit_point.second;
    int axis = orient == VERTICAL ? X : Y;
    Direction prev_dir = orient == VERTICAL ? LEFT : UP;
    Direction post_dir = orient == VERTICAL ? RIGHT : DOWN;
    std::list<pair_dir_int> moves;
    Direction dir;
    pair obstacle_bound = {0, -1};

    int car_length = info.get_car_len(car_id);
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
            if (map[Y][i] != 0 && info.get_car_orient(map[Y][i]) == VERTICAL) {
                int other_car_len = info.get_car_len(map[Y][i]);
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
            if (map[i][X] != 0 && info.get_car_orient(map[i][X]) == HORIZONTAL) {
                int other_car_len = info.get_car_len(map[i][X]);
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
    for (int i = hit + 1; i < obstacle_bound.second - car_length + 1; i++)
        moves.emplace_back(post_dir, i - axis);

    return moves;
}

pair
find_car(int **map, int height, int width, int car_id, pair prev_pos, Orientation orient) {
    if (map[prev_pos.second][prev_pos.first] == car_id) {
        int X = prev_pos.first;
        int Y = prev_pos.second;
        set_car_pos(X, Y, orient, map);
        return {X, Y};
    }
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
    pair iterator = get_iterator(dir);
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

pair get_hit_point(int **map, int X, int Y, pair next_car, cars_info info) {
    Orientation prev_orient = info.get_car_orient(map[Y][X]);
    Orientation next_orient = info.get_car_orient(map[next_car.second][next_car.first]);
    if (prev_orient == next_orient) throw std::runtime_error("you will try do invalid moves");
    return prev_orient == VERTICAL ? pair(next_car.first, Y) : pair(X, next_car.second);
}

void set_car_pos(int &X, int &Y, Orientation orientation, int **map) {
    int car_id = map[Y][X];
    if (orientation == HORIZONTAL)
        while (map[Y - 1][X] == car_id) Y--;
    else if (orientation == VERTICAL)
        while (map[Y][X - 1] == car_id) X--;
}

void fix_car_pos(int **map, int height, int width, int &X, int &Y, Direction &dir, int &L, int car_id) {
    int prev_X = X;
    int prev_Y = Y;
    Orientation orient = get_orient(dir);
    auto tmp = find_car(map, height, width, car_id, {X, Y}, orient);
    X = tmp.first;
    Y = tmp.second;
    int prev_axis = orient == VERTICAL ? prev_X : prev_Y;
    int actual_axis = orient == VERTICAL ? X : Y;
    if (prev_axis != actual_axis) {
        if (prev_axis < actual_axis) {
            if (dir == LEFT || dir == UP) L -= (actual_axis - prev_axis);
            else L += (actual_axis - prev_axis);
        } else {
            if (dir == LEFT || dir == UP) L += (actual_axis - prev_axis);
            else L -= (actual_axis - prev_axis);
        }
        if (L < 0) {
            dir = oposite_direction(dir);
            L *= -1;
        }
    }
}


int main(int argc, char *argv[]) {
    int width, height, moves;
    width = atoi(argv[1]);
    height = atoi(argv[2]);
    moves = atoi(argv[3]);
    char **map_str = &argv[4];
    int L;
    pair *start_pos = new pair(0, 0);
    Orientation start_orient;
    cars_info info;
    int **map = create_map(map_str, height, width, start_pos, start_orient);
    info.add_cars(map, height, width);
    Direction dir_to_move;
    if (start_orient == HORIZONTAL) {
        if (map[height - 1][start_pos->first] == 0) {
            L = height - start_pos->second;
            dir_to_move = DOWN;
        }
        while (map[start_pos->second - 1][start_pos->first] == -1) start_pos->second--;
        if (dir_to_move != DOWN && map[0][start_pos->first] == 0) {
            L = start_pos->second;
            dir_to_move = UP;
        }
    } else if (start_orient == VERTICAL) {
        if (map[start_pos->second][width - 1] == 0) {
            L = width - start_pos->first;
            dir_to_move = RIGHT;
        }
        while (map[start_pos->second][start_pos->first - 1] == -1) start_pos->first--;
        if (dir_to_move != RIGHT && map[start_pos->second][0] == 0) {
            L = start_pos->first;
            dir_to_move = LEFT;
        }
    }

    std::cout << "\033[34m";
    std::cout << width << " " << height << " " << moves << std::endl;
    print_map(map, height, width);
    info.print_save_places();
    std::cout << "\033[0m";
    std::list<map_scored> start_root;

    start_root.emplace_back(map, 0);
    delete start_pos;
//    std::cout << "\033[34m" << (std::get<1>(res)) << "\033[0m" << std::endl;
//    for (const auto &move: res.moves_list)
//        std::cout << "\033[34m" << move << "\033[0m" << std::endl;
//        std::cout << move << std::endl;
    if (map != nullptr)
        clear_map(map, height);
}
