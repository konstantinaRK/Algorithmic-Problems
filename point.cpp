#include "point.h"

using namespace std;

Point::Point(string id, vector<int> X){

	this->id = id;
	for (int i=0; i<X.size(); i++){
		this->X.push_back(X[i]);
	}
}

int Point::operator[](int i){
	 if ( i>=0 && i<this->X.size() )
	 	return this->X[i];
	 else
	 	return -1; 
}