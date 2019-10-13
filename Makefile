all: lsh cube

lsh.o: lsh.cpp
	g++ -o lsh.o -c lsh.cpp

cube.o: cube.cpp
	g++ -o cube.o -c cube.cpp

utilities.o: utilities.cpp
	g++ -o utilities.o -c utilities.cpp

lsh_functions.o: lsh_functions.cpp
	g++ -o lsh_functions.o -c lsh_functions.cpp

hypercube.o: hypercube.cpp
	g++ -o hypercube.o -c hypercube.cpp

lsh: lsh.o utilities.o lsh_functions.o
	g++ -o lsh lsh.o utilities.o lsh_functions.o

cube: cube.o utilities.o hypercube.o lsh_functions.o
	g++ -o cube cube.o utilities.o hypercube.o lsh_functions.o

clean:
	-rm lsh cube lsh.o cube.o utilities.o lsh_functions.o hypercube.o