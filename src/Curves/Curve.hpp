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
		inline string get_id(void){ return this->curve_id; }
		void add_point(double x, double y);
		int get_length(void);
		pair<double, double> operator[](int i);
		void clear(void);
	
};

#endif