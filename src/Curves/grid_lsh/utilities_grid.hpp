#ifndef UTILITIES_GRID_HPP
#define UTILITIES_GRID_HPP

#include "../../Points/lsh/lsh_functions.hpp"
#include "../../Points/cube/hypercube_functions.hpp"
#include "../Curve.hpp"

using namespace std;

class Grid{
		vector<double> a;	// beginning of the grid
		pair<double, double> convert_point(pair<double, double>);
	public:
		Grid(double delta);	
		vector<double> snap(Curve* c, double* max = NULL);
};

class Grid_LSH{
		vector<Curve*> curveset; 
		vector<Grid*> Grids;
		vector<double> snap_num;
		vector<LSH*> lsh;
	public:
		Grid_LSH(vector<Curve*>* curves, int L_grid, int k_vec, int max_d, int min_d);
		~Grid_LSH();
		NN* predict(Curve* curve, int max_d);
};

class Grid_Hypercube{

		vector<Curve*> curveset;
		vector<Grid*> Grids;
		vector<double> snap_num;
		vector<Hypercube*> hypercube;
	public:
		Grid_Hypercube(vector<Curve*>* curves, unsigned int k_hypercube, unsigned int M, unsigned int probes, int L_grid, int max_d, int min_d);
		~Grid_Hypercube();
		NN* predict(Curve* curve, int max_d);
};

double DTW_distance(Curve* x1, Curve* x2);
double eucl_dist(pair<double, double> x, pair<double, double> y);
double min(double x, double y, double z);

void update_output_curves(string* output, string curve_id, NN* found_nearest_neighbor, NN* true_nearest_neighbor,/*int duration, int duration_brute_force,*/ string method, string hashfunction);

NN* curves_brute_force(Curve* curve, vector<Curve*>* dataset);

#endif