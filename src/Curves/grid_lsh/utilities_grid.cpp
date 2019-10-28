#include "./utilities_grid.hpp"

#include <unordered_map>
#include <cmath>

using namespace std;

Grid::Grid(double delta){
	
	random_device rd;  //Will be used to obtain a seed for the random number engine
	mt19937 generator(rd()); //Standard mersenne_twister_engine seeded with rd()
	uniform_real_distribution<> distribution(0.0,2.0);

	for (int i = 0; i < 2; ++i)
		this->a.push_back(delta + distribution(generator));
}

// Point * Grid::snap(Curve* c, int pos, int d_vec){

// 	vector<double> snap;
// 	pair<double, double> prev, converted_point;
// 	for (int i = 0; i < c->get_length(); ++i)
// 	{
// 		converted_point = convert_point((*c)[i]); 
// 		// Ignore consecutive duplicates
// 		if ( i!=0 && converted_point == prev ){
// 			// cout << "continue" << endl;
// 			continue;
// 		}
// 		prev = converted_point;
// 		snap.push_back(converted_point.first);
// 		snap.push_back(converted_point.second);
// 	}

// 	// Adjust dimension
// 	for (int i = snap.size(); i < d_vec; ++i)
// 	{
// 		snap.push_back(0);
// 	}

// 	Point* point = NULL;
// 	try{
// 		point = new Point(to_string(pos), &snap);
// 	}
// 	catch(bad_alloc&){
// 		cerr << "snap: No memory available" << endl;
// 		return NULL;
// 	}

// 	snap.clear();
// 	return point;
// }

vector<double> Grid::snap(Curve* c, double* max){

	vector<double> snap;
	pair<double, double> prev, converted_point;
	for (int i = 0; i < c->get_length(); ++i)
	{
		converted_point = convert_point((*c)[i]); 
		// Ignore consecutive duplicates
		if ( i!=0 && converted_point == prev ){
			// cout << "continue" << endl;
			continue;
		}
		prev = converted_point;
		snap.push_back(converted_point.first);
		snap.push_back(converted_point.second);

		if ( max != NULL ){
			if ( converted_point.first > (*max) )
				(*max) = converted_point.first;
			if (converted_point.second > (*max))
				(*max) = converted_point.second;
		}
	}

	return snap;
}

pair<double, double> Grid::convert_point(pair<double, double> point){

	pair<double, double> converted_point;
	converted_point.first = point.first -this->a[0];
	converted_point.second = point.second -this->a[1];

// cout << "before " << point.first << " " << point.second << " after " << converted_point.first << " " << converted_point.second << endl;

	return converted_point;
}

double DTW_distance(Curve* x1, Curve* x2){

	int m1 = x1->get_length();
	int m2 = x2->get_length();

	double** C;
	try{
		C = new double*[m1];
		for (int i = 0; i < m1; ++i)
		{
			C[i] = new double[m2];
		}
	}
	catch(bad_alloc&){
		cerr << "DTW_distance: No memory available" << endl;
		return -1;
	}

	C[0][0] = eucl_dist((*x1)[0], (*x2)[0]);

	// Fill the first row
	for (int j = 1; j < m2; ++j)
	{
		C[0][j] = C[0][j-1] + eucl_dist((*x1)[0], (*x2)[j]);
	}

	// Fill the first column
	for (int i = 1; i < m1; ++i){
		C[i][0] = C[i-1][0] + eucl_dist((*x1)[i], (*x2)[0]);

	}

	// Fill the rest of the matrix
	for (int i = 1; i < m1; ++i)
	{
		for (int j = 1; j < m2; ++j)
		{
			C[i][j] = min(C[i-1][j], C[i-1][j-1], C[i][j-1]) + eucl_dist((*x1)[i], (*x2)[j]);
		}
	}

	double distance = C[m1-1][m2-1];
	// Free table
	for (int i = 0; i < m1; ++i)
	{
		delete [] C[i];
	}
	delete [] C;

	return distance;

}

double min(double x, double y, double z){

	double min = x;
	if ( y < min && z >= y )
		return y;
	else if ( z < min )
		return z;
	return min;
}

double eucl_dist(pair<double, double> x, pair<double, double> y){

	double x1 = x.first - y.first;
	double x2 = x.second - y.second;
if (sqrt((x1*x1)+(x2*x2)) < 0) cout << sqrt((x1*x1)+(x2*x2)) << endl;
	return (sqrt((x1*x1)+(x2*x2)));
}

