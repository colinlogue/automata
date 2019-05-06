#ifndef GARDEN_H
#define GARDEN_H

#include <array>
#include <vector>

using namespace std;

// constants
static const char r_filter = 0x30;
static const char g_filter = 0xC;
static const char b_filter = 0x3;
static const char rgb_filter = 0x3F;
static const int n_neighbors = 8;
static const int n_colors = 3;
static const int len_ruleset = 4;

// classes
class Rule {
public:
    bool check_activation(array<char, n_neighbors>);
    Rule(array<char, len_ruleset>);
private:
    bool check_color(char, char);
    bool check_matches(int);

    char self_color_filter;
    // defines what the cell's color state needs to be for the
    // rule to activate. VALS is the cell's 6-bit color values
    // 00<------> : any 
    // 01<RRGGBB> : activate if none of VALS is more than RRGGBB
    // 10<RRGGBB> : activate if none of VALS is less than RRGGBB
    // 11<RRGGBB> : actiavte only if VALS match RRGGBB exactly
    
    char position_filter;
    // mask to detemine which neighboring cells are checked.
    // least significant bit is top left and moves clockwise
    // from there. 1 is check, 0 is not

    char color_filter;
    // defines what colors activate the rule if they are found
    // at the filtered positions. VALS is the 6-bit color values
    // at the position.
    // 00<------> : any 
    // 01<RRGGBB> : activate if none of VALS is more than RRGGBB
    // 10<RRGGBB> : activate if none of VALS is less than RRGGBB
    // 11<RRGGBB> : activate only if VALS match RRGGBB exactly

    char number_filter;
    // how many color/position matches are needed to activate rule
    // <-->00<----> : any matches
    // <-->01<####> : at least #### matches
    // <-->10<####> : at most #### matches
    // <-->11<####> : exactly #### matches

    // results
    // -------
};

class Cell {
public:
    char get_state();
    bool is_out_of_bounds();
    void set_neighbor_states(array<char, n_neighbors>);
    void set_out_of_bounds(bool);
    void set_ruleset(vector<Rule> *);
    void set_state(char);
    // constructor and destructor
    Cell();
    Cell(char, bool oob=false);
private:
    char state;
    array<char, n_neighbors> neighbor_states;
    vector<Rule> *ruleset;
    bool out_of_bounds = false;
};

static Cell * const CELL_OOB = new Cell(0, true);

class WorldRow {
    // this is a helper class for working with the overloaded
    // subscript operator of Worlds in two dimensions
public:
    Cell &operator[] (const  int);
    Cell *cell_row;
    int size_y;
    WorldRow(Cell *, int);
};

static const WorldRow ROW_OOB(CELL_OOB, 0);

class World {
public:
    void add_rule(Rule &);
    void add_rule(array<char, len_ruleset>);
    int get_size_x();
    int get_size_y();
    array<int, 2> get_size_xy();
    Cell &get_cell_at(int, int);
    void step();
    void update_cell_neighbor_states(int, int);
    // constructor and destructor
    World(int, int);
    ~World();
    // operator overloading
    WorldRow operator[] (const int);
private:
    int size_x, size_y, t;
    Cell **cells;
    void init_cell_array();
    void destroy_cell_array();
    vector<Rule> world_rules;
};

#endif