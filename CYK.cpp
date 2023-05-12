#include <iostream>
#include <string>
#include "Grammar_FNC.h"
#include "color.hpp"
int main () {
    Grammar_FNC grammar; // a^n c^m b^n
    grammar.addProd({0, 0, 0});
    grammar.addUnitProd({0, 'c'});
    grammar.addProd({0, 1, 3});
    grammar.addProd({0, 1, 2});
    grammar.addProd({3, 0, 2});
    grammar.addUnitProd({1, 'a'});
    grammar.addUnitProd({2, 'b'});
    while(true) {
        std::string input;
        std::cin>>input;
        if(input == "exit") break;
        if(grammar.cyk_check(input, 0))
            color("green", "Accepted!", true);
        else
            color("red", "Rejected!", true);
    }
    return 0;
}
