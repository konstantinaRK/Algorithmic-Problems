#include "Curve.hpp"

using namespace std;

Curve::Curve(string id){
	this->curve_id = id;
}

void Curve::add_point(double x, double y){

	this->points.push_back(make_pair(x, y));
}

void Curve::clear(void){

	this->points.clear();
}

int Curve::get_size(void){

	return this->points.size();
}