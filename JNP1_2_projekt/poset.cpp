#include <iostream>
#include <unordered_set>
#include <set>
#include <queue>
#include "poset.h"
#include <cassert>
#include <unordered_map>
#include <map>
#include <cstring>

using string = std::string;

using relations = std::map<string, bool>;
using poset = std::unordered_map<string, relations>;
using all_postes = std::unordered_map<unsigned long, poset>;

unsigned long free_ids_beg = 0;

namespace {
    std::queue<unsigned long> &the_free_ids() {
        static std::queue<unsigned long> free_ids;
        return free_ids;
    }

    all_postes &the_postes() {
        static all_postes posets;
        return posets;
    }

    bool poset_exists(unsigned long id) {
        return the_postes().find(id) != the_postes().end();
    }
    /* Jeżeli w zbiorze posetów wskazywanym przez the_postes() istnieje poset o
    * danym przez id identyfikatorze, zwraca true. False wpp.*/

    unsigned long choose_new_id() {
        if (the_free_ids().empty()) {
            free_ids_beg++;
            return free_ids_beg - 1;
        }
        unsigned long id = the_free_ids().front();
        the_free_ids().pop();
        return id;
    }
    /* Wybiera id dla nowego posetu spośród wolnych id.*/

    bool is_in_poset(const char *element, unsigned long id) {
        return poset_exists(id) && the_postes()[id].count(string(element)) > 0;
    }
    /* Sprawdza, czy napis wskazywany przez element jest w posecie id.
    * Jeżeli poset o danym id nie istnieje, zrwaca false. */

    void switch_edges(unsigned long id, char const *value) {
        if (!poset_exists(id) || !is_in_poset(value, id))
            return;

        relations r = the_postes()[id][string(value)];

        for (auto &[name, direction] : r)
            if (direction == 0)
                for (auto &[name2, direction2] : r)
                    if (direction2 == 1)
                        jnp1::poset_add(id, name.c_str(), name2.c_str());

    }
    /* Dla wszystkich poprzedników value daje krawędź do wszystkich
    *  jego następników.*/

    bool test_DFS(unsigned long id, char const *value1, char const *value2) {
        string previous = value1;
        string next = value2;
        if (previous.compare(next) == 0)
            return true;

        relations r = the_postes()[id][previous];
        for (auto &[name, direction] : r) {
            if (next.compare(name) == 0 && direction == 1)
                return true;
        }

        for (auto &[name, direction] : r) {
            if (direction == 1) {
                if (test_DFS(id, name.c_str(), value2))
                    return true;
            }
        }

        return false;
    }
    /* Funkcja potrzebna do wykonania poset_test. Przechodzi poset
    *  rekurencyjnie (tak jak DFS) w celu sprawdzenia czy istnieje
    *  połączenie z value1 do value2.*/

    bool poset_test_direct(unsigned long id, char const *value1,
                           char const *value2) {
        string previous = value1;
        string next = value2;
        relations r = the_postes()[id][previous];

        for (auto &[name, direction] : r) {
            if (next.compare(name) == 0 && direction == 1)
                return true;
        }
        return false;
    }
    /* Sprawdza czy value2 jest bezpośrednim następnikiem value1.*/
}

