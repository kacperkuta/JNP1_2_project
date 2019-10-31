#include "poset.h"

#include <queue>
#include <cassert>
#include <unordered_map>
#include <map>
#include <cstring>


using string = std::string;
using relations = std::map<string, bool>;
using poset = std::unordered_map<string, relations>;
using all_posets = std::unordered_map<unsigned long, poset>;

#ifdef NDEBUG
    bool debug = {false};
#else
    bool debug = {true};
#endif

namespace {
    unsigned long& the_free_ids_beg() {
        static unsigned long free_ids_beg = 0;
        return free_ids_beg;
    }

    std::queue<unsigned long> &the_free_ids() {
        static std::queue<unsigned long> free_ids;
        return free_ids;
    }

    all_posets &the_posets() {
        static all_posets posets;
        return posets;
    }

    bool poset_exists(unsigned long id) {
        return the_posets().find(id) != the_posets().end();
    }
    /* Jeżeli w zbiorze posetów wskazywanym przez the_posets() istnieje poset o
    * danym przez id identyfikatorze, zwraca true. False wpp.*/

    unsigned long choose_new_id() {
        if (the_free_ids().empty()) {
            the_free_ids_beg()++;
            return the_free_ids_beg() - 1;
        }
        unsigned long id = the_free_ids().front();
        the_free_ids().pop();
        return id;
    }
    /* Wybiera id dla nowego posetu spośród wolnych id.*/

    bool is_in_poset(const char *element, unsigned long id) {
        return poset_exists(id) && the_posets()[id].count(string(element)) > 0;
    }
    /* Sprawdza, czy napis wskazywany przez element jest w posecie id.
    * Jeżeli poset o danym id nie istnieje, zrwaca false. */

    void switch_edges(unsigned long id, char const *value) {
        if (!poset_exists(id) || !is_in_poset(value, id))
            return;

        relations r = the_posets()[id][string(value)];

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

        relations r = the_posets()[id][previous];
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
        relations r = the_posets()[id][previous];

        for (auto &[name, direction] : r) {
            if (next.compare(name) == 0 && direction == 1)
                return true;
        }
        return false;
    }
    /* Sprawdza czy value2 jest bezpośrednim następnikiem value1.*/

    bool check_inquiry_correctness(string inquiry_name, unsigned long id,
            char const *value1, char const *value2) {
        if (debug) {
            std::cerr << inquiry_name << "(" << id << ", \""
                      << ((value1 == nullptr) ? "NULL" : string(value1))
                      << "\", \""
                      << ((value2 == nullptr) ? "NULL" : string(value2))
                      << "\")\n";
        }

        if (!poset_exists(id)) {
            if (debug) {
                std::cerr << inquiry_name << ": poset " << id
                          << " does not exist\n";
            }
            return false;
        } else if (value1 == nullptr) {
            if (debug) {
                std::cerr << inquiry_name << ": invalid value1 (NULL)\n";
            }
            if (value2 != nullptr)
                return false;
        }
        if (value2 == nullptr) {
            if (debug) {
                std::cerr << inquiry_name << ": invalid value2 (NULL)\n";
            }
            return false;
        } else if (!is_in_poset(value1, id) || !is_in_poset(value2, id)) {
            if (debug) {
                std::cerr << inquiry_name << ": poset " << id << ", element \""
                          << string(value1) << "\" or \"" << string(value2) <<
                          "\" does not exist\n";
            }
            return false;
        }
        return true;
    }
    /* Sprawdza poprawność zapytania w zakresie istnienia posetu, poprawości
     * wartości value1 i value2 oraz ich istnienia w posecie. */

    void poset_add_no_cerr(unsigned long id, const char* value1,
            const char* value2) {
        poset *p = &(the_posets()[id]);

        string previous = value1;
        string next = value2;

        for (auto &[node, relation] : (*p)) {
            if (previous.compare(node) == 0) {
                relation.insert({next, 1});
            }
            if (next.compare(node) == 0) {
                relation.insert({previous, 0});
            }
        }
    }
    /* Dodaje relację między istniejącymi elementami posetu. Nie wypisuje na
     * cerr informacji diagnostycznych.*/
}

