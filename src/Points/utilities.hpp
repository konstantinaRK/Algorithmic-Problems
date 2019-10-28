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
double manhattan_dist(Point* x, Point* y);
double average_distance(vector<Point*>* pointset);

bool check_for_new_queries(string* queries_file, string* output_file);

#endif