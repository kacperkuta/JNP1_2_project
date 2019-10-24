#include <iostream>
#include <unordered_set>
#include <set>
#include <queue>
#include "poset.h"
#include <cassert>
#include <unordered_map>
#include <map>

using relation = std::map<char const*, std::set<std::map<char const*, bool>>>;
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

void delete_poset(unsigned long id) {
    if (poset_exists(id)) {
        posets.erase(id);
    }
}

size_t poset_size(unsigned long id) {
    if (poset_exists(id)) {
        poset p = posets[id];
        return p.size();
    }
    assert(!poset_exists(id));
    return 0;
}

bool is_in_poset(const char* element, poset* p) {
    for (auto &r : *p) {
        if (r.first == element)
            return true;
    }
    return false;
}

bool poset_insert(unsigned long id, char const *value) {
    if (!poset_exists(id) || is_in_poset(value, &(posets[id])))
        return false;
    poset* p = &(posets[id]);
    relation r;
    r.first = value;
    p->insert(r);
    assert(is_in_poset(value, p));
    return true;
}

bool poset_remove(unsigned long id, char const* value) {
    if (!poset_exists(id) || !is_in_poset(value, &(posets[id])))
        return false;

    poset* p = &(posets[id]);
    relation* r = p->find(value);
    for ()
}


int main() {

    return 0;
}
