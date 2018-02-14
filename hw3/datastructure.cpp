// Definition of Datastructure class, hw3 of TIE-20100/TIE-20106

#include "datastructure.hpp"

// For debug
#include <iostream>
using std::cerr;
using std::endl;

#include <random>
using std::minstd_rand;
using std::uniform_int_distribution;

#include <algorithm>
#include <iterator>
#include <queue>
using std::queue;
using std::priority_queue;

#include <stack>
using std::stack;

#include <unordered_set>
using std::unordered_set;



minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

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
    p->friends = {};
    p->underlings = {};

    // add person into necessary containers
    persons_un.insert(std::make_pair(id, p));
    persons_al.insert(std::make_pair(name, p));
    persons_sa.insert(std::make_pair(salary, p));
    bossless.push_back(id);


    sorted_alpha = false;
    sorted_salary = false;

}

void Datastructure::remove_person(PersonID id)
{
    auto it = persons_un.find(id);
    if (it != persons_un.end()) {

        // the person to be removed
        Person* removed = it->second;
        // if the person has a boss
        if (removed->bossid != NO_ID) {

            Person* boss = persons_un.at(removed->bossid);
            // add removed's underlings to boss' underlings
            for (Person* p : removed->underlings) {
                p->bossid = removed->bossid;
                boss->underlings.push_back(p);
            }
            // remove the person from their boss' underlings.
            boss->underlings.erase(std::find_if(boss->underlings.begin(), boss->underlings.end(), [&] (Person* p) {
                return p->id == removed->id;
            }));
        } else {
            // change the person's underlings' bossid to removed's bossid
            for (Person* p : removed->underlings) {
                p->bossid = removed->bossid;
            }
        }

        // remove friendships
        for (auto a : removed->friends) {

            Person* person = a.first;
            person->friends.erase(std::find_if(person->friends.begin(), person->friends.end(), [&] (pair<Person*, Cost> pair) {
                                      return pair.first->id == id;
                                  }));
        }

        auto first = std::find_if(friendships.begin(), friendships.end(), [&] (pair<PersonID, PersonID> asd) {
            return asd.first == id;
        });
        if (first != friendships.end()) {
            auto last = first;
            while (last->first == id) {
                ++last;
            }
            friendships.erase(first, last);
        }


        persons_al.erase(std::find_if(persons_al.begin(), persons_al.end(), [&](pair<string, Person*> p){
            return p.second->id == removed->id;
        }));
        persons_sa.erase(std::find_if(persons_sa.begin(), persons_sa.end(), [&](pair<int, Person*> p){
            return p.second->id == removed->id;
        }));

        persons_un.erase(id);

        // the lists are no longer valid
        sorted_alpha = false;
        sorted_salary =  false;
        delete removed;
    }

}

// assumes id exists
string Datastructure::get_name(PersonID id)
{
    return persons_un[id]->name;
}

// assumes id exists
string Datastructure::get_title(PersonID id)
{
    return persons_un[id]->title;
}

// assumes id exists
Salary Datastructure::get_salary(PersonID id)
{
    return persons_un[id]->salary;
}

vector<PersonID> Datastructure::find_persons(string name)
{
    vector<PersonID> v;
    multimap<string, Person*>::iterator it = std::find_if(persons_al.begin(), persons_al.end(), [&] (pair<string, Person*> p) {
        return p.first == name;
    });
    if (it != persons_al.end()) {
        while (it->first == name) {
            v.push_back(it->second->id);
            ++it;
        }
    }
    std::sort(v.begin(), v.end(), [&] (PersonID lhs, PersonID rhs) {
        return lhs < rhs;
    });
    return v;
}

vector<PersonID> Datastructure::personnel_with_title(string title)
{
    vector<PersonID> v;
    std::for_each(persons_un.begin(), persons_un.end(), [&](pair<PersonID, Person*> p) {
        if (p.second->title == title) {
            v.push_back(p.first);
        }
    });
    std::sort(v.begin(), v.end());

    return v;
}

