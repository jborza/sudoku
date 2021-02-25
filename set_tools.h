#pragma once
#include <set>
#include <string>
#include <iterator>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

template<typename T>
bool contains(std::set<T>& someSet, T item)
{
	return someSet.find(item) != someSet.end();
}

template<typename T>
bool contains(initializer_list<T>& list, T item)
{
	return find(list.begin(), list.end(), item) != list.end();
}

//TODO refactor for all containers (set, initializer_list, set)
template<typename T>
bool contains(vector<T>& list, T item)
{
	return find(list.begin(), list.end(), item) != list.end();
}

template<typename T>
T first(vector<T>& container) {
	return *(container.begin());
}

template<typename T>
string comma_separated(vector<T>& container) {
	ostringstream oss;
	if (!container.empty()) {
		copy(container.begin(), prev(container.end()), ostream_iterator<T>(oss, ", "));
		T last = *prev(container.end());
		oss << last;

	}
	return oss.str();
}

template<typename T>
string comma_separated(set<T>& someSet) {
	ostringstream oss;
	if (!someSet.empty()) {
		copy(someSet.begin(), prev(someSet.end()), ostream_iterator<T>(oss, ", "));
		T last = *prev(someSet.end());
		oss << last;

	}
	return oss.str();
}