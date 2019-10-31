#include "poset.h"

#include <list>
#include <queue>
#include <cassert>
#include <unordered_map>
#include <map>
#include <cstring>

using string = std::string;
using relations = std::map<long long, bool>;
using poset = std::unordered_map<long long, relations>;
using all_posets = std::unordered_map<unsigned long, poset>;

#ifdef NDEBUG
bool debug = {false};
#else
bool debug = {true};
#endif

namespace {
    std::unordered_map<long long, string> &the_names() {
        static std::unordered_map<long long, string> names;
        return names;
    }
    /* Mapa identyfikatorów liczbowych nadanych stringom w celu
     * nieprzechowywania dwóch kopii stringa. */

    long long& the_name_ids_beg() {
        static long long name_id = 0;
        return name_id;
    }
    //Pierwszy nigdy nie nadany identyfikator dla stringa.

    std::queue<long long> &the_free_name_ids() {
        static std::queue<long long> free_ids;
        return free_ids;
    }
    //Kolejka wolnych identyfikatorów liczbowych dla nazw elementów posetu.

    long long find_key(string v) {
        for (auto &[key, value] : the_names()) {
            if (v == value)
                return key;
        }
        return -1;
    }
    //Znajduje klucz w mapie the_names(), który posiada wartość v.

    long long choose_new_name_id(string name) {
        if (the_free_name_ids().empty()) {
            the_name_ids_beg()++;
            the_names().insert({the_name_ids_beg() - 1, name});
            return the_name_ids_beg() - 1;
        }
        long long id = the_free_name_ids().front();
        the_names().insert({id, name});
        the_free_name_ids().pop();
        return id;
    }
    //Wybiera identyfikator dla stringa name i wpisuje do the_names().

    std::queue<unsigned long> &the_free_ids() {
        static std::queue<unsigned long> free_ids;
        return free_ids;
    }
    //Kolejka zwolnionych identyfikatorów posetów.

    unsigned long& the_free_ids_beg() {
        static unsigned long free_ids_beg = 0;
        return free_ids_beg;
    }
    //Pierwszy nigdy nie wykorzystany identyfikator posetu.

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
        return poset_exists(id) &&
                the_posets()[id].count(find_key(string(element))) > 0;
    }
    /* Sprawdza, czy napis wskazywany przez element jest w posecie id.
    * Jeżeli poset o danym id nie istnieje, zrwaca false. */

    bool test_DFS(unsigned long id, char const *value1, char const *value2) {
        string previous = value1;
        string next = value2;
        if (previous.compare(next) == 0)
            return true;

        relations r = the_posets()[id][find_key(previous)];
        for (auto &[name, direction] : r) {
            if (next.compare(the_names()[name]) == 0 && direction == 1)
                return true;
        }

        for (auto &[name, direction] : r) {
            if (direction == 1) {
                if (test_DFS(id, the_names()[name].c_str(), value2))
                    return true;
            }
        }
        return false;
    }
    /* Funkcja potrzebna do wykonania poset_test. Przechodzi poset
    *  rekurencyjnie (tak jak DFS) w celu sprawdzenia czy istnieje
    *  połączenie z value1 do value2.*/

    bool poset_test_transitive(unsigned long id, char const *value1,
                               char const *value2) {
        string previous = value1;
        string next = value2;
        relations r = the_posets()[id][find_key(previous)];

        for (auto &[name, direction] : r) {
            if (direction == 1 && the_names()[name] != next) {
                if (test_DFS(id, the_names()[name].c_str(), value2)) {
                    return false;
                }
            }
        }

        return true;
    }
    /* Sprawdza czy usuniecie relacji value1 -> value2 przerwie przechodniosc
    *  zwraca true, jesli relacje można usunac*/

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
            if (previous.compare(the_names()[node]) == 0) {
                relation.insert({find_key(next), 1});
            }
            if (next.compare(the_names()[node]) == 0) {
                relation.insert({find_key(previous), 0});
            }
        }
    }
    /* Dodaje relację między istniejącymi elementami posetu. Nie wypisuje na
     * cerr informacji diagnostycznych.*/

    void switch_edges(unsigned long id, char const *value) {
        if (!poset_exists(id) || !is_in_poset(value, id))
            return;

        relations r = the_posets()[id][find_key(string(value))];

        for (auto &[name, direction] : r)
            if (direction == 0)
                for (auto &[name2, direction2] : r)
                    if (direction2 == 1)
                        poset_add_no_cerr(id, the_names()[name].c_str(),
                                the_names()[name2].c_str());

    }
    /* Dla wszystkich poprzedników value daje krawędź do wszystkich
    *  jego następników.*/

    void remove_edge(unsigned long id, const char* value1, const char* value2) {
        poset *p = &(the_posets()[id]);
        string previous = value1;
        string next = value2;

        //Usuwa krawędź z previous do next.
        auto it_poset = (*p).find(find_key(previous));
        if (it_poset != (*p).end()) {
            auto it_relation = it_poset->second.find(find_key(next));
            if (it_relation != it_poset->second.end() &&
                    it_relation->second == 1) {
                it_poset->second.erase(it_relation->first);
            }
        }
        //Usuwa krawędź z next do previous.
        it_poset = (*p).find(find_key(next));
        if (it_poset != (*p).end()) {
            auto it_relation = it_poset->second.find(find_key(previous));
            if (it_relation != it_poset->second.end() &&
                    it_relation->second == 0) {
                it_poset->second.erase(it_relation->first);
            }
        }
    }
    /* Usuwa krawędź z value1 do value2, nie sprawdza czy taka krawędź istnieje.*/

    void keep_transitive(unsigned long id, char const *value1,
            char const *value2) {

        string previous = value1;
        string next = value2;

        poset *p = &(the_posets()[id]);
        //Dla każdego wierzchołka do którego prowadzi krawędź z value1, dodaje
        //krawędź prowadzącą od value2, żeby nie przerwać przechodniości
        auto it_poset = (*p).find(find_key(next));
        if (it_poset != (*p).end()) {
            for (auto &[name, direction] : it_poset->second) {
                if (direction == 1) {
                    poset_add_no_cerr(id, previous.c_str(),
                            the_names()[name].c_str());
                }
            }
        }

        //Dla każdego wierzchołka który prowadzi do value1, daje krawędź
        //prowadzącą do value2
        it_poset = (*p).find(find_key(previous));
        if (it_poset != (*p).end()) {
            for (auto &[name, direction] : it_poset->second) {
                if (direction == 0) {
                    poset_add_no_cerr(id, the_names()[name].c_str(),
                            next.c_str());
                }
            }
        }
    }
    /* Część poset_del, po usunięciu krawędzi uzupełnia poset tak, aby
    *  pozostał przechodni.*/
}

