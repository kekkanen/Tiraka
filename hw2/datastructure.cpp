// Definition of Datastructure class, hw2 of TIE-20100/TIE-20106

#include "datastructure.hpp"

// For debug
#include <iostream>
using std::cerr;
using std::endl;

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = rand() % range;
    return static_cast<Type>(start+num);
}


Datastructure::Datastructure()
{

}

Datastructure::~Datastructure()
{
    clear();
}

void Datastructure::add_person(string name, PersonID id, string title, Salary salary)
{  


    Person* p = new Person;
    p->name = name;
    p->id = id;
    p->title = title;
    p->salary = salary;
    p->bossid = NO_ID;

    // multimap allows multiple values, so check if the unique id exists.
    // if not, insertion is allowed
    if (!person_by_id.count(id)) {

        persons_al.insert(std::make_pair(name, p));
        persons_sa.insert(std::make_pair(salary, p));
    }

    auto pair = std::make_pair(id, p);
    person_by_id.insert(pair);


    // persons_a.push_back(p);
    // persons_s.push_back(p);

    // invalidate booleans
    ceo_valid = false;
    sorted_alpha = false;
    sorted_salary = false;

}

// removes a person. does not invalidate order
void Datastructure::remove_person(PersonID id)
{
    if (person_by_id.count(id)) {

        // the person to be removed
        Person* removed = person_by_id[id];

        // if the person is ceo
        if (id == ceoID) {

            // if ceo has only 1 underling, it becomes the new ceo
            if (bosses[id].size() == 1) {
                ceoID = bosses[id].front();
                ceo_valid = true;
            } else {
                ceo_valid = false;
                ceoID = NO_ID;
            }
        }

        // if the person has a boss, the person has to be removed from their underlings
        if (removed->bossid != NO_ID) {

            vector<PersonID> underlings_new; // vector without the removed id

            // iterate the whole underlings vector, add every not matching id to underlings_new.
            // also set removed's boss as underlings' boss
            std::for_each(bosses[removed->bossid].begin(), bosses[removed->bossid].end(), [&](PersonID i){
                if (i != id) underlings_new.push_back(i);
            });
            bosses[removed->bossid] = underlings_new;
        }

        // see if the person has underlings
        auto it = bosses.find(id);
        if (it != bosses.end()) {

            // iterate through all their underlings and set removed persons boss as their boss
            vector<PersonID> &v = it->second;
            std::for_each(v.begin(), v.end(), [&](PersonID p){
                person_by_id[p]->bossid = removed->bossid;
                bosses[removed->bossid].push_back(p);
            });
            // finally remove person from bosses
            bosses.erase(id);
        }

        persons_al.erase(std::find_if(persons_al.begin(), persons_al.end(), [&](pair<string, Person*> p){
            return p.second->id == removed->id;
        }));
        persons_sa.erase(std::find_if(persons_sa.begin(), persons_sa.end(), [&](pair<int, Person*> p){
            return p.second->id == removed->id;
        }));

        person_by_id.erase(id);

        // the lists are no longer valid
        sorted_alpha = false;
        sorted_salary =  false;
        delete removed;
    }

}

string Datastructure::get_name(PersonID id)
{
    // if (person_by_id.count(id) == 0) return NO_ID;
    // else
    return person_by_id[id]->name;
}

string Datastructure::get_title(PersonID id)
{
    // if (person_by_id.count(id) == 0) return NO_ID;
    // else
    return person_by_id[id]->title;
}

Salary Datastructure::get_salary(PersonID id)
{
    // if (person_by_id.count(id) == 0) return NO_SALARY;
    // else
    return person_by_id[id]->salary;
}

vector<PersonID> Datastructure::find_persons(string name)
{
    vector<PersonID> v;
    std::for_each(persons_al.begin(), persons_al.end(), [&](pair<string, Person*> p){
        if (p.first == name) {
            v.push_back(p.second->id);
        }
    });

    return v;
}

vector<PersonID> Datastructure::personnel_with_title(string title)
{
    vector<PersonID> v;
    std::for_each(person_by_id.begin(), person_by_id.end(), [&](pair<PersonID, Person*> p) {
        if (p.second->title == title) {
            v.push_back(p.first);
        }
    });

    return v;

}

void Datastructure::change_name(PersonID id, string new_name)
{
    auto it = person_by_id.find(id);

    if (it != person_by_id.end()) {
        it->second->name = new_name;

        Person* pe = person_by_id[id];   // save the Person* with id id.
        // remove the person from persons_al
        persons_al.erase(std::find_if(persons_al.begin(), persons_al.end(), [&](pair<string, Person*> p){
            return p.second->id == pe->id;
        }));
        persons_al.insert(std::make_pair(new_name, pe)); // insert person with new key

    } else {
        cerr << "id not found." << endl;
        return;
    }
    sorted_alpha = false;
}

