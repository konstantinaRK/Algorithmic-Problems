CC = g++
CFLAGS = -g

all: ./bin/lsh ./bin/cube ./bin/curve_grid_lsh ./bin/curve_grid_hypercube ./bin/curve_projection_lsh ./bin/curve_projection_hypercube

./bin/lsh: ./build/lsh.o ./build/utilities.o ./build/LSH_Structure.o ./build/DataHandling.o
	$(CC) -o ./bin/lsh ./build/lsh.o ./build/utilities.o ./build/LSH_Structure.o ./build/DataHandling.o

./bin/cube: ./build/cube.o ./build/utilities.o ./build/Hypercube_Structure.o ./build/LSH_Structure.o ./build/DataHandling.o
	$(CC) -o ./bin/cube ./build/cube.o ./build/utilities.o ./build/Hypercube_Structure.o ./build/LSH_Structure.o ./build/DataHandling.o

./bin/curve_grid_lsh: ./build/grid_lsh.o ./build/DataHandling.o ./build/utilities.o ./build/LSH_Structure.o ./build/Grid.o ./build/Hypercube_Structure.o
	$(CC) -o ./bin/curve_grid_lsh ./build/grid_lsh.o ./build/DataHandling.o ./build/utilities.o ./build/LSH_Structure.o ./build/Grid.o ./build/Hypercube_Structure.o

./bin/curve_grid_hypercube: ./build/grid_hypercube.o ./build/DataHandling.o ./build/Hypercube_Structure.o ./build/utilities.o ./build/LSH_Structure.o ./build/Grid.o
	$(CC) -o ./bin/curve_grid_hypercube ./build/grid_hypercube.o ./build/DataHandling.o ./build/Hypercube_Structure.o ./build/utilities.o ./build/LSH_Structure.o ./build/Grid.o

./bin/curve_projection_lsh: ./build/projection_lsh.o ./build/DataHandling.o ./build/Projection.o ./build/LSH_Structure.o ./build/utilities.o ./build/Hypercube_Structure.o
	$(CC) -o ./bin/curve_projection_lsh ./build/projection_lsh.o ./build/DataHandling.o ./build/Projection.o ./build/LSH_Structure.o ./build/utilities.o ./build/Hypercube_Structure.o

./bin/curve_projection_hypercube: ./build/projection_hypercube.o ./build/DataHandling.o ./build/Projection.o ./build/Hypercube_Structure.o ./build/utilities.o ./build/LSH_Structure.o
	$(CC) -o ./bin/curve_projection_hypercube ./build/projection_hypercube.o ./build/DataHandling.o ./build/Projection.o ./build/Hypercube_Structure.o ./build/utilities.o ./build/LSH_Structure.o


# Main code
./build/lsh.o: ./src/Points/lsh/lsh.cpp
	$(CC) -o ./build/lsh.o -c ./src/Points/lsh/lsh.cpp

./build/cube.o: ./src/Points/cube/cube.cpp
	$(CC) -o ./build/cube.o -c ./src/Points/cube/cube.cpp

./build/grid_lsh.o: ./src/Curves/Grid/grid_lsh/grid_lsh.cpp
	$(CC) -o ./build/grid_lsh.o -c ./src/Curves/Grid/grid_lsh/grid_lsh.cpp

./build/grid_hypercube.o: ./src/Curves/Grid/grid_hypercube/grid_hypercube.cpp
	$(CC) -o ./build/grid_hypercube.o -c ./src/Curves/Grid/grid_hypercube/grid_hypercube.cpp

./build/projection_lsh.o: ./src/Curves/Projection/projection_lsh/projection_lsh.cpp
	$(CC) -o ./build/projection_lsh.o -c ./src/Curves/Projection/projection_lsh/projection_lsh.cpp

./build/projection_hypercube.o: ./src/Curves/Projection/projection_hypercube/projection_hypercube.cpp
	$(CC) -o ./build/projection_hypercube.o -c ./src/Curves/Projection/projection_hypercube/projection_hypercube.cpp


# Libraries
./build/LSH_Structure.o: ./include/LSH_Structure.cpp
	$(CC) ${CFLAGS} -c -o ./build/LSH_Structure.o ./include/LSH_Structure.cpp

./build/Hypercube_Structure.o: ./include/Hypercube_Structure.cpp
	$(CC) ${CFLAGS} -c -o ./build/Hypercube_Structure.o ./include/Hypercube_Structure.cpp

./build/Grid.o: ./include/Grid.cpp
	$(CC) ${CFLAGS} -c -o ./build/Grid.o ./include/Grid.cpp

./build/Projection.o: ./include/Projection.cpp
	$(CC) ${CFLAGS} -c -o ./build/Projection.o ./include/Projection.cpp

./build/utilities.o: ./include/utilities.cpp
	$(CC) ${CFLAGS} -c -o ./build/utilities.o ./include/utilities.cpp

./build/DataHandling.o: ./include/DataHandling.cpp
	$(CC) ${CFLAGS} -c -o ./build/DataHandling.o ./include/DataHandling.cpp

clean:
	-rm ./bin/* ./build/*