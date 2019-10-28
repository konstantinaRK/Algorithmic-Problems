#ifndef POINTS_DATAHANDLING_HPP
#define POINTS_DATAHANDLING_HPP

#include "./utilities.hpp"

using namespace std;

bool check_arguments_lsh(int argc, char* argv[], string* input_file, string* queries_file, int* k, int* L, string* output_file);
bool check_arguments_cube(int argc, char* argv[], string* input_file, string* queries_file, unsigned int* k, unsigned int* M, unsigned int* probes, string* output_file);

bool read(string file_name, vector<Point*>* points, int* r = NULL);
bool find_range(string line, int* r);
bool point_proccessing(vector<Point*>* points, string point, int d = -1);

void update_output_lsh(string* output,string query_id, NN* lsh_nearest_neighbor, NN* true_nearest_neighbor, int duration_lsh, int duration_brute_force);
void update_output_cube(string* output, string query_id, NN* hypercube_nearest_neighbor, NN* true_nearest_neighbor, int duration_hypercube, int duration_brute_force);
bool write_output(string file_name, string output);

#endif