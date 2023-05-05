all: compile link

compile:
	g++ -I src/include -c main.cpp
	g++ -I src/include -c game.cpp


link:
	g++ main.o game.o -o Teeworlds_Automapper -L src/lib -mwindows -l sfml-graphics -l sfml-window -l sfml-system