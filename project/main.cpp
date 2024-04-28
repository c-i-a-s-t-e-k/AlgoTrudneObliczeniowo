#include <iostream>
#include <cstdlib>
#include <list>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

enum Orientation {
    VERTICAL,
    HORIZONTAL
};

enum PathState {
    GOOD,
    NOT_ENOUGH_MOVES,
    NEED_TO_CYCLE_CAR_MOVE,
    CAN_NOT_MOVE_HERE
};

class MapScored {
public:
    int **map;
    int score;
    std::list<std::string> moves_list;
    PathState state;

public:
    MapScored(int **map, int initial_score, std::list<std::string> other_move_list, PathState state) : map(map),
                                                                                                       score(initial_score),
                                                                                                       state(state) {
        moves_list.splice(moves_list.begin(), other_move_list);
    }

    MapScored(int **map, int initial_score, PathState state) : map(map),
                                                               score(initial_score),
                                                               state(state) {}
};


class pair {
public:
    int first;
    int second;

    int get(int **map) {
        return map[second][first];
    }

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

struct triplet {
    Direction dir;
    int L;
    pair car_pos;
};

class CarsInfo {
private:
    std::unordered_map<int, Orientation> cars_orientation;
    std::unordered_map<int, int> car_lengths;
    std::unordered_map<int, std::list<pair>> safe_palaces;

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

    bool have_safe_place(int car_id) {
        return !safe_palaces[car_id].empty();
    }

    std::list<pair> get_safe_places(int car_id) {
        return {this->safe_palaces[car_id]};
    }

