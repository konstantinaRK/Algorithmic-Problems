#ifndef DATAHANDLING_HPP
#define DATAHANDLING_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

#include "../../Curve.hpp"

using namespace std;

vector <Curve *> file_handling(int argc, char * argv[], string * queries, string * output, unsigned int * K, unsigned int * M_hyper, unsigned int * probes, double * e, unsigned int * M);

#endif