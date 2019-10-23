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
		inline string get_id(void){ return this->id; } 
		inline double get_distance(void){ return this->distance; }
		unsigned int r_near_neighbors_size();
		string get_near_neighbor(int i);
};

template <class C>
void delete_vector(vector<C*>* v){

	for (int i = 0; i < (*v).size(); ++i)
		delete (*v)[i];

	(*v).clear();
}

double manhattan_dist(Point* x, Point* y);
NN* brute_force(Point* point, vector<Point*>* pointset);
double average_distance(vector<Point*>* pointset);


void print_points(vector<Point*> points);
void print_hash_tables(vector<unordered_map<unsigned int, vector<Point*>>>* hash_tables);

#endif