/* Przestrzeń nazw zawierająca funkcje wypisujące na standardowe wyjście
 * diagostyczne. */
namespace log {
    void function_start(string inquiry, unsigned long id) {
        if (debug)
            std::cerr << inquiry << "(" << id << ")\n";
    }

    void no_poset(string inquiry, unsigned long id) {
        if (debug) {
            std::cerr << inquiry << ": poset " << id
                      << " does not exist\n";
        }
    }

    void success(unsigned long id, string inquiry, string effect) {
        if (debug)
            std::cerr << inquiry << ": poset " << id << " " << effect << "\n";
    }

    void poset_new_start() {
        if (debug)
            std::cerr << "poset_new()\n";
    }

    void poset_size_success(unsigned long id, size_t result) {
        if (debug) {
            std::cerr << "poset_size: poset " << id << " contains "
                      << result << " element(s)\n";
        }
    }

    void poset_insert_remove_start (string inquiry,unsigned long id,
                                    char const*value) {
        if (debug) {
            std::cerr << inquiry << "(" << id << ", \""
                      << ((value == nullptr) ?
                          "NULL" : string(value)) << "\")\n";
        }
    }

    void poset_insert_remove_NULL(string inquiry) {
        if (debug)
            std::cerr << inquiry << ": invalid value (NULL)\n";
    }

    void poset_insert_again(unsigned long id, char const *value) {
        if (debug) {
            std::cerr << "poset_insert: poset " << id << ", element \""
                      << string(value) << "\" already exists\n";
        }
    }

    void poset_insert_success(unsigned long id, char const *value) {
        if (debug) {
            std::cerr << "poset_insert: poset " << id << ", element \""
                      << string(value) << "\" inserted\n";
        }
    }

    void poset_remove_no_element(unsigned long id, char const *value) {
        if (debug) {
            std::cerr << "poset_remove: poset " << id << ", element \""
                      << string(value) << "\" does not exist\n";
        }
    }

    void poset_remove_success(unsigned long id, char const *value) {
        if (debug) {
            std::cerr << "poset_remove: poset " << id << ", element \""
                      << string(value) << "\" removed\n";
        }
    }

    void poset_add_remove_success(unsigned long id, char const *value1,
                                  char const *value2, string inquiry,
                                  string effect) {
        if (debug) {
            std::cerr << inquiry << ": poset " << id << ", relation (\""
                      << string(value1) << "\", \"" << string(value2)
                      << "\") " << effect << "\n";
        }
    }

    void poset_add_remove_failure(unsigned long id, char const *value1,
                                  char const *value2, string inquiry,
                                  string effect) {
        if (debug) {
            std::cerr << inquiry << ": poset " << id << ", relation (\""
                      << string(value1) << "\", \"" << string(value2)
                      << "\") cannot be " << effect << "\n";
        }
    }