    void add_cars(int **map, int height, int width) {
        std::unordered_map<int, std::unordered_set<int>> car_on_horizontal_line;
        std::unordered_map<int, std::unordered_set<int>> car_on_vertical_line;
        std::unordered_set<int> cars;
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
        std::vector<std::vector<std::unordered_set<int>>> presence_table(height - 2,
                                                                         std::vector<std::unordered_set<int>>(
                                                                                 width - 2));
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
                            flag = flag && x < width - 2 &&
                                   (presence_table[y_][x].size() == 1 && *presence_table[y_][x].begin() == car_id);
                        }
                    else
                        for (int y = y_ + 1; flag && y < y_ + car_lengths[car_id]; y++) {
                            flag = flag && y < height - 2 &&
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
    if (map_source == nullptr) throw std::runtime_error("you try to copy cleared map");
    for (int i = 0; i < height; i++) {
        map[i] = new int[width];
        for (int j = 0; j < width; j++) {
            map[i][j] = map_source[i][j];
        }
    }
    return map;
}

std::vector<pair> get_cars_to_move(int **map, int X, int Y, Direction dir, int L, CarsInfo &info) {
    std::vector<pair> res;
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
        int new_X = X, new_Y = Y;
        set_car_pos(new_X, new_Y, info.get_car_orient(map[Y][X]), map);
        res.emplace_back(new_X, new_Y);
    }
    return res;
}

bool is_road_free(int **map, pair start, pair end, Orientation orient) {
    int car_id = map[start.second][start.first];
    int s, e;
    if (orient == VERTICAL) {
        s = start.first < end.first ? start.first : end.first;
        e = start.first < end.first ? end.first : start.first;
        for (int i = s; i <= e; i++)
            if (map[start.second][i] != 0 && map[start.second][i] != car_id) return false;
    } else {
        s = start.second < end.second ? start.second : end.second;
        e = start.second < end.second ? end.second : start.second;
        for (int i = s; i <= e; i++)
            if (map[i][start.first] != 0 && map[i][start.first] != car_id) return false;
    }
    return true;
}

// can optimize
std::list<pair_dir_int> get_possible_moves(
        int **map, int height, int width, int X, int Y, pair hit_point, CarsInfo &info
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

void _make_move(int **map, int X, int Y, Direction dir, int L) {
    int car_id = map[Y][X];
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

pair get_hit_point(int **map, int X, int Y, pair next_car, CarsInfo &info) {
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

std::list<pair_dir_int> get_no_collision_moves(int **map, int X, int Y, CarsInfo &info) {
    int car_id = map[Y][X];
    int car_len = info.get_car_len(car_id);
    Orientation orientation = info.get_car_orient(car_id);
    int i;
    std::list<pair_dir_int> moves;
    if (orientation == VERTICAL) {
        i = X;
        while (map[Y][--i] == 0)
            moves.emplace_back(LEFT, X - i);
        i = X;
        while (map[Y][++i + car_len - 1] == 0)
            moves.emplace_back(RIGHT, i - X);
    } else {
        i = Y;
        while (map[--i][X] == 0)
            moves.emplace_back(UP, Y - i);
        i = Y;
        while (map[++i + car_len - 1][X] == 0)
            moves.emplace_back(DOWN, i - Y);
    }
    return moves;
}

std::list<MapScored>
all_paths(int **map, int height, int width, CarsInfo &info, int X, int Y, Direction dir, int L, int score,
          std::list<std::string> list_moves, int moves, std::unordered_set<int> cars_before) {
    int car_id = map[Y][X];
    auto cars_to_move = get_cars_to_move(map, X, Y, dir, L, info);
    cars_before.insert(map[Y][X]);

    //  not enough moves to clear road
    if (cars_to_move.size() + score > moves) {
        clear_map(map, height);
        list_moves.clear();
        return {{map, INT16_MAX, NOT_ENOUGH_MOVES}};
    }
    //    sprawdzenie czy jakieś auto  da się przesunąć na "bezpieczne miejsce"
    for (auto car_pos: cars_to_move) {
        int other_car_id = car_pos.get(map);
        //  edge cond - cycle moves
        if (cars_before.find(other_car_id) != cars_before.end()) {
            clear_map(map, height);
            list_moves.clear();
            return {{map, INT16_MAX, NEED_TO_CYCLE_CAR_MOVE}};
        }
        if (info.have_safe_place(other_car_id)) {
            for (auto safe_place: info.get_safe_places(other_car_id)) {
//                print_map(map,height,width);
                if (is_road_free(map, car_pos, safe_place, info.get_car_orient(other_car_id))) {
                    Orientation other_car_orient = info.get_car_orient(other_car_id);
                    int other_car_L =
                            other_car_orient == VERTICAL ?
                            abs(car_pos.first - safe_place.first) : abs(car_pos.second - safe_place.second);
                    Direction other_car_dir = other_car_orient == VERTICAL ?
                                              car_pos.first < safe_place.first ? RIGHT : LEFT :
                                              car_pos.second < safe_place.second ? DOWN : UP;
                    _make_move(map, car_pos.first, car_pos.second, other_car_dir, other_car_L);
                    list_moves.push_back(
                            "" + std::to_string(car_pos.first) + " " + std::to_string(car_pos.second) + " " +
                            get_char(other_car_dir) + " " + std::to_string(other_car_L)
                    );
                    score++;
                    break;
                }
            }

        }
    }
//    CAN BE OPTIMALIZED
    cars_to_move.clear();
    cars_to_move = get_cars_to_move(map, X, Y, dir, L, info);
//    END
    //  road is cleared;
    if (cars_to_move.empty()) {
        _make_move(map, X, Y, dir, L);
        list_moves.push_back(
                "" + std::to_string(X) + " " + std::to_string(Y) + " " + get_char(dir) + " " + std::to_string(L)
        );
        score++;
        return {{map, score, list_moves, GOOD}};
    }


    std::list<triplet> available_moves_to_clear_road;
    for (auto car_pos: cars_to_move) {
        pair hit_point = get_hit_point(map, X, Y, car_pos, info);
        for (auto elem: get_possible_moves(map, height, width, car_pos.first, car_pos.second,
                                           hit_point, info)) {
            available_moves_to_clear_road.push_back({elem.first, elem.second, car_pos});
        }
    }

    std::list<MapScored> tmp_result;
    std::list<MapScored> result;
    for (auto move_to_clear: available_moves_to_clear_road) {
        tmp_result.splice(tmp_result.end(),
                          all_paths(map_copy(map, height, width), height, width, info, move_to_clear.car_pos.first,
                                    move_to_clear.car_pos.second, move_to_clear.dir, move_to_clear.L, score,
                                    {list_moves}, moves, {cars_before})
        );
    }

    for (auto tmp: tmp_result) {
        if (tmp.state == GOOD) {
            if (tmp.score < moves)
                result.splice(result.end(),
                              all_paths(map_copy(tmp.map, height, width), height, width, info, X,
                                        Y, dir, L, tmp.score,
                                        {tmp.moves_list}, moves, {cars_before})
                );
            clear_map(tmp.map, height);
            tmp.moves_list.clear();
        }
    }

    tmp_result.clear();
    int new_X, new_Y, new_L;
    for (auto move_pair: get_no_collision_moves(map, X, Y, info)) {
        int **new_map = map_copy(map, height, width);
        std::list<std::string> new_list_moves = {list_moves};
        _make_move(new_map, X, Y, move_pair.first, move_pair.second);
        new_list_moves.push_back("" + std::to_string(X) + " " + std::to_string(Y) + " " +
                                 get_char(move_pair.first) + " " + std::to_string(move_pair.second));
        new_L = dir == move_pair.first ? L - move_pair.second : L + move_pair.second;
        if (info.get_car_orient(map[Y][X]) == VERTICAL) {
            new_Y = Y;
            new_X = move_pair.first == LEFT ? X - move_pair.second : X + move_pair.second;
        } else {
            new_X = X;
            new_Y = move_pair.first == UP ? Y - move_pair.second : Y + move_pair.second;
        }

        for (auto move_to_clear: available_moves_to_clear_road) {
            tmp_result.splice(tmp_result.end(),
                              all_paths(map_copy(new_map, height, width), height, width, info,
                                        move_to_clear.car_pos.first,
                                        move_to_clear.car_pos.second, move_to_clear.dir, move_to_clear.L, score + 1,
                                        {new_list_moves}, moves, {cars_before})
            );
        }
        for (auto tmp: tmp_result) {
            if (tmp.state == GOOD) {
                if (tmp.score < moves)
                    result.splice(result.end(),
                                  all_paths(map_copy(tmp.map, height, width), height, width, info, new_X,
                                            new_Y, dir, new_L, tmp.score,
                                            {tmp.moves_list}, moves, {cars_before})
                    );
//                clear_map(tmp.map, height);
//                tmp.moves_list.clear();
            }
        }
        clear_map(new_map, height);
        new_list_moves.clear();
    }


//    clear_map(map, height);
//    list_moves.clear();
    return result;
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
    CarsInfo info;
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

//    std::cout << "\033[34m";
//    std::cout << width << " " << height << " " << moves << std::endl;
//    print_map(map, height, width);
////    info.print_save_places();
//    std::cout << "\033[0m";
    auto results = all_paths(map, height, width, info, start_pos->first, start_pos->second, dir_to_move, L - 1, 0, {},
                             moves, {});
    std::list<MapScored> res;
    for (auto result: results) {
        if (result.state == GOOD && result.score <= moves) {
//            std::cout << "------------------------" << std::endl;
//            std::cout << "\033[34m" << result.score << "\033[0m" << std::endl;
            for (const auto &move: result.moves_list)
//                std::cout << "\033[34m" << move << "\033[0m" << std::endl;
            std::cout << move << std::endl;break;
        }
//        clear_map(result.map, height);
//        result.moves_list.clear();
    }
    delete start_pos;

}