void update_output_curves(string* output, string curve_id, NN* found_nearest_neighbor, NN* true_nearest_neighbor,/*int duration, int duration_brute_force,*/ string method, string hashfunction){

	string query = "Query: "+curve_id+"\nMethod: "+method+"\nHashFunction: "+hashfunction+"\nFound Nearest neighbor: "+found_nearest_neighbor->get_id()+"\nTrue Nearest neighbor: "+true_nearest_neighbor->get_id()
					+"\ndistanceFound: "+to_string(found_nearest_neighbor->get_distance())+"\ndistanceTrue: "+to_string(true_nearest_neighbor->get_distance());
					//+"\ntHypercube: "+to_string(duration_hypercube)+"\ntTrue: "+to_string(duration_brute_force)+"\n\n";
	(*output) = (*output) + query;
}

NN* curves_brute_force(Curve* curve, vector<Curve*>* dataset){

	double min_dist;
	string min_id = "";

	double cur_dist;
	for (int i = 0; i < (*dataset).size(); ++i)
	{
		cur_dist = DTW_distance(curve, (*dataset)[i]);
		if ( min_id.compare("") == 0 || cur_dist < min_dist ){
			min_dist = cur_dist;
			min_id = ((*dataset)[i])->get_id();
		}
	}

	// If there is no neighbor
	if ( min_id.compare("") == 0 )
		return NULL;
	else
	{
		NN* nearest_neighbor;
		try{
			nearest_neighbor = new NN(min_id, min_dist);
		}
		catch(bad_alloc&){
			return NULL;
		}
		return nearest_neighbor;
	}
}

Grid_LSH::Grid_LSH(vector<Curve*>* curves, int L_grid, int k_vec, int max_d, int min_d){

	double delta = 8*min_d/10000.0;

	// Create curveset
	for (int i = 0; i < (*curves).size(); ++i)
	{
		this->curveset.push_back((*curves)[i]);
	}
	// (*curves).clear();

	// Create Grids and LSHs
	if ( this->curveset.size() > 0 ){
		for (int i = 0; i < L_grid; ++i)
		{
	cout << "grid " << i << endl;
			Grids.push_back(new Grid(delta));

			// Convert curves to points
			vector<vector<double>> points;
			double max_coord= 0;
			for (int j = 0; j < (this->curveset).size(); ++j)
			{
				points.push_back(((this->Grids)[i])->snap((this->curveset)[j], &max_coord));

			}
			this->snap_num.push_back(max_coord+1);

			// Adjust d of points
			int cur_d;
			for (int j = 0; j < points.size(); ++j)
			{
				cur_d = points[j].size();
				for (int k = cur_d; k < max_d; ++k)
				{
					points[j].push_back((this->snap_num)[i]);
				}
			}

			// Create points
			vector<Point*> pointset;
			Point* point;
			for (int j = 0; j < points.size(); ++j)
			{
				point = new Point(to_string(j), &(points[j]));
				pointset.push_back(point);
			}


			lsh.push_back(new LSH(&pointset, 1, k_vec, max_d));
			// lsh[i]->print_points();
			pointset.clear();
		}
	}
}

Grid_LSH::~Grid_LSH(){

	delete_vector<LSH>(&(this->lsh));
	delete_vector<Grid>(&(this->Grids));
	delete_vector<Curve>(&(this->curveset));
	this->snap_num.clear();
}

