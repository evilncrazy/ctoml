CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++0x -g
SF = ../src
HF = ../src/include
BF = ../build

all : tomltest

main.o : main.cc $(HF)/toml.h
	$(CC) $(CFLAGS) -c main.cc

tomltest : main.o
	$(CC) main.o $(BF)/tomlvalue.o $(BF)/toml.o -o tomltest
