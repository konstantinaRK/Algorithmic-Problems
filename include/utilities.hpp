#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <unistd.h> 
#include <cmath>
#include <unordered_map>
#include <set>
#include <chrono> 
#include <utility> // for pair


using namespace std;

class Point{
		string id;
		vector<double> X;
	public:
		Point(string id, vector<double>* X);
		~Point();
		inline string get_id(void) { return this->id; }
		int get_dimension(void);
		double operator[](int i);
};

class Curve
{
	private:
		string curve_id;
		vector <pair <double, double>> points;
	public:
		Curve(string id);
		~Curve();
		inline string get_id(void){ return this->curve_id; }
		void add_point(double x, double y);
		int get_length(void);
		pair<double, double> operator[](int i);
		void clear(void);
	
};

class NN {
		string id;
		double distance;
		vector<string> r_near_neighbors;
	public:
		NN(string id, double distance, set<string>* neighbors=NULL);
		~NN();
		inline string get_id(void){ return this->id; } 
		inline double get_distance(void){ return this->distance; }
		unsigned int r_near_neighbors_size();
		string get_near_neighbor(int i);
		vector<string> get_near_neighbors(void);
		void add_neighbors(vector<string>* new_neighbors);
};

template <class C>
void delete_vector(vector<C*>* v){

	for (int i = 0; i < (*v).size(); ++i)
		delete (*v)[i];

	(*v).clear();
}

NN* brute_force(Point* point, vector<Point*>* pointset);
NN* curves_brute_force(Curve* curve, vector<Curve*>* dataset);

double DTW_distance(Curve* x1, Curve* x2);
double eucl_dist(pair<double, double> x, pair<double, double> y);
double manhattan_dist(Point* x, Point* y);

double average_distance(vector<Point*>* pointset);
double min(double x, double y, double z);

#endif