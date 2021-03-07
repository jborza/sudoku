#pragma once
#include <set>
#include <vector>
#include <string>

using namespace std;

class Cell
{
public:
	int value;
	std::set<int> hints;
	std::set<int> systemHints;
	std::set<int> accentHints;
	std::set<int> crossedOutHints;
	bool fixedFromStart;
	inline bool hasValue() { return value != 0; }
	int row;
	int col;
	int house;
	void See(std::vector<Cell*> cells);
	std::string CoordsToString();
	bool SharesRowWith(Cell* b);
	bool SharesColumnWith(Cell* b);
	void SetValue(int value);
	void Clear();

	template<typename T>
	static void RemoveCellsWithValue(std::set<T>& someSet, std::vector<Cell*> cells);

	static void RemoveSelf(vector<Cell*>& cells, Cell* cell);

	static vector<Cell*> Except(vector<Cell*> cells, vector<Cell*> exclusions);

	static vector<Cell*> Except(vector<Cell*> cells, initializer_list<Cell*> exclusions);

	static bool AllShareRow(vector<Cell*> cells);
	static bool AllShareColumn(vector<Cell*> cells);
};


