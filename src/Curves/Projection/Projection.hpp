#ifndef PROJECTION_HPP
#define PROJECTION_HPP

#include "../Curve.hpp"

class Traversal
{
	private:
		Curve * curve1;
		Curve * curve2;
		vector <pair<int,int>> traversal;
	public:
		Traversal(Curve * Curve1, Curve * Curve2);
		~Traversal();
	
};

class IJ_Traversals
{
	private:
		vector <Traversal *> traversals;
	public:
		// IJ_Traversals(){};
		~IJ_Traversals();
		void insertTraversals(Curve * curve1, Curve * curve2, double ** G);
};

double ** createG(int d, int k);
IJ_Traversals ** train(vector <Curve*> data, unsigned int M, double **G);


#endif