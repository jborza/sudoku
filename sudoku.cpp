// sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Game.h"
#include "Solver.h"
#include "sudoku.h"


int main()
{
	Game game;
	std::cout << "Loading puzzle...!\n";
	// game.Load("easy.txt");
	// game.Load("test-nakedsingle.txt");
	game.Load("test-hiddensingle.txt");
	game.Print();
	game.grid.AutoNoteUser();

	for (int col = 0; col < 8; col++)
		game.PrintNote(col, 0);
	game.PrintNote(0, 1);
	game.PrintNote(8, 0);
	game.PrintNote(8, 8);
	Solver solver;
	auto hint = solver.Hint(&(game.grid));
	cout << "Hint:" << endl;
	cout << hint.message << endl;
	cout << "Affects cells at: ";
	for(auto cell : hint.cellsToHighlight)
		cout << cell->CoordsToString() << " ";
	cout << endl;	
}

