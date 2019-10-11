#ifndef DATAHANDLING_HPP
#define DATAHANDLING_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

#include "../Curve.hpp"

using namespace std;

vector <Curve *> file_handling(int argc, char * argv[], string * queries, string * output, int * K, int * M, int * probes, double * e);

vector <Curve*> struct_initialization(string file);

#endif