NN* Grid_LSH::predict(Curve* curve, int max_d){

	string min_id = "";
	double min_dist;

	Point* point;
	Curve* cur_curve;
	double cur_dist;

	for (int i = 0; i < (this->Grids).size(); ++i)
	{

		// Convert curve to point
		vector<double> point_coord = ((this->Grids)[i])->snap(curve);
		int cur_d = point_coord.size();
		for (int j = cur_d; j < max_d; ++j)
		{
			point_coord.push_back((this->snap_num)[i]);
		}

		Point * point = new Point("0", &point_coord);
		point_coord.clear();

		if (point == NULL)
			return NULL;
		
		// Find point's nearest neighbor
		NN* nearest_neighbor = (this->lsh)[i]->predict(point);
		if (nearest_neighbor == NULL){
			delete point;
			continue;
		}

		// Find the actual distance between this curve and the nn point's curve
		cur_curve =  (this->curveset)[stoi(nearest_neighbor->get_id())];
		cur_dist = DTW_distance(curve, cur_curve);

		if ( min_id.compare("") == 0 || cur_dist < min_dist )
		{
			min_id = cur_curve->get_id();
			min_dist = cur_dist;
		}

		delete nearest_neighbor;
		delete point;
	}

	// If i haven't found a neighbor
	if ( min_id.compare("") == 0 )
		return NULL;
	else
	{
		NN* nearest_neighbor;
		try{
			nearest_neighbor = new NN(min_id, min_dist);
		}
		catch(bad_alloc&){
			cerr << "grid_lsh_nearest_neighbor: No memory available" << endl;
			return NULL;
		}
		return nearest_neighbor;
	}

}

Grid_Hypercube::Grid_Hypercube(vector<Curve*>* curves, unsigned int k_hypercube, unsigned int M, unsigned int probes, int L_grid, int max_d, int min_d){

	double delta = 8*min_d/1000.0;

	// Create the curveset
	for (int i = 0; i < (*curves).size(); ++i)
	{
		this->curveset.push_back((*curves)[i]);
	}
	// (*curves).clear();

	// Create Grids and hypercubes
	for (int i = 0; i < L_grid; ++i)
	{
cout << " grid " << i << endl;
		Grids.push_back(new Grid(delta));

		// Convert curves to points
		vector<vector<double>> points;
		double max_coord = 0;
		for (int j = 0; j < (this->curveset).size(); ++j)
		{
			points.push_back(((this->Grids)[i])->snap((this->curveset)[j], &max_coord));
		}
		this->snap_num.push_back(max_coord+1);

		// Adjust d of points
		int cur_d;
		for (int j = 0; j < points.size(); ++j)
		{
			cur_d = points[j].size();
			for (int k = cur_d; k < max_d; ++k)
			{
				points[j].push_back((this->snap_num)[i]);
			}
		}
		
		// Create points
		vector<Point*> pointset;
		Point* point;
		for (int j = 0; j < points.size(); ++j)
		{
			point = new Point(to_string(j), &(points[j]));
			pointset.push_back(point);
		}

		hypercube.push_back(new Hypercube(&pointset, max_d, k_hypercube, M, probes));
		pointset.clear();
	}
}

Grid_Hypercube::~Grid_Hypercube(){

	delete_vector<Hypercube>(&(this->hypercube));
	delete_vector<Grid>(&(this->Grids));
	delete_vector<Curve>(&(this->curveset));
	this->snap_num.clear();
}

NN* Grid_Hypercube::predict(Curve* curve, int max_d){

	string min_id = "";
	double min_dist;

	Point* point;
	Curve* cur_curve;
	double cur_dist;

	for (int i = 0; i < (this->Grids).size(); ++i)
	{
		// Convert curve to point
		vector<double> point_coord = ((this->Grids)[i])->snap(curve);
		int cur_d = point_coord.size();
		for (int j = cur_d; j < max_d; ++j)
		{
			point_coord.push_back((this->snap_num)[i]);
		}

		Point * point = new Point("0", &point_coord);
		point_coord.clear();

		if (point == NULL)
			return NULL;
		
		// Find point's nearest neighbor
		NN* nearest_neighbor = (this->hypercube)[i]->predict(point);
		if (nearest_neighbor == NULL){
			delete point;
			continue;
		}

		// Find the actual distance between this curve and the nn point's curve
		cur_curve =  (this->curveset)[stoi(nearest_neighbor->get_id())];
		cur_dist = DTW_distance(curve, cur_curve);

		if ( min_id.compare("") == 0 || cur_dist < min_dist )
		{
			min_id = cur_curve->get_id();
			min_dist = cur_dist;
		}

		delete nearest_neighbor;
		delete point;
	}

	// If i haven't found a neighbor
	if ( min_id.compare("") == 0 )
		return NULL;
	else
	{
		NN* nearest_neighbor;
		try{
			nearest_neighbor = new NN(min_id, min_dist);
		}
		catch(bad_alloc&){
			cerr << "grid_lsh_nearest_neighbor: No memory available" << endl;
			return NULL;
		}
		return nearest_neighbor;
	}
}
