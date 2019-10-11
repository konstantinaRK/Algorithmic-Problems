#include "Curve.hpp"

using namespace std;

Curve::Curve(int id){
	this->curve_id = id;
}

void Curve::add_point(double x, double y){

	this->points.push_back(make_pair(x, y));
}

void Curve::clear(void){

	this->points.clear();
}