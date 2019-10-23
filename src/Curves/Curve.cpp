#include "Curve.hpp"

using namespace std;

Curve::Curve(string id){
	this->curve_id = id;
}

void Curve::add_point(double x, double y){

	this->points.push_back(make_pair(x, y));
}

int Curve::get_length(void){

	return this->points.size();
}

pair<double, double> Curve::operator[](int i){

	return this->points[i];
}


void Curve::clear(void){

	this->points.clear();
}