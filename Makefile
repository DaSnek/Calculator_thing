syntaxinterpreter: token.o syntaxinterpreter.o
	g++ -std=c++11 -o syntaxinterpreter token.o syntaxinterpreter.o

token.o: token.cpp token.h
	g++ -std=c++11 -c token.cpp

syntaxinterpreter.o: syntaxinterpreter.cpp
	g++ -std=c++11 -c syntaxinterpreter.cpp