void Datastructure::change_name(PersonID id, string new_name)
{
    auto it = persons_un.find(id);

    if (it != persons_un.end()) {

        Person* pe = it->second;   // save the Person* with id id.
        it->second->name = new_name; // change their name

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
    auto it = persons_un.find(id);

    if (it != persons_un.end()) {

        Person* pe = it->second;   // save the Person* with id id.
        it->second->salary = new_salary; // change their name

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
    auto it = persons_un.find(id);
    // if id exists in the container
    if (it != persons_un.end()) {

        Person* pe = it->second;
        pe->bossid = bossid;


        //
        auto it2 = persons_un.find(bossid);
        it2->second->underlings.push_back(pe);
        //

        // bosses[bossid].push_back(id);
        bossless.erase(std::find_if(bossless.begin(), bossless.end(), [&] (PersonID p) {
            return p == id;
        }));
    } else cerr << "id not found" << endl;
}

unsigned int Datastructure::size()
{
    return persons_un.size();
}

void Datastructure::clear()
{

    std::for_each(persons_al.begin(), persons_al.end(), [](pair<string, Person*> p){
           delete p.second;
    });

    // bosses.erase(bosses.begin(), bosses.end());
    bossless.erase(bossless.begin(), bossless.end());

    persons_un.erase(persons_un.begin(), persons_un.end());
    friendships.erase(friendships.begin(), friendships.end());

    persons_al.erase(persons_al.begin(), persons_al.end());
    persons_sa.erase(persons_sa.begin(), persons_sa.end());
}

vector<PersonID> Datastructure::underlings(PersonID id)
{
    vector <PersonID> v;
    auto p = persons_un.find(id);   // get iterator to person
    if (p != persons_un.end()) {
        Person* pe = p->second;
        std::for_each(pe->underlings.begin(), pe->underlings.end(), [&] (Person* person) {
            v.push_back(person->id);
        });
        std::sort(v.begin(), v.end());
    }
    return v;     // else return empty vector
}

vector<PersonID> Datastructure::personnel_alphabetically()
{
    if (!sorted_alpha) {
        update_alphabetic();
    }

    return alphabetic;

}

vector<PersonID> Datastructure::personnel_salary_order()
{
    if (!sorted_alpha) {
        update_salary();
    }

    return salary;
}

void Datastructure::update_alphabetic()
{
    vector<PersonID> v;

    // persons_al always in order because of multimap's behaviour.
    std::for_each(persons_al.begin(), persons_al.end(), [&](pair<string, Person*> p) {
        v.push_back(p.second->id);
    });
    alphabetic = v;
    sorted_alpha = true;
}

void Datastructure::update_salary()
{
    vector<PersonID> v;

    // persons_al always in order because of multimap's behaviour.
    std::for_each(persons_sa.begin(), persons_sa.end(), [&](pair<Salary, Person*> p) {
        v.push_back(p.second->id);
    });
    salary = v;
    sorted_salary = true;
}

PersonID Datastructure::find_ceo()
{
    // Check the bossless vector.
    // If there is only one bossless person, he is the CEO.
    // Otherwise return NO_ID

    if (bossless.size() != 1) return NO_ID;
    else return bossless.front();
}

PersonID Datastructure::nearest_common_boss(PersonID id1, PersonID id2)
{
    return NO_ID; // Replace this with the actual implementation, if any
}

pair<unsigned int, unsigned int> Datastructure::higher_lower_ranks(PersonID id)
{
    return {0,0}; // Replace this with the actual implementation, if any
}

PersonID Datastructure::min_salary()
{
    if (!sorted_salary) {
        update_salary();
    }
    return salary.front();
}

PersonID Datastructure::max_salary()
{
    if (!sorted_salary) {
        update_salary();
    }
    return salary.back();
}

PersonID Datastructure::median_salary()
{
    if (!sorted_salary) update_salary();
    int pos = salary.size()/2;
    return salary[pos];
}

PersonID Datastructure::first_quartile_salary()
{
    if (!sorted_salary) update_salary();
    int pos = salary.size()/4;
    return salary[pos];
}

PersonID Datastructure::third_quartile_salary()
{
    if (!sorted_salary) update_salary();
    int pos = salary.size()*3/4;
    return salary[pos];
}

// After this are the new operations of hw3
// NOTE!!! Add all your new hw3 code here below these comments (to keep it
// separated from old hw2 code).
void Datastructure::add_friendship(PersonID id, PersonID friendid, Cost cost)
{
    auto it = persons_un.find(id);
    auto it2 = persons_un.find(friendid);

    // if the IDs exist
    if (it != persons_un.end() && it2 != persons_un.end()) {

        Person* p1 = it->second;
        Person* p2 = it2->second;

        // add the IDs to each other's friendlist and the friendships multimap
        p1->friends.push_back(std::make_pair(p2, cost));
        p2->friends.push_back(std::make_pair(p1, cost));

        // make a pair of the id's to be added into friendships.
        // the "smaller" id comes first in the pair
        pair<PersonID, PersonID> p;
        if (id < friendid) {
            p = std::make_pair(id, friendid);
        } else p = std::make_pair(friendid, id);

        friendships.insert(p);
    }
}

void Datastructure::remove_friendship(PersonID id, PersonID friendid)
{
    // check if ids exist
    auto it = persons_un.find(id);
    auto it2 = persons_un.find(friendid);

    if (it == persons_un.end() || it2 == persons_un.end()) {
        return;
    }
    // remove ids from each other's friendlist
    Person* p1 = it->second;
    Person* p2 = it2->second;

    p1->friends.erase(std::find_if(p1->friends.begin(), p1->friends.end(), [&] (pair<Person*, Cost> p) {
                          return p.first->id == friendid;
                      }));
    p2->friends.erase(std::find_if(p2->friends.begin(), p2->friends.end(), [&] (pair<Person*, Cost> p) {
                          return p.first->id == id;
                      }));

    pair<PersonID, PersonID> p;             // pair for searching, not really necessary
    if (id < friendid) {                    // the smaller id comes first
        p = std::make_pair(id, friendid);
    } else p = std::make_pair(friendid, id);

    // get an iterator to the located pair
    auto iter = std::find_if(friendships.begin(), friendships.end(), [&] (pair<PersonID, PersonID> pa) {

            // true if the pair p is found
            return (p.first == pa.first && p.second == pa.second);
    });

    friendships.erase(iter);  // what a cruel world

}

vector<pair<PersonID, Cost>> Datastructure::get_friends(PersonID id)
{
    vector<pair<PersonID, Cost>> v;
    vector<pair<Person*, Cost>> m = persons_un[id]->friends;                       // get the map of person's friends
    std::for_each(m.begin(), m.end(), [&] (pair<Person*, Cost> p) {
        v.push_back(std::make_pair(p.first->id, p.second));                     // add every friend into v
    });
    std::sort(v.begin(), v.end());
    return v;                                                                   // sort v by id
}

vector<pair<PersonID, PersonID>> Datastructure::all_friendships()
{
    vector<pair<PersonID, PersonID>> v;
    std::for_each(friendships.begin(), friendships.end(), [&] (pair<PersonID, PersonID> p) {
        v.push_back(p);
    });
    std::sort(v.begin(), v.end());      // sort by id
    return v;
}

vector<pair<PersonID, Cost> > Datastructure::shortest_friendpath(PersonID fromid, PersonID toid)
{
    // let's use BFS with a little modification

    // first check if the IDs are the same
    // and return empty vector if they are
    if (fromid == toid) return {};

    Person* to = persons_un.at(toid);
    Person* from = persons_un.at(fromid);
    if (to->friends.empty()) return {};               // no use to search if the target has no friends ;__;
    if (from->friends.empty()) return {};             // same with the start node, just a minor optimization

    queue<Person*> q;
    unordered_map<PersonID, pair<PersonID, Cost>> m;                    // map for the route
    bool found = false;                                                 // was the target found?

    m.insert(std::make_pair(fromid, std::make_pair(NO_ID, NO_COST)));   // insert the root denoted by the (-1,-1) pair to the route map

    q.push(from);                                                       // push the root node into the queue

    while (!q.empty()) {                                                // do things while the queue is not empty

        if (found) {                                                    // break if target's found
            break;
        }
        Person* current = q.front();                                    // take the first guy in queue as current
        q.pop();                                                        // and remove them from queue

        vector<pair<Person*, Cost>> friends = current->friends;
        for (pair<Person*, Cost> p : friends) {                         // iterate through current's friends

            Person* person = p.first;                                   // the friend
            Cost cost = p.second;                                       // cost between friends

            // Check if the target is in current's friends and break
            // if they are. This saves a lot of time because we can
            // bypass the queue.
            if (person->id == toid) {       // found 'im!

                pair<PersonID, Cost> route = std::make_pair(current->id, cost); // the route to the friend node
                m.insert(std::make_pair(person->id, route));                    // insert the friend node into the map with the route
                found = true;                                                   // set found to true to stop looping
                break;                                                          // i want to break free
            }

            if (m.find(person->id) != m.end()) {                                // if friend is processed, skip
                continue;
            } else {

                pair<PersonID, Cost> route = std::make_pair(current->id, cost); // the route to the friend node

                m.insert(std::make_pair(person->id, route));                    // insert the friend node into the map with the route
                q.push(person);                                                 // and enqueue them
            }
        }
    }

    vector<pair<PersonID, Cost>> v;                                     // the final route
    if (found) {

        auto it = m.find(toid);

        // while we're not at fromid
        while (it->second.first != NO_ID) {

            PersonID friendid = it->first;                              // friend's id
            Cost cost = it->second.second;                              // cost of getting there
            PersonID nextperson = it->second.first;                     // the next person to find

            pair<PersonID, Cost> p = std::make_pair(friendid, cost);
            v.push_back(p);                                             // push the pair into v
            m.erase(it);

            it = m.find(nextperson);
        }

        // we've gone the whole route, but the vector is in reverse order
        std::reverse(v.begin(), v.end());
    }

    return v;
}




bool Datastructure::check_boss_hierarchy()
{
    // Use DFS to check all persons and their subordinates.
    // If a processed person is found again, there is a loop and
    // thus the hierarchy is broken.
    PersonID ceo = find_ceo();

    if (ceo == NO_ID) {
        return false;
    }

    Person* curr = persons_un.at(ceo);
    stack<Person*> s;
    unordered_set<PersonID> set;
    s.push(curr);
    set.insert(curr->id);
    while (!s.empty()) {
        curr = s.top();
        s.pop();

        for (auto p : curr->underlings) {
            if (set.find(p->id) != set.end()) {
                return false;
            } else set.insert(p->id);
            s.push(p);
        }
    }
    // We have checked the main hierarchy, but there might be some stray
    // people as each others' bosses circlejerking somewhere..
    // If visited's size is equal to persons_un size, then we truly have checked everyone.
    // Else the hierarchy is broken.
    if (set.size() != persons_un.size()) return false;

    return true;
}



vector<pair<PersonID, Cost>> Datastructure::cheapest_friendpath(PersonID fromid, PersonID toid)
{
    // This function's underlying algorithm is Dijkstra.

    // make a priority queue for handling all nodes.
    // SmallerCost is a function used to compare
    // nodes, so that the node with smallest distance
    // to source node is handled first.
    priority_queue<Node*, vector<Node*>, Node::SmallerCost> pq;
    unordered_map<PersonID, Node*> map;         // this map contains all the done routing

    Person* src = persons_un.at(fromid);        // find the source person from persons_un
    Node* source = new Node { src, 0, {nullptr, NO_COST}};  // make a new Node pointer of it
    map.insert(std::make_pair(fromid, source));             // and insert it into map.
    pq.push(source);                                        // push source into queue

    while (!pq.empty()) {

        Node* current = pq.top();               // take the person with lowest total distance to source (0 for source itself).
        pq.pop();                               // remove the first item from queue

        vector<pair<Person*, Cost>>& friends = current->p->friends; // create a reference to current's friends
        for (pair<Person*, Cost> pa : friends) {        // for every friend of current:

            Node* node;
            Cost dist_from_source = current->dist + pa.second;  // distance from source node
            auto it = map.find(pa.first->id);                   // see if node has been visited
            if (it == map.end()) {                              // if not, add them and push a new node pointer into queue

                node = new Node {pa.first, dist_from_source, {current, pa.second}};
                map.insert(std::make_pair(pa.first->id, node));
                pq.push(node);

            } else {
                node = it->second;                                      // node has been visited.
                if (node->dist > dist_from_source) {                    // check if path through current is shorter
                                                                        // than current distance. if so,
                    node->dist = dist_from_source;                      // update node's distance and change its previous node into current
                    node->prev = {current, pa.second};

                } else if (current->dist > pa.second + node->dist) {    // also see if the path into current is shorter via this node.
                    current->dist = pa.second + node->dist;             // if so, update current's distance and change its previous node into this node
                    current->prev = {node, pa.second};
                }
            }
        }
        // we have iterated all current's friends, so if current is our target
        // we can end searching, because thanks to priotiy queue's function no shorter paths
        // should exist.
        if (current->p->id == toid) break;
    }

    // all that's left is to find the path from map
    // and write it into a resulting vector
    vector<pair<PersonID, Cost>> result_path;

    auto it = map.find(toid);   // we haven't necessarily found a path, so check if we have

    // if path was found, route it into result_path. else just return an empty vector
    if (it != map.end()) {

        Node* person = it->second;
        // while we're not at fromid
        while (person->prev.second != NO_COST) {

            // here we find a node, check its previous node and push a pair
            // of PersonID and Cost into result_path
            Cost cost = person->prev.second;

            pair<PersonID, Cost> p = std::make_pair(person->p->id, cost);
            result_path.push_back(p);

            person = person->prev.first;
        }

        // we've gone the whole route, but the vector is in reverse order
        std::reverse(result_path.begin(), result_path.end());

    }

    return result_path;
}


/*
vector<pair<PersonID, Cost>> Datastructure::cheapest_friendpath(PersonID fromid, PersonID toid)
{
    struct SmallerCost {

        bool operator()(const pair<Person*, Cost>& lhs, const pair<Person*, Cost>& rhs) const {
            return lhs.second > rhs.second;
        }
    };
    priority_queue<pair<Person*, Cost>, vector<pair<Person*, Cost>>, SmallerCost> pq;

    unordered_map<PersonID, pair<PersonID, Cost>> prev;       // map of nodes and routes to them
    unordered_map<PersonID, Cost> dist;

    Person* source = persons_un.at(fromid);

    dist[source->id] = 0;
    pq.push(std::make_pair(source, 0));
    prev[fromid] = std::make_pair(NO_ID, NO_COST);

    while (!pq.empty()) {

        pair<Person*, Cost> state = pq.top();
        pq.pop();
        unordered_map<PersonID, Cost>::iterator state_it = dist.find(state.first->id);
        Cost dist_state = state_it->second;
        unordered_map<PersonID, pair<PersonID, Cost>>::iterator prev_state = prev.find(state.first->id);

        vector<pair<Person*, Cost>> neighbors = state.first->friends;
        for (pair<Person*, Cost> v : neighbors) {

            Cost dist_from_source = dist_state + v.second;
            unordered_map<PersonID, pair<PersonID, Cost>>::iterator prev_v = prev.find(v.first->id);

            if (prev_v == prev.end()) {

                pair<PersonID, Cost> previous = std::make_pair(state.first->id, v.second);
                prev_v = prev.insert(std::make_pair(v.first->id, previous)).first;
                pq.push(std::make_pair(v.first, dist_from_source));
                dist.insert(std::make_pair(v.first->id, dist_from_source));

            } else {

                auto it = dist.find(v.first->id);
                Cost dist_v = it->second;

                if (dist_from_source < dist_v) {    // shorter total distance, change it to dist
                    it->second = dist_from_source;
                    prev_v->second = std::make_pair(state.first->id, v.second);
                } else if (v.second + dist_v < dist_state) {
                    state_it->second = v.second + dist_v;
                    prev_state->second = std::make_pair(v.first->id, v.second);
                }
            }

        }

    }

    vector<pair<PersonID, Cost>> result_path;

    auto it = prev.find(toid);

    // if path was found, route it into result_path. else just return an empty vector
    if (it != prev.end()) {

        PersonID friendid = it->first;
        // while we're not at fromid
        while (friendid != fromid) {

            Cost cost = it->second.second;
            PersonID previous = it->second.first;

            pair<PersonID, Cost> p = std::make_pair(friendid, cost);
            result_path.push_back(p);
            prev.erase(it);

            it = prev.find(previous);
            friendid = it->first;
        }

        // we've gone the whole route, but the vector is in reverse order
        std::reverse(result_path.begin(), result_path.end());

    }

    return result_path; // Replace this with the actual implementation
}

*/





pair<unsigned int, Cost> Datastructure::leave_cheapest_friendforest()
{
    return {}; // Replace this with the actual implementation
}


















