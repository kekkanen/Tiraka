// Definition of Datastructure class, hw2 of TIE-20100/TIE-20106

#ifndef DATASTRUCTURE_HPP
#define DATASTRUCTURE_HPP

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <list>
using std::list;

#include <utility>
using std::pair;

#include <map>
using std::map;
using std::multimap;

#include <algorithm>
using std::find_if;

#include <iterator>

using PersonID = string;
PersonID const NO_ID = "";

using Salary = int;
Salary const NO_SALARY = -1;

struct Person
{
    string name;
    PersonID id;
    string title;
    Salary salary;
    PersonID bossid;
};


class Datastructure
{
public:
    Datastructure();
    ~Datastructure();

    void add_person(string name, PersonID id, string title, Salary salary);
    void remove_person(PersonID id);

    string get_name(PersonID id);
    string get_title(PersonID id);
    Salary get_salary(PersonID id);
    vector<PersonID> find_persons(string name);
    vector<PersonID> personnel_with_title(string title);

    void change_name(PersonID id, string new_name);
    void change_salary(PersonID id, Salary new_salary);

    void add_boss(PersonID id, PersonID bossid);

    unsigned int size();
    void clear();

    vector<PersonID> underlings(PersonID id);
    vector<PersonID> personnel_alphabetically();
    vector<PersonID> personnel_salary_order();

    PersonID find_ceo();
    PersonID nearest_common_boss(PersonID id1, PersonID id2);
    pair<unsigned int, unsigned int> higher_lower_ranks(PersonID id);

    PersonID min_salary();
    PersonID max_salary();

    PersonID median_salary();
    PersonID first_quartile_salary();
    PersonID third_quartile_salary();

private:
    // Add your implementation here

    map<PersonID, Person*> person_by_id;
    map<PersonID, vector<PersonID>> bosses;

    vector<PersonID> alphabetic;
    vector<PersonID> salary;

    multimap<string, Person*> persons_al; // alphabet
    multimap<int, Person*> persons_sa;     // salary

    void alphasort();
    void salarysort();

    bool sorted_alpha{false};
    bool sorted_salary{false};

    PersonID ceoID;
    bool ceo_valid{false};

};

#endif // DATASTRUCTURE_HPP

















