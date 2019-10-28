#ifndef DATAHANDLING_HPP
#define DATAHANDLING_HPP

#include "./utilities.hpp"

using namespace std;

/*	Point functions	*/
// Point lsh check arguments
bool check_arguments_lsh(int argc, char* argv[], string* input_file, string* queries_file, int* k, int* L, string* output_file);

// Point hypercube check arguments
bool check_arguments_cube(int argc, char* argv[], string* input_file, string* queries_file, unsigned int* k, unsigned int* M, unsigned int* probes, string* output_file);

// Points read
bool read(string file_name, vector<Point*>* points, int* r = NULL); 

bool find_range(string line, int* r);
bool point_proccessing(vector<Point*>* points, string point, int d = -1);

// Output functions
void update_output_lsh(string* output,string query_id, NN* lsh_nearest_neighbor, NN* true_nearest_neighbor, int duration_lsh, int duration_brute_force);
void update_output_cube(string* output, string query_id, NN* hypercube_nearest_neighbor, NN* true_nearest_neighbor, int duration_hypercube, int duration_brute_force);

/*	Curve functions	*/
vector <Curve*> file_handling_grid_lsh(int argc, char * argv[], string * queries, string * output, int * K, int * L, int * max_d, int * min_d);
vector <Curve *> file_handling_grid_hypercube(int argc, char * argv[], string * queries, string * output, unsigned int * K, unsigned int * M, unsigned int * probes, unsigned int * L, int* max_d, int* min_d);
vector <Curve *> file_handling_projection_lsh(int argc, char * argv[], string * queries, string * output, unsigned int * K, unsigned int * L, double * e, unsigned int *M);
vector <Curve *> file_handling_projection_hypercube(int argc, char * argv[], string * queries, string * output, unsigned int * K, unsigned int * M_hyper, unsigned int * probes, double * e, unsigned int * M);

vector <Curve*> struct_initialization_grid(string file, int* max_d = NULL, int* min_d = NULL);
vector <Curve*> struct_initialization_projection(string file);

void update_output_curves(string* output, string curve_id, NN* found_nearest_neighbor, NN* true_nearest_neighbor,/*int duration, int duration_brute_force,*/ string method, string hashfunction);

/*	General functions	*/
bool check_for_new_queries(string* queries_file, string* output_file);
bool write_output(string file_name, string output);


#endif

