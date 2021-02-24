#pragma once
#include <set>
#include <string>
#include <iterator>
#include <iostream>
#include <sstream>

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

template<typename T>
string comma_separated(std::set<T>& someSet) {
	std::ostringstream oss;
	if (!someSet.empty()) {
		copy(someSet.begin(), prev(someSet.end()), ostream_iterator<T>(oss, ","));
		T last = *prev(someSet.end());
		oss << last;

	}
	return oss.str();
}