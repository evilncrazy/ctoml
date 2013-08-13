CC = g++
CFLAGS = -Wall -Wextra -pedantic -std=c++11 -g
SF = ../src
HF = ../src/include

all : toml

main.o : $(SF)/main.cc $(HF)/toml.h
	$(CC) $(CFLAGS) -c $(SF)/main.cc

tomlvalue.o : $(SF)/tomlvalue.cc $(HF)/tomlvalue.h
	$(CC) $(CFLAGS) -c $(SF)/tomlvalue.cc

toml.o : $(SF)/toml.cc $(HF)/toml.h $(HF)/tomlvalue.h
	$(CC) $(CFLAGS) -c $(SF)/toml.cc

toml : main.o tomlvalue.o toml.o
	$(CC) main.o tomlvalue.o toml.o -o ctoml

clean :
	rm -f *.o ctoml
