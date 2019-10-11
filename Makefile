all: lsh cube

lsh.o: lsh.cpp
	g++ -o lsh.o -c lsh.cpp

cube.o: cube.cpp
	g++ -o cube.o -c cube.cpp

utilities.o: utilities.cpp
	g++ -o utilities.o -c utilities.cpp

point.o: point.cpp
	g++ -o point.o -c point.cpp

lsh_functions.o: lsh_functions.cpp
	g++ -o lsh_functions.o -c lsh_functions.cpp

lsh: lsh.o utilities.o point.o lsh_functions.o
	g++ -o lsh lsh.o utilities.o point.o lsh_functions.o

cube: cube.o utilities.o point.o
	g++ -o cube cube.o utilities.o point.o

clean:
	-rm lsh cube lsh.o cube.o utilities.o point.o lsh_functions.o