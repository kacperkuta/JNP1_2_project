#include <iostream>
#include <unordered_set>
#include <set>
#include <queue>
#include "poset.h"
#include <cassert>

using relation = std::pair<char const*, std::unordered_set<char const*>>;
using poset = std::pair<unsigned long, std::set<relation>>;
using all_posets = std::set<poset>;

all_posets posets;
unsigned long free_ids_beg = 0;
std::queue<unsigned long> free_ids;


bool poset_exists(unsigned long id) {
    for (auto &p : posets) {
        if (p.first == id)
            return true;
    }
    return false;
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
    p.first = id;
    posets.insert(p);
    assert(poset_exists(id));
    return id;
}
poset find_poset(unsigned long id) {
    for (auto itr = posets.begin(); itr != posets.end(); itr++) {
        if ((*itr).first == id)
            return *itr;
    }
    return *(posets.end());
}

/*Znajduje poset o danym id, zwraca iterator do posets na niego, albo iterator
  na koniec posets, gdy nie ma takiego posetu.*/

size_t poset_size(unsigned long id) {
    if (poset_exists(id)) {
        poset p = find_poset(id);

    }
}


int main() {

    return 0;
}
