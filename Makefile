all: lsh cube curve_grid_lsh curve_grid_hypercube curve_projection_lsh curve_projection_hypercube

lsh: ./build/lsh.o ./build/utilities.o ./build/lsh_functions.o
	g++ -o ./lsh ./build/lsh.o ./build/utilities.o ./build/lsh_functions.o

cube: ./build/cube.o ./build/utilities.o ./build/hypercube.o ./build/lsh_functions.o
	g++ -o ./cube ./build/cube.o ./build/utilities.o ./build/hypercube.o ./build/lsh_functions.o

./build/lsh.o: ./src/Points/lsh/lsh.cpp
	g++ -o ./build/lsh.o -c ./src/Points/lsh/lsh.cpp

./build/cube.o: ./src/Points/cube/cube.cpp
	g++ -o ./build/cube.o -c ./src/Points/cube/cube.cpp

./build/hypercube.o: ./src/Points/cube/hypercube.cpp
	g++ -o ./build/hypercube.o -c ./src/Points/cube/hypercube.cpp

./build/utilities.o: ./src/Points/utilities.cpp
	g++ -o ./build/utilities.o -c ./src/Points/utilities.cpp

./build/lsh_functions.o: ./src/Points/lsh_functions.cpp
	g++ -o ./build/lsh_functions.o -c ./src/Points/lsh_functions.cpp

curve_grid_lsh: ./build/grid_lsh.o ./build/DataHandling1.o ./build/Curve.o
	g++ -o ./curve_grid_lsh ./build/grid_lsh.o ./build/DataHandling1.o ./build/Curve.o

curve_grid_hypercube: ./build/grid_hypercube.o ./build/DataHandling2.o ./build/Curve.o
	g++ -o ./curve_grid_hypercube ./build/grid_hypercube.o ./build/DataHandling2.o ./build/Curve.o

curve_projection_lsh: ./build/projection_lsh.o ./build/DataHandling3.o ./build/Curve.o
	g++ -o ./curve_projection_lsh ./build/projection_lsh.o ./build/DataHandling3.o ./build/Curve.o

curve_projection_hypercube: ./build/projection_hypercube.o ./build/DataHandling4.o ./build/Curve.o
	g++ -o ./curve_projection_hypercube ./build/projection_hypercube.o ./build/DataHandling4.o ./build/Curve.o

./build/grid_lsh.o: ./src/Curves/grid_lsh/grid_lsh.cpp
	g++ -o ./build/grid_lsh.o -c ./src/Curves/grid_lsh/grid_lsh.cpp

./build/DataHandling1.o: ./src/Curves/grid_lsh/DataHandling.cpp
	g++ -o ./build/DataHandling1.o -c ./src/Curves/grid_lsh/DataHandling.cpp

./build/grid_hypercube.o: ./src/Curves/grid_hypercube/grid_hypercube.cpp
	g++ -o ./build/grid_hypercube.o -c ./src/Curves/grid_hypercube/grid_hypercube.cpp

./build/DataHandling2.o: ./src/Curves/grid_hypercube/DataHandling.cpp
	g++ -o ./build/DataHandling2.o -c ./src/Curves/grid_hypercube/DataHandling.cpp

./build/projection_lsh.o: ./src/Curves/projection_lsh/projection_lsh.cpp
	g++ -o ./build/projection_lsh.o -c ./src/Curves/projection_lsh/projection_lsh.cpp

./build/DataHandling3.o: ./src/Curves/projection_lsh/DataHandling.cpp
	g++ -o ./build/DataHandling3.o -c ./src/Curves/projection_lsh/DataHandling.cpp

./build/projection_hypercube.o: ./src/Curves/projection_hypercube/projection_hypercube.cpp
	g++ -o ./build/projection_hypercube.o -c ./src/Curves/projection_hypercube/projection_hypercube.cpp

./build/DataHandling4.o: ./src/Curves/projection_hypercube/DataHandling.cpp
	g++ -o ./build/DataHandling4.o -c ./src/Curves/projection_hypercube/DataHandling.cpp

./build/Curve.o: ./src/Curves/Curve.cpp
	g++ -o ./build/Curve.o -c ./src/Curves/Curve.cpp

clean:
	-rm ./lsh ./cube  ./curve_* ./build/*