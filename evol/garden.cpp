#include "garden.h"
#include <cassert>

using namespace std;

// Rule public functions
bool Rule::check_activation(array<char, n_neighbors> state) {
    // state is array of chars in the following format:
    // state[0] : <--><RRGGBB> cell's current color state
    // state[1-8] : <--><RRGGBB> state of each of the
    //     neighboring cells, starting from top left

    // 1) return false if color doesn't match
    if (!check_color(state[0], self_color_filter)) return false;

    // 2) loop through neighboring cells, finding matches
    int n_matches = 0;
    for (int i=1; i <= n_neighbors; i++) {
        char filter = 1 << (i-1);
        if ((state[i] & filter) && check_color(state[i], color_filter)) {
            ++n_matches;
        }
    }

    // 3) activate if correct number of matches
    return check_matches(n_matches);
}

Rule::Rule(array<char, len_ruleset> init_vals) {
    self_color_filter = init_vals[0];
    position_filter = init_vals[1];
    color_filter = init_vals[2];
    number_filter = init_vals[3];
}

// Rule private functions
bool Rule::check_color(char state, char color_filter) {
    // 1) determine color check mode
    char filter_type = color_filter & ~rgb_filter;
    if (filter_type == 0) return true;

    array<int, n_colors> rgb_vals = {
        state & r_filter,
        state & g_filter,
        state & b_filter
    };

    array<int, n_colors> filter_vals = {
        color_filter & r_filter,
        color_filter & g_filter,
        color_filter & b_filter
    };

    if (filter_type == 1) {
        for (int i=0; i < 3; ++i) {
            if (rgb_vals[i] > filter_vals[i]) return false;
        }
        return true;
    }

    if (filter_type == 2) {
        for (int i=0; i < 3; ++i) {
            if (rgb_vals[i] < filter_vals[i]) return false;
        }
        return true;
    }

    if (filter_type == 3) {
        for (int i=0; i < 3; ++i)  {
            if (!(rgb_vals[i] == filter_vals[i])) return false;
        }
        return true;
    }

    throw("Error in check_color function");
}
bool Rule::check_matches(int n_matches) {
    char filter_type = number_filter & ~rgb_filter;
    if (filter_type == 0) return (bool) n_matches;

    int threshold = number_filter & rgb_filter;
    if (filter_type == 1) return n_matches >= threshold;
    if (filter_type == 2) return n_matches <= threshold;
    if (filter_type == 3) return n_matches == threshold;

    throw("Error in check_matches function");
}

// Cell public functions
void Cell::add_rule(array<char, len_ruleset> new_rule) {
    ruleset.push_back(Rule(new_rule));
}
char Cell::get_state() {
    return state;
}
bool Cell::is_out_of_bounds() {
    return out_of_bounds;
}
void Cell::set_neighbor_states(array<char, n_neighbors> states) {
    neighbor_states = states;
}
void Cell::set_out_of_bounds(bool oob) {
    out_of_bounds = oob;
}
void Cell::set_state(char state_val) {
    state = state_val;
}
Cell::Cell() {
    state = 0x00;
}
Cell::Cell(char state_val, bool oob) {
    state = state_val;
    if (oob) out_of_bounds = true;
}
// Cell private 


// World public
void World::add_rule(array<char, len_ruleset> new_ruleset) {
    world_rules.push_back(Rule(new_ruleset));
}
void World::add_rule(Rule &new_rule) {
    world_rules.push_back(new_rule);
}
int World::get_size_x() {
    return size_x;
}
int World::get_size_y() {
    return size_y;
}
array<int, 2> World::get_size_xy() {
    return array<int, 2> {size_x, size_y};
}
Cell &World::get_cell_at(int x, int y) {
    return cells[x][y];
}
void World::step() {
    for (int i = 0; i < size_x; ++i) {
        for (int j = 0; j < size_y; ++j) {
            update_cell_neighbor_states(i, j);
        }
    }
}
void World::update_cell_neighbor_states(int x, int y) {
    array<char, n_neighbors> states;
    for (int i = (x-1); i < (x+1); ++i) {
        for (int j = (y-1); j < (y+1); ++j) {
            states[i + j] = get_cell_at(x, y).get_state();
        }
    }
    cells[x][y].set_neighbor_states(states);
}
World::World(int rows, int cols) {
    size_x = rows;
    size_y = cols;
    t = 0;

    cells = NULL;
    init_cell_array();
}
World::~World() {
    destroy_cell_array();
}

// World private
void World::init_cell_array() {
    destroy_cell_array();
    cells = new Cell*[size_x];
    for (int i = 0; i < size_x; ++i) {
        cells[i] = new Cell[size_y];
    }
}
void World::destroy_cell_array() {
    if (cells) {
        for (int i = 0; i < size_x; ++i) {
            delete[] cells[i];
        }
        delete[] cells;
        cells = NULL;
    }
}

// WorldRow
WorldRow World::operator[] (const int row) {
    if (row == -1 || row == size_x) return ROW_OOB;
    assert(row >= 0 && row < size_y);
    if (!cells) throw("Cell array uninitialized");
    return WorldRow(cells[row], size_y);
}
WorldRow::WorldRow(Cell *cell_row, int size_y) {
    this->cell_row = cell_row;
    this->size_y = size_y;
}
Cell &WorldRow::operator[] (const int col) {
    if (col == -1 || col == size_y) return *CELL_OOB;
    assert(col >= 0 && col < size_y);
    return cell_row[col];
}