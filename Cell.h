#pragma once
#include <set>
class Cell
{
public:
	int value;
	std::set<uint8_t> hints;
	std::set<uint8_t> accentHints; 
	bool fixedFromStart;
	bool hasValue;
};

