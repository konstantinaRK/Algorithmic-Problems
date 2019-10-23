#ifndef DATAHANDLING_HPP
#define DATAHANDLING_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#include "../Curve.hpp"

using namespace std;

vector <Curve *> file_handling(int argc, char * argv[], string * queries, string * output, unsigned int * K, unsigned int * M, unsigned int * probes, unsigned int * L, int* max_d, int* min_d);

vector <Curve*> struct_initialization(string file, int* max_d = NULL, int* min_d = NULL);

#endif