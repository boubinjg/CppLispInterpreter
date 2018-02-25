CC=g++
OBJ = lisp

lisp: lisp.cpp
	g++ -std=c++11 -O3 -Wall -g lisp.cpp -o $(OBJ)