extern "C" {
    unsigned long poset_new() {
        if (debug)
            std::cerr << "poset_new()\n";
        poset p;
        unsigned long id = choose_new_id();
        the_posets().insert(std::make_pair(id, p));
        assert(poset_exists(id));
        if (debug)
            std::cerr << "poset_new: poset " << id << " created\n";
        return id;
    }

    void poset_delete(unsigned long id) {
        if (debug)
            std::cerr << "poset_delete(" << id << ")\n";
        if (poset_exists(id)) {
            the_posets().erase(id);
            the_free_ids().push(id);
            if (debug)
                std::cerr << "poset_delete: poset " << id << " deleted\n";
        } else if (debug) {
            std::cerr << "poset_delete: poset " << id << " does not exist\n";
        }
    }

    size_t poset_size(unsigned long id) {
        if (debug)
            std::cerr << "poset_size(" << id << ")\n";
        if (poset_exists(id)) {
            poset p = the_posets()[id];
            if (debug) {
                std::cerr << "poset_size: poset " << id << " contains "
                          << p.size()
                          << " element(s)\n";
            }
            return p.size();
        }
        assert(!poset_exists(id));
        if (debug)
            std::cerr << "poset_size: poset " << id << " does not exist\n";
        return 0;
    }

    bool poset_insert(unsigned long id, char const *value) {
        if (debug) {
            std::cerr << "poset_insert(" << id << ", \""
                      << ((value == nullptr) ?
                          "NULL" : string(value)) << "\")\n";
        }
        if (value == nullptr) {
            if (debug)
                std::cerr << "poset_insert: invalid value (NULL)\n";
            return false;
        } else if (!poset_exists(id)) {
            if (debug)
                std::cerr << "poset_insert: poset " << id
                          << " does not exist\n";
            return false;
        } else if (is_in_poset(value, id)) {
            if (debug) {
                std::cerr << "poset_insert: poset " << id << ", element \""
                          << string(value) << "\" already exists\n";
            }
            return false;
        }
        poset *p = &(the_posets()[id]);
        string insert_value = string(value);
        p->insert(std::make_pair(insert_value, std::map<string, bool>()));
        assert(is_in_poset(value, id));
        if (debug) {
            std::cerr << "poset_insert: poset " << id << ", element \""
                      << string(value) << "\" inserted\n";
        }
        return true;
    }

    bool poset_remove(unsigned long id, char const *value) {
        if (debug) {
            std::cerr << "poset_remove(" << id << ", \""
                      << ((value == nullptr) ? "NULL" : string(value))
                      << "\")\n";
        }
        if (!poset_exists(id)) {
            if (debug)
                std::cerr << "poset_remove: poset " << id << " does not exist\n";
            return false;
        } else if (value == nullptr) {
            if (debug)
                std::cerr << "poset_remove: invalid value (NULL)\n";
            return false;
        } else if (!is_in_poset(value, id)) {
            if (debug) {
                std::cerr << "poset_remove: poset " << id << ", element \""
                          << string(value) << "\" does not exist\n";
            }
            return false;
        }
        switch_edges(id, value);

        relations r = the_posets()[id][string(value)];

        for (auto &p : r) {
            relations *pom = &(the_posets()[id][p.first]);
            (*pom).erase(string(value));
        }
        poset *p = &(the_posets()[id]);
        p->erase(string(value));
        assert(!is_in_poset(value, id));
        if (debug) {
            std::cerr << "poset_remove: poset " << id << ", element \""
                      << string(value) << "\" removed\n";
        }
        return true;
    }

    bool poset_add(unsigned long id, char const *value1, char const *value2) {
        if (!check_inquiry_correctness("poset_add", id, value1, value2))
            return false;

        if (test_DFS(id, value1, value2) ||
                test_DFS(id, value2, value1)) {
            if (debug) {
                std::cerr << "poset_add: poset " << id << ", relation (\""
                          << string(value1) << "\", \"" << string(value2)
                          << "\") cannot be added\n";
            }
            return false;
        }

        poset_add_no_cerr(id, value1, value2);
        assert(test_DFS(id, value1, value2));

        if (debug) {
            std::cerr << "poset_add: poset " << id << ", relation (\""
                      << string(value1) << "\", \"" << string(value2)
                      << "\") added\n";
        }
        return true;
    }

    bool poset_del(unsigned long id, char const *value1, char const *value2) {
        if (!check_inquiry_correctness("poset_del", id, value1, value2))
            return false;

        string previous = value1;
        string next = value2;

        if (!poset_test_direct(id, value1, value2) || previous == next) {
            if (debug) {
                std::cerr << "poset_del: poset " << id << ", relation (\""
                          << string(value1) << "\", \"" << string(value2)
                          << "\") cannot be deleted\n";
            }
            return false;
        }
        poset *p = &(the_posets()[id]);

        //Usuwam krawędzie z previous do next i z next to previous.
        auto it_poset = (*p).find(previous);
        if (it_poset != (*p).end()) {
			auto it_relation = it_poset->second.find(next);
			if (it_relation != it_poset->second.end() && 
			        it_relation->second == 1) {
				it_poset->second.erase(it_relation->first);
			}
		}

		it_poset = (*p).find(next);
        if (it_poset != (*p).end()) {
			auto it_relation = it_poset->second.find(previous);
			if (it_relation != it_poset->second.end() && 
			        it_relation->second == 0) {
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

        assert(!test_DFS(id, value1, value2));
        if (debug) {
            std::cerr << "poset_del: poset " << id << ", relation (\""
                      << string(value1) << "\", \"" << string(value2)
                      << "\") deleted\n";
        }
        return true;
    }

    bool poset_test(unsigned long id, char const *value1, char const *value2) {
        if (!check_inquiry_correctness("poset_test", id, value1, value2))
            return false;

        bool result =  test_DFS(id, value1, value2);
        if (debug) {
            std::cerr << "poset_test: poset " << id << ", relation (\""
                      << string(value1) << "\", \"" << string(value2) << "\") "
                      << (result ? "exists\n" : "does not exist\n");
        }
        return result;
    }

    void poset_clear(unsigned long id) {
        if (debug)
            std::cerr << "poset_clear(" << id << ")\n";
        if (!poset_exists(id)) {
            if (debug)
                std::cerr << "poset_clear: poset " << id << " does not exist\n";
            return;
        }

        poset *p = &(the_posets()[id]);

        for (auto &r : (*p)) {
            r.second.clear();
        }
        (*p).clear();
        assert(poset_exists(id) && p->empty());
        if (debug)
            std::cerr << "poset_clear: poset " << id << " cleared\n";
    }
}