extern "C" {
    unsigned long poset_new() {
        poset p;
        unsigned long id = choose_new_id();
        the_postes().insert(std::make_pair(id, p));
        assert(poset_exists(id));
        return id;
    }

    void poset_delete(unsigned long id) {
        if (poset_exists(id)) {
            the_postes().erase(id);
            the_free_ids().push(id);
        }
    }

    size_t poset_size(unsigned long id) {
        if (poset_exists(id)) {
            poset p = the_postes()[id];
            return p.size();
        }
        assert(!poset_exists(id));
        return 0;
    }

    bool poset_insert(unsigned long id, char const *value) {
        if (value == nullptr || !poset_exists(id) || is_in_poset(value, id))
            return false;
        poset *p = &(the_postes()[id]);
        std::string insert_value = string(value);
        p->insert(std::make_pair(insert_value, std::map<string, bool>()));
        assert(is_in_poset(value, id));
        return true;
    }

    bool poset_remove(unsigned long id, char const *value) {
        if (!poset_exists(id) || !is_in_poset(value, id))
            return false;

        switch_edges(id, value);

        relations r = the_postes()[id][string(value)];

        for (auto &p : r) {
            relations *pom = &(the_postes()[id][p.first]);
            (*pom).erase(string(value));
        }
        poset *p = &(the_postes()[id]);
        p->erase(string(value));
        assert(!is_in_poset(value, id));
        return true;
    }

    bool poset_add(unsigned long id, char const *value1, char const *value2) {
        if (value1 == nullptr || value2 == nullptr || !poset_exists(id) ||
            !is_in_poset(value1, id) || !is_in_poset(value2, id))
            return false;

        if (jnp1::poset_test(id, value1, value2))
            return false;
        if (jnp1::poset_test(id, value2, value1))
            return false;

        poset *p = &(the_postes()[id]);
        string previous = value1;
        string next = value2;

        if (previous.compare(next) == 0)
            return false;

        for (auto &[node, relation] : (*p)) {
            if (previous.compare(node) == 0) {
                relation.insert({next, 1});
            }
            if (next.compare(node) == 0) {
                relation.insert({previous, 0});
            }
        }

        assert(jnp1::poset_test(id, value1, value2));
        return true;
    }

    bool poset_del(unsigned long id, char const *value1, char const *value2) {
        if (value1 == nullptr || value2 == nullptr || !poset_exists(id) ||
            !is_in_poset(value1, id) || !is_in_poset(value2, id))
            return false;

        if (!poset_test_direct(id, value1, value2))
            return false;

        poset *p = &(the_postes()[id]);
        string previous = value1;
        string next = value2;

        if (previous.compare(next) == 0)
            return false;

        //Usuwam krawędzie z previous do next i z next to previous.
        auto it_poset = (*p).find(previous);
        if (it_poset != (*p).end()) {
			auto it_relation = it_poset->second.find(next);
			if (it_relation != it_poset->second.end() && it_relation->second == 1) {
				it_poset->second.erase(it_relation->first);
			}
		}

		it_poset = (*p).find(next);
        if (it_poset != (*p).end()) {
			auto it_relation = it_poset->second.find(previous);
			if (it_relation != it_poset->second.end() && it_relation->second == 0) {
				it_poset->second.erase(it_relation->first);
			}
		}

        //Dla każdego wierzchołka do którego prowadzi krawędź z next, dodaje
        //krawędź prowadzącą od previous, żeby nie przerwać przechodniości
        it_poset = (*p).find(next);
        if (it_poset != (*p).end()) {
            for (auto &[name, direction] : it_poset->second) {
                if (direction == 1) {
                    poset_add(id, previous.c_str(), name.c_str());
                }
            }
        }

        assert(!jnp1::poset_test(id, value1, value2));
        return true;
    }

    bool poset_test(unsigned long id, char const *value1, char const *value2) {
        if (value1 == nullptr || value2 == nullptr || !poset_exists(id) ||
            !is_in_poset(value1, id) || !is_in_poset(value2, id))
            return false;

        return test_DFS(id, value1, value2);
    }

    void poset_clear(unsigned long id) {
        if (!poset_exists(id))
            return;

        poset *p = &(the_postes()[id]);

        for (auto &r : (*p)) {
            r.second.clear();
        }

        (*p).clear();
    }
}


int main() {

    unsigned long id = poset_new();

    for (int i = 0; i < 128; i++) {
        string s = string(1, (char(i)));
        poset_insert(id, s.c_str());

        if (i == 0)
            continue;
        poset_add(id, string(1, (char(i-1))).c_str(),s.c_str());
    }

    assert(poset_test(id, "a", "a"));
    /*
    for (int i = 0; i < 128; i++) {
        for (int j = i + 1; j < 128; j++) {
            assert(poset_del(id, string(1, (char(i))).c_str(), string(1, (char
                    (j)))
                    .c_str()));
        }
    }

    for (int i = 0; i < 128; i++) {
        for (int j = i + 1; j < 128; j++) {
            assert(!poset_test(id, string(1, (char(i))).c_str(), string(1, (char
                    (j)))
                    .c_str()));
        }
    }
     */
    return 0;
}
