CFLAGS = -Wall -Wextra -std=c++11

game: main.o  iron-dome-program.o
	g++ $(CFLAGS) -o game main.o  iron-dome-program.o -I include -L lib -lraylib -lGL -lX11 -lm
main.o: main.cpp main.h
	g++ $(CFLAGS) -c main.cpp
iron-dome-program.o : programs/iron-dome/*
	g++ $(CFLAGS) -c programs/iron-dome/iron-dome.cpp -I include -L lib -lraylib -lGL -lX11 -lm -o ./iron-dome-program.o