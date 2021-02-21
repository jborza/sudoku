CXX=clang++

all: compile

compile:
	$(CXX) -g -o sudoku -Wall *.cpp