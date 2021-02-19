// sudoku.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Game.h"
#include "sudoku.h"

int main()
{
    Game game;
    std::cout << "Loading puzzle...!\n";
    game.Load("easy.txt");
    game.Print();

    game.PrintNote(0, 0);
}

