#ifndef DATAHANDLING_HPP
#define DATAHANDLING_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <utility> // for pair

using namespace std;

class Curve
{
	private:
		vector <pair <double, double>> points;
	public:
		// Curve();
		// ~Curve();
		void add_point(double x, double y);
	
};

int file_handling(int argc, char * argv[], string * input, string * queries, string * output);

vector <Curve*> struct_initialization(string file);

#endif