// Definition of Datastructure class, hw3 of TIE-20100/TIE-20106

#include "datastructure.hpp"

// For debug
#include <iostream>
using std::cerr;
using std::endl;

// Helper function to return a random (enough) number in range start-end
// I.e. random_in_range(1,4) produces a random number between 1, 2, 3, or 4.
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
    // makes I/O streams faster
    std::ios_base::sync_with_stdio(false);

}

Datastructure::~Datastructure()
{

}

// adds a new person into the personnel vector. also checks whether the person
// to be added has lowest or highest salary and appoints them to the corresponding pointer.
void Datastructure::add_person(std::string name, int salary)
{
    Person* p = new Person();
    p->name = name;
    p->salary = salary;
    personnel.push_back(p);
    sorted_alpha = false;
    sorted_salary = false;

    // checks if the new person has lower or higher salary than the current record holders.
    // if lower, lowest_salary points to p. same with the highest_salary.
    // when adding the first person to the vector, the person added will hold both titles - lowest and highest salary
    if (personnel.size() == 1) {
        highest_salary = p;
        lowest_salary = p;
    } else
    if (p->salary > highest_salary->salary) {
        highest_salary = p;
    } else if (p->salary < lowest_salary->salary) {
        lowest_salary = p;
    }
}

unsigned int Datastructure::size()
{
    return personnel.size();
}

void Datastructure::clear()
{
    // a new empty vector is created and
    // its contents are swapped with the
    // personnel vector. the temp vector
    // is destroyed when this scope ends,
    // leaving the personnel vector empty
    vector<Person*>().swap(personnel);

    // naturally these must be reset
    lowest_salary = nullptr;
    highest_salary = nullptr;
}

vector<Person *> Datastructure::personnel_alphabetically()
{
    // if the vector is already in alphabetical order it needs no sorting
    if (!sorted_alpha) {
    mergesort(0, personnel.size()-1, name);
    sorted_alpha = true;
    sorted_salary = false;
    }
    return personnel;
}

vector<Person *> Datastructure::personnel_salary_order()
{
    // if the vector is already in salary order it needs no sorting
    if (!sorted_salary) {
        mergesort(0, personnel.size()-1, salary);
        sorted_salary = true;
        sorted_alpha = false;
    }
    return personnel;
}

Person* Datastructure::min_salary()
{
    return lowest_salary;
}

Person* Datastructure::max_salary()
{
    return highest_salary;
}

// returns a pointer to the person at the median position
Person* Datastructure::median_salary()
{
    // if not sorted, sort
    if (!sorted_salary) {
        personnel_salary_order();
    }

    // median position = size / 2
    int pos = personnel.size() / 2;
    if (pos > 0) return personnel.at(pos);
    else return nullptr;
}

Person* Datastructure::first_quartile_salary()
{
    // if not sorted, sort
    if (!sorted_salary) {
        personnel_salary_order();
    }

    int pos = personnel.size() / 4;
    if (pos > 0) return personnel.at(pos);
    else return nullptr;
}

Person* Datastructure::third_quartile_salary()
{
    // if not sorted, sort
    if (!sorted_salary) {
        personnel_salary_order();
    }
    int pos = (3 * personnel.size()) / 4;
    if (pos > 0) return personnel.at(pos);
    else return nullptr;
}


void Datastructure::mergesort(int begin, int end, sort_by s) {

    // already sorted
    if (end <= begin) {
        return;
    }

    // divide vector into two parts: begin -> mid &
    int mid = (begin + end) / 2;


    // sort first half
    mergesort(begin, mid, s);
    // second half
    mergesort(mid+1, end, s);
    // merge halves
    merge(begin, mid, end, s);
}

void Datastructure::merge(int begin, int mid, int end, const sort_by s) {

    int left = begin;
    int right = mid + 1;
    int stop  = end;

    // vector to store ordered sublist
    vector<Person*> result;
    // result.reserve(end + 1);
    if (s == salary) {

        // first iterate through 2 halves of subsection personnel[begin:end].
        while (left <= mid && right <= stop) {

            // if the last item in left side is smaller than the item
            // currently at personnel[right], the values are in order
            // from left to mid + right to end. No need to check, only push_back in order
            // and break from while when done
            if (personnel[mid]->salary < personnel[right]->salary) {
                while (left <= mid) {
                    result.push_back(personnel[left]);
                    left++;
                }
                while (right <= stop) {
                    result.push_back(personnel[right]);
                    right++;
                }
                break;
            }
            // if left < right, push left to result and increment
            // left. else push right to result and increment right
            if (personnel[left]->salary < personnel[right]->salary) {
                result.push_back(personnel[left]);
                left++;
            } else {
                result.push_back(personnel[right]);
                right++;
            }
        }
        // push the rest from either side to result. they are
        // in order now. only one loop will be entered
        while (right <= stop) {
            result.push_back(personnel[right]);
            right++;
        }
        while (left <= mid) {
            result.push_back(personnel[left]);
            left++;
        }

    } else { // the same logic as above, but with name, not salary

        while (left <= mid && right <= stop) {

            if (personnel[mid]->name < personnel[right]->name) {
                while (left <= mid) {
                    result.push_back(personnel[left]);
                    left++;
                }
                while (right <= stop) {
                    result.push_back(personnel[right]);
                    right++;
                }
                break;
            }
            if (personnel[left]->name < personnel[right]->name) {
                result.push_back(personnel[left]);
                left++;
            } else {
                result.push_back(personnel[right]);
                right++;
            }
        }
        while (right <= stop) {
            result.push_back(personnel[right]);
            right++;
        }
        while (left <= mid) {
            result.push_back(personnel[left]);
            left++;
        }

    }
    // replace personnel[begin:end] with the result vector
    int k = begin;
    for (unsigned int i = 0; i < result.size(); ++i) {
        personnel[k] = result[i];
        ++k;
    }
}













