#include "garden.h"
#include <iostream>

using namespace std;

int main() {
    array<char, len_ruleset> ruleset = {0x00, static_cast<char>(0xFF), 0x00, 0x00};
    World garden(3, 3);
    Cell cell = garden[1][1];
    cell.add_rule(ruleset);
    cout << "Cell state: " << static_cast<unsigned>(cell.get_state()) << endl;
}