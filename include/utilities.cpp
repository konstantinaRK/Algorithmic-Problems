#include "./utilities.hpp"

using namespace std;

Point::Point(string id, vector<double>* X){

	this->id = id;
	for (int i=0; i<(*X).size(); i++){
		this->X.push_back((*X)[i]);
	}
}

Point::~Point(){
	this->X.clear();
}

int Point::get_dimension(void){
	return this->X.size();
}

double Point::operator[](int i){
	 if ( i>=0 && i<this->X.size() )
	 	return this->X[i];
	 else
	 	return -1; 
}

Curve::Curve(string id){
	this->curve_id = id;
}

Curve::~Curve()
{
	this->points.clear();
}

void Curve::add_point(double x, double y){

	this->points.push_back(make_pair(x, y));
}

int Curve::get_length(void){

	return this->points.size();
}

pair<double, double> Curve::operator[](int i){

	return this->points[i];
}


void Curve::clear(void){

	this->points.clear();
}

NN::NN(string id, double distance, set<string>* neighbors){

	this->id = id;
	this->distance = distance;

	if ( neighbors != NULL ){
		(this->r_near_neighbors).insert((this->r_near_neighbors).end(), (*neighbors).begin(), (*neighbors).end());
		(*neighbors).clear();
	}
}

NN::~NN(){
	this->r_near_neighbors.clear();
}

string NN::get_near_neighbor(int i){

	if ( i<0 || i >= (this->r_near_neighbors).size())
		return "";
	return (this->r_near_neighbors)[i];
}

unsigned int NN::r_near_neighbors_size(){
	return (this->r_near_neighbors).size();
}

vector<string> NN::get_near_neighbors(void){
	return this->r_near_neighbors;
}

void NN::add_neighbors(vector<string>* new_neighbors){

	for (int i = 0; i < (*new_neighbors).size(); ++i)
	{
		(this->r_near_neighbors).push_back((*new_neighbors)[i]);
	}
}

NN* brute_force(Point* point, vector<Point*>* pointset){

	string min_id = "";
	int min_distance;

	int current_distance;
	for (int i = 0; i < (*pointset).size(); ++i)	// For every point
	{
		if ( (point->get_id()).compare((*pointset)[i]->get_id()) != 0 )	// If the point has different id from query point
		{
			// Calculate the distance
			current_distance = manhattan_dist(point, (*pointset)[i]);

			// Replace min if this is the first point or if current distance < min distance
			if ( min_id.compare("") == 0 || current_distance < min_distance )
			{
				min_distance = current_distance;
				min_id = (*pointset)[i]->get_id();
			}
		}

	}

	// If i have found a nearest neighbor
	if ( min_id.compare("") !=0 )
	{
		NN * nearest_neighbor;
		try{
			nearest_neighbor = new NN(min_id, min_distance);
		}
		catch(bad_alloc&)
		{
			cerr << "No memory available" << endl;
			nearest_neighbor = NULL;
		}

		return nearest_neighbor;
	}
	else	// If there is no point except this point in the pointset
		return NULL;
}

NN* curves_brute_force(Curve* curve, vector<Curve*>* dataset){

	double min_dist;
	string min_id = "";

	double cur_dist;
	for (int i = 0; i < (*dataset).size(); ++i)	// FOr every curve
	{
		// Calculate current distance
		cur_dist = DTW_distance(curve, (*dataset)[i]);

		// If this is the first curve of if the current distance < min_distance
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

double eucl_dist(pair<double, double> x, pair<double, double> y){

	double x1 = x.first - y.first;
	double x2 = x.second - y.second;
if (sqrt((x1*x1)+(x2*x2)) < 0) cout << sqrt((x1*x1)+(x2*x2)) << endl;
	return (sqrt((x1*x1)+(x2*x2)));
}

double manhattan_dist(Point* x, Point* y){

	double distance = 0;
	int xi, yi;
	double d;
	for (int i = 0; i < x->get_dimension(); ++i)
	{
		xi = (*x)[i];
		yi = (*y)[i];

		d = xi - yi;
		if ( d >= 0 )
			distance += d;
		else
			distance += yi - xi; 
	}

	return distance;
}

double average_distance(vector<Point*>* pointset){


	int size = (*pointset).size();
	int step = floor(sqrt(size));

	int sum_d = 0;
	NN * nearest_neighbor;
	for (int i = 0; i < size; i+=step)	// For points in pointset
	{
		// Find nearest neighbor
		nearest_neighbor = brute_force((*pointset)[i], &(*pointset));
		sum_d += nearest_neighbor->get_distance();
		delete nearest_neighbor;
	}

	// Return the average nearest neighbor distance
	return sum_d/(size/step);
}

double min(double x, double y, double z){

	double min = x;
	if ( y < min && z >= y )
		return y;
	else if ( z < min )
		return z;
	return min;
}