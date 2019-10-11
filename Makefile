all: curve_grid_lsh curve_grid_hypercube curve_projection_lsh curve_projection_hypercube

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

run1:
	./curve_grid_lsh -d ./Datasets/trajectories_dataset -q query_file -k_vec 4 -L_grid 6 -o output_file

run2:
	./curve_grid_hypercube -d ./Datasets/trajectories_dataset -q query_file -k_hypercube 4 -M 8 -probes 10 -L_grid 6 -o output_file

run3:
	./curve_projection_lsh -d ./Datasets/trajectories_dataset -q query_file -k_vec 4 -L_vec 8 -e 0.4 -o output_file

run4:
	./curve_projection_hypercube -d ./Datasets/trajectories_dataset -q query_file -k_hypercube 4 -M 8 -probes 10 -e 0.7 -o output_file

run_all: run1 run2 run3 run4

clean:
	rm ./curve_* ./build/*
