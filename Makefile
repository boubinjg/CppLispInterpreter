CC=g++
OBJ = lisp

lisp: lisp.cpp
	g++ -std=c++0x -O3 -Wall -g lisp.cpp -o $(OBJ)