    void poset_test_result(unsigned long id, char const *value1,
                           char const *value2, bool result) {
        if (debug) {
            std::cerr << "poset_test: poset " << id << ", relation (\""
                      << string(value1) << "\", \"" << string(value2) << "\") "
                      << (result ? "exists\n" : "does not exist\n");
        }
    }
}

extern "C" {
    unsigned long poset_new() {
        log::poset_new_start();
        poset p;
        unsigned long id = choose_new_id();
        the_posets().insert(std::make_pair(id, p));
        assert(poset_exists(id));
        log::success(id, "poset_new", "created");
        return id;
    }

    void poset_delete(unsigned long id) {
        log::function_start("poset_delete", id);
        if (poset_exists(id)) {
            the_posets().erase(id);
            the_free_ids().push(id);
            log::success(id, "poset_delete", "deleted");
        } else {
            log::no_poset("poset_delete", id);
        }
    }

    size_t poset_size(unsigned long id) {
        log::function_start("poset_size", id);
        if (poset_exists(id)) {
            poset p = the_posets()[id];
            size_t result = p.size();
            log::poset_size_success(id, result);
            return result;
        }
        assert(!poset_exists(id));
        log::no_poset("poset_size", id);
        return 0;
    }

    bool poset_insert(unsigned long id, char const *value) {
        log::poset_insert_remove_start("poset_insert", id, value);
         if (!poset_exists(id)) {
            log::no_poset("poset_insert", id);
            return false;
        } else if (value == nullptr) {
            log::poset_insert_remove_NULL("poset_insert");
            return false;
        } else if (is_in_poset(value, id)) {
            log::poset_insert_again(id, value);
            return false;
        }
        poset *p = &(the_posets()[id]);
        string insert_value = string(value);
        long long insert_id = choose_new_name_id(string(value));
        p->insert(std::make_pair(insert_id, std::map<long long, bool>()));
        assert(is_in_poset(value, id));
        log::poset_insert_success(id, value);
        return true;
    }

    bool poset_remove(unsigned long id, char const *value) {
        log::poset_insert_remove_start("poset_remove", id, value);
        if (!poset_exists(id)) {
            log::no_poset("poset_remove", id);
            return false;
        } else if (value == nullptr) {
            log::poset_insert_remove_NULL("poset_remove");
            return false;
        } else if (!is_in_poset(value, id)) {
            log::poset_remove_no_element(id, value);
            return false;
        }

        long long value_key = find_key(string(value));

        relations r = the_posets()[id][value_key];

        for (auto &p : r) {
            relations *pom = &(the_posets()[id][p.first]);
            (*pom).erase(value_key);
        }
        switch_edges(id, value);
        poset *p = &(the_posets()[id]);
        p->erase(value_key);

        the_names().erase(value_key);
        the_free_name_ids().push(value_key);

        assert(!is_in_poset(value, id));
        log::poset_remove_success(id, value);
        return true;
    }

    bool poset_add(unsigned long id, char const *value1, char const *value2) {
        if (!check_inquiry_correctness("poset_add", id, value1, value2))
            return false;

        if (test_DFS(id, value1, value2) ||
            test_DFS(id, value2, value1)) {
            log::poset_add_remove_failure(id, value1, value2,
                                          "poset_add", "added");
            return false;
        }

        poset_add_no_cerr(id, value1, value2);
        assert(test_DFS(id, value1, value2));
        log::poset_add_remove_success(id, value1, value2, "poset_add", "added");
        return true;
    }

    bool poset_del(unsigned long id, char const *value1, char const *value2) {
        if (!check_inquiry_correctness("poset_del", id, value1, value2))
            return false;

        string previous = value1;
        string next = value2;

        if (!poset_test_transitive(id, value1, value2) ||
            !test_DFS(id, value1, value2) || previous == next) {
            log::poset_add_remove_failure(id, value1, value2,
                                          "poset_del", "deleted");
            return false;
        }

        //Usuwam krawędzie z previous do next i z next to previous.
        remove_edge(id, value1, value2);
        keep_transitive(id, value1, value2);

        assert(!test_DFS(id, value1, value2));
        log::poset_add_remove_success(id, value1, value2,
                                      "poset_del", "deleted");
        return true;
    }

    bool poset_test(unsigned long id, char const *value1, char const *value2) {
        if (!check_inquiry_correctness("poset_test", id, value1, value2))
            return false;

        bool result =  test_DFS(id, value1, value2);
        log::poset_test_result(id, value1, value2, result);
        return result;
    }

    void poset_clear(unsigned long id) {
        log::function_start("poset clear", id);
        if (!poset_exists(id)) {
            log::no_poset("poset_clear", id);
            return;
        }

        poset *p = &(the_posets()[id]);

        for (auto &r : (*p)) {
            r.second.clear();
        }
        (*p).clear();
        assert(poset_exists(id) && p->empty());
        log::success(id, "poset_clear", "cleared");
    }
}