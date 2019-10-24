#include <iostream>
#include <unordered_set>
#include <set>
#include <queue>
#include "poset.h"
#include <cassert>
#include <unordered_map>

using relation = std::pair<char const*, std::unordered_set<char const*>>;
using poset = std::set<relation>;
using all_posets = std::unordered_map<unsigned long, poset>;

all_posets posets;
unsigned long free_ids_beg = 0;
std::queue<unsigned long> free_ids;


bool poset_exists(unsigned long id) {
    return posets.find(id) != posets.end();
}

/*Jeżeli w zbiorze posetów wskazywanym przez posets istnieje poset o
  danym przez id identyfikatorze, zwraca true. False wpp.*/

unsigned long choose_new_id() {
    if (free_ids.empty()) {
        free_ids_beg++;
        return free_ids_beg - 1;
    }
    unsigned long id = free_ids.front();
    free_ids.pop();
    return id;
}

/* Wybiera id dal nowego posetu spośród wolnych id.*/

unsigned long poset_new() {
    poset p;
    unsigned long id = choose_new_id();
    posets.insert(std::make_pair(id, p));
    assert(poset_exists(id));
    return id;
}

size_t poset_size(unsigned long id) {
    if (poset_exists(id)) {
        poset p = posets[id];
        return p.size();
    }
    return 0;
}


int main() {

    return 0;
}
