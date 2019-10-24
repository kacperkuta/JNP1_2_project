#include <iostream>
#include <unordered_set>
#include <set>

#include "poset.h"

using relation = std::pair<char const*, std::unordered_set<char const*>>;
using poset = std::pair<unsigned long, std::set<relation>>;
using all_posets = std::set<poset>;

all_posets posets;

bool poset_exists(unsigned long id) {
    for (auto &p : posets) {
        if (p.first == id)
            return true;
    }
    return false;
}

/*Jeżeli w zbiorze posetów wskazywanym przez posets istnieje poset o
  danym przez id identyfikatorze, zwraca true. False wpp.*/


int main() {

    return 0;
}
