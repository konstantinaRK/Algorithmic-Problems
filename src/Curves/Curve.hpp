#ifndef CURVE_HPP
#define CURVE_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <utility> // for pair

using namespace std;

class Curve
{
	private:
		string curve_id;
		vector <pair <double, double>> points;
	public:
		Curve(string id);
		// ~Curve();
		void add_point(double x, double y);
		void clear(void);
		int get_size(void);
	
};

#endif