./curves: curves.o DataHandling.o
	g++ -o ./curves ./build/curves.o ./build/DataHandling.o

curves.o: ./src/Curves/curves.cpp
	g++ -o ./build/curves.o -c ./src/Curves/curves.cpp

DataHandling.o: ./src/Curves/DataHandling.cpp
	g++ -o ./build/DataHandling.o -c ./src/Curves/DataHandling.cpp

clean:
	rm ./curves ./build/curves.o ./build/DataHandling.o