void Datastructure::change_salary(PersonID id, Salary new_salary)
{
    auto it = person_by_id.find(id);

    if (it != person_by_id.end()) {
        it->second->salary = new_salary;

        Person* pe = person_by_id[id];   // save the Person* with id id.
        // remove the person from persons_sa
        persons_sa.erase(std::find_if(persons_sa.begin(), persons_sa.end(), [&](pair<int, Person*> p){
            return p.second->id == pe->id;
        }));
        persons_sa.insert(std::make_pair(new_salary, pe)); // insert person with new key
    } else {
        cerr << "id not found." << endl;
        return;
    }
    sorted_salary = false;
}

void Datastructure::add_boss(PersonID id, PersonID bossid)
{
    // if id exists in the container
    if (person_by_id.count(id)) {

        // if ceo is given a boss, the boss becomes ceo
        if (id == ceoID) {
            ceoID = bossid;
        } else if (person_by_id[bossid]->bossid == NO_ID) {
            // the added boss has no boss, invalidate ceoID
            ceo_valid = false;
        }

        person_by_id[id]->bossid = bossid;

        bosses[bossid].push_back(id);
    } else cerr << "id not found" << endl;
}

unsigned int Datastructure::size()
{
    return person_by_id.size();
}

void Datastructure::clear()
{
    /* std::for_each(persons_a.begin(), persons_a.end(), [](Person* p){
       delete p;
    }); */
    std::for_each(persons_al.begin(), persons_al.end(), [](pair<string, Person*> p){
           delete p.second;
    });

    bosses.erase(bosses.begin(), bosses.end());
    person_by_id.erase(person_by_id.begin(), person_by_id.end());
    // persons_a.erase(persons_a.begin(), persons_a.end());
    // persons_s.erase(persons_s.begin(), persons_s.end());

    persons_al.erase(persons_al.begin(), persons_al.end());
    persons_sa.erase(persons_sa.begin(), persons_sa.end());
}

vector<PersonID> Datastructure::underlings(PersonID id)
{
    vector<PersonID> v;
    if (bosses.count(id)) {
        v = bosses[id];
        std::sort(v.begin(), v.end(), [&](PersonID lhs, PersonID rhs){
            return lhs < rhs;
        });
    }
    return v;
}

vector<PersonID> Datastructure::personnel_alphabetically()
{
    if (!sorted_alpha) {
        alphasort();
    }

    return alphabetic;
}

vector<PersonID> Datastructure::personnel_salary_order()
{
    if (!sorted_alpha) {
        salarysort();
    }

    return salary;
}

void Datastructure::alphasort()
{
    vector<PersonID> v;

    // persons_al always in order because of multimap's behaviour.
    std::for_each(persons_al.begin(), persons_al.end(), [&](pair<string, Person*> p) {
        v.push_back(p.second->id);
    });
    alphabetic = v;
    sorted_alpha = true;
}

void Datastructure::salarysort()
{
    vector<PersonID> v;

    // persons_sa always in order because of multimap's behaviour.
    std::for_each(persons_sa.begin(), persons_sa.end(), [&](pair<int, Person*> p) {
        v.push_back(p.second->id);
    });
    salary = v;
    sorted_salary = true;
}

PersonID Datastructure::find_ceo()
{
    if (!ceo_valid) {
        // counter for how many people count as ceo. if > 1, function returns NO_ID.
        // a ceo has underlings but 0 bosses.
        int counter = 0;

        std::for_each(person_by_id.begin(), person_by_id.end(), [&] (pair<PersonID, Person*> p) {


            if (p.second->bossid == NO_ID) {
                ceoID = p.second->id;
                ++counter;
            }

        });
        // if more than 1 possible ceo was found, invalidate ceoID and set it as NO_ID
        if (counter > 1) {
            ceoID = NO_ID;
            ceo_valid = false;
        }
    }
    return ceoID;
}

PersonID Datastructure::nearest_common_boss(PersonID id1, PersonID id2)
{
    return NO_ID;
}

pair<unsigned int, unsigned int> Datastructure::higher_lower_ranks(PersonID id)
{
    return {0, 0}; // If you do not implement this, leave this return value as it is.
}

PersonID Datastructure::min_salary()
{
    if (!sorted_salary) {
        salarysort();
    }
    return salary.front();

}

PersonID Datastructure::max_salary()
{
    if (!sorted_salary) {
        salarysort();
    }
    return salary.back();
}

PersonID Datastructure::median_salary()
{
    if (!sorted_salary) salarysort();
    int pos = salary.size()/2;
    return salary[pos];

}

PersonID Datastructure::first_quartile_salary()
{
    if (!sorted_salary) salarysort();
    int pos = salary.size()/4;
    return salary[pos];
}

PersonID Datastructure::third_quartile_salary()
{
    if (!sorted_salary) salarysort();
    int pos = salary.size()*3/4;
    return salary[pos];
}
