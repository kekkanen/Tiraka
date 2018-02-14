// Definition of Datastructure class, hw1 of TIE-20100/TIE-20106

#ifndef DATASTRUCTURE_HPP
#define DATASTRUCTURE_HPP

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <deque>
using std::deque;

struct Person
{
    string name;
    int salary;
};

class Datastructure
{
public:
    Datastructure();
    ~Datastructure();

    void add_person(string name, int salary);

    unsigned int size();
    void clear();

    vector<Person*> personnel_alphabetically();
    vector<Person*> personnel_salary_order();

    Person* min_salary();
    Person* max_salary();

    Person* median_salary();
    Person* first_quartile_salary();
    Person* third_quartile_salary();

private:
    // Add your implementation here

    // variable to determine how the list will be sorted
    enum sort_by {name, salary};

    // list of all persons
    vector<Person*> personnel;

    // has the list been altered since the last sort, e.g. persons added or different sort
    bool sorted_alpha{true};
    bool sorted_salary{true};

    // pointers to persons with lowest and highest salaries
    Person* lowest_salary{nullptr};
    Person* highest_salary{nullptr};

    // the sorting algorithms
    void mergesort(int begin, int end, sort_by s);
    void merge(int begin, int mid, int end, const sort_by s);
};

#endif // DATASTRUCTURE_HPP
