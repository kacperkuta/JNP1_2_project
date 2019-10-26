#include <iostream>
#include <unordered_set>
#include <set>
#include <queue>
#include "poset.h"
#include <cassert>
#include <unordered_map>
#include <map>
#include <string.h>

using string = std::string;

using relations = std::map<string, bool>;
using poset = std::map<string, relations>;
using all_posets = std::unordered_map<unsigned long, poset>;

all_posets posets;
unsigned long free_ids_beg = 0;
std::queue<unsigned long> free_ids;

bool poset_exists(unsigned long id) {
    return posets.find(id) != posets.end();
}
/* Jeżeli w zbiorze posetów wskazywanym przez posets istnieje poset o
 * danym przez id identyfikatorze, zwraca true. False wpp.*/

unsigned long choose_new_id() {
    if (free_ids.empty()) {
        free_ids_beg++;
        return free_ids_beg - 1;
    }
    unsigned long id = free_ids.front();
    free_ids.pop();
    return id;
}
/* Wybiera id dla nowego posetu spośród wolnych id.*/

unsigned long poset_new() {
    poset p;
    unsigned long id = choose_new_id();
    posets.insert(std::make_pair(id, p));
    assert(poset_exists(id));
    return id;
}

void poset_delete(unsigned long id) {
    if (poset_exists(id)) {
        posets.erase(id);
        free_ids.push(id);
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

bool is_in_poset(const char* element, unsigned long id) {
    return poset_exists(id) && posets[id].count(string(element)) > 0;
}
/* Sprawdza, czy napis wskazywany przez element jest w posecie id.
 * Jeżeli poset o danym id nie istnieje, zrwaca false. */

bool poset_insert(unsigned long id, char const *value) {
    if (!poset_exists(id) || is_in_poset(value, id))
        return false;
    poset* p = &(posets[id]);
    std::string insert_value = string(value);
    p->insert(std::make_pair(insert_value, std::map<string, bool>()));
    assert(is_in_poset(value, id));
    return true;
}

bool poset_remove(unsigned long id, char const* value) {
    if (!poset_exists(id) || !is_in_poset(value, id))
        return false;

    relations r = posets[id][string(value)];

    for (auto &p : r) {
        relations* pom = &(posets[id][p.first]);
        (*pom).erase(string(value));
    }
    poset* p = &(posets[id]);
    p->erase(string(value));
    assert(!is_in_poset(value, id));
    return true;
}

// Narazie bez domknięcia przechodniego.
bool poset_add(unsigned long id, char const *value1, char const *value2) {
    if(!poset_exists(id) || !is_in_poset(value1, id) || !is_in_poset(value2, id))
        return false;

    if(poset_test(id, value1, value2))
        return false;

    poset* p = &(posets[id]);
    string previous = value1;
    string next = value2;

    for(auto &[node, relation] : (*p)) {
        if(previous.compare(node) == 0) {
            relation.insert({next, 1});
        }
        if(next.compare(node) == 0) {
            relation.insert({previous, 0});
        }
    }

    assert(poset_test(id, value1, value2));
    return true;
}


bool poset_del(unsigned long id, char const *value1, char const *value2) {
    if(!poset_exists(id) || !is_in_poset(value1, id) || !is_in_poset(value2, id))
        return false;

    if(!poset_test(id, value1, value2))
        return false;

    poset* p = &(posets[id]);
    string previous = value1;
    string next = value2;

    for(auto &[node, relation] : (*p)) {
        if(previous.compare(node) == 0) {
            for (auto &[name, direction] : relation) {
                if(next.compare(name) == 0 && direction == 1) {
                    relation.erase(name);
                }
            }
        }

        if(next.compare(node) == 0) {
            for (auto &[name, direction] : relation) {
                if(previous.compare(name) == 0 && direction == 0) {
                    relation.erase(name);
                }
            }
        }
    }

    assert(!poset_test(id, value1, value2));
    return true;
}


bool poset_test(unsigned long id, char const *value1, char const *value2) {
    if(!poset_exists(id) || !is_in_poset(value1, id) || !is_in_poset(value2, id))
        return false;

    poset p = posets[id];
    string previous = value1;
    string next = value2;

    if(previous.compare(next) == 0)
        return true;

    for (auto &[node, relation] : p) {
        if(previous.compare(node) == 0) {
            for (auto &[name, direction] : relation) {
                if(next.compare(name) == 0 && direction == 1)
                    return true;
            }
        }
    }
    return false;
}

void poset_clear(unsigned long id) {
    if(!poset_exists(id))
        return;

    poset* p = &(posets[id]);

    for (auto &[node, relation] : (*p)) {
        relation.clear();
    }

    (*p).clear();
}
/* Jeżeli istnieje poset o identyfikatorze id, usuwa wszystkie jego elementy
 * oraz relacje między nimi, a w przeciwnym przypadku nic nie robi.*/


int main() {

    unsigned long id = poset_new();
    assert(poset_exists(id));

    poset_insert(id, "kacper");
    poset_insert(id, "marta");

    assert(is_in_poset("kacper", id));
    assert(is_in_poset("marta", id));
    assert(poset_size(id)== 2);

    poset_remove(id, "kacper");

    assert(!is_in_poset("kacper", id));
    assert(is_in_poset("marta", id));
    assert(poset_size(id)== 1);

    poset_delete(id);
    assert(!poset_exists(id));

    unsigned long id2 = poset_new();

    assert(poset_size(id2) == 0);

    poset_delete(id);

    unsigned long id3 = poset_new();

    assert(poset_size(id3) == 0);
    assert(id3 == 0);
    return 0;
}