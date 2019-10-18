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
		int distance;		// δεν ειμαι σιγουρη οτι ειναι int, μπορει να ειναι και double
	public:
		inline NN(string id, int distance){ this->id = id; this->distance = distance; }
		inline string get_id(void){ return this->id; } 
		inline int get_distance(void){ return this->distance; }
};

bool check_arguments_lsh(int argc, char* argv[], string* input_file, string* queries_file, int* k, int* L, string* output_file);
bool check_arguments_cube(int argc, char* argv[], string* input_file, string* queries_file, unsigned int* k, unsigned int* M, unsigned int* probes, string* output_file);
bool read(string file_name, vector<Point*>* points);
bool point_proccessing(vector<Point*>* points, string point, int d = -1);
void update_output_lsh(string* output,string query_id, NN* lsh_nearest_neighbor, NN* true_nearest_neighbor, int duration_lsh, int duration_brute_force);
void update_output_cube(string* output, string query_id, NN* hypercube_nearest_neighbor, NN* true_nearest_neighbor, int duration_hypercube, int duration_brute_force);
bool write_output(string file_name, string output);

template <class C>
void delete_vector(vector<C*>* v){

	for (int i = 0; i < (*v).size(); ++i)
		delete (*v)[i];

	(*v).clear();
}

int manhattan_dist(Point* x, Point* y);
NN* brute_force(Point* point, vector<Point*>* pointset);

int average_distance(vector<Point*>* pointset);
int modulo(int a, int b);
int modPow(int b, int e, int m);

void print_points(vector<Point*> points);
void print_hash_tables(vector<unordered_map<unsigned int, vector<Point*>>>* hash_tables);

#endif