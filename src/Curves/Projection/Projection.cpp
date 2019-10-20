#include "Projection.hpp"

#include <chrono>
#include <random>

Traversal::Traversal(Curve * Curve1, Curve * Curve2){
	this->curve1 = Curve1;
	this->curve2 = Curve2;
}

Traversal::~Traversal(){
	this->traversal.clear();
}

IJ_Traversals::~IJ_Traversals(){
	for (int i = 0; i < this->traversals.size(); i++)
	{
		delete this->traversals.at(i);
	}
	this->traversals.clear();	
}

void IJ_Traversals::insertTraversals(Curve * curve1, Curve * curve2, double **G){

	// Create Traversal

	// For each Traversal
	try {
		this->traversals.push_back(new Traversal(curve1, curve2 /* traversal vector*/));
	}
	catch (std::bad_alloc& ba)
	{
		exit(1);
	}
}

double ** createG(int d, int k){

	double ** G;

	int pos = 0;
	try {
		G = new  double *[k];
		for (pos = 0; pos < k; pos++)
		{
			G[pos] = new double[d];
		}
	}
	catch(std::bad_alloc& ba){
		for (int i = 0; i < pos; i++)
		{
			delete [] G[i];
		}
		delete [] G;
		return NULL;
	}

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator (seed);
	normal_distribution<double> distribution(0.0,1.0);

	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < d; j++)
		{
			G[i][j] = distribution(generator);
		}
	}

	return G;
}

IJ_Traversals ** train(vector <Curve*> data, unsigned int M, double **G){

	IJ_Traversals ** MM_table;
	int pos = 0;
	try {
		MM_table = new IJ_Traversals * [M];
		for (pos = 0; pos < M; pos++)
		{
			MM_table[pos] = new IJ_Traversals [M];
		}
	}
	catch (std::bad_alloc& ba) {
		for (int i = 0; i < pos; i++)
		{
			delete [] MM_table[i];
		}
		delete [] MM_table;
		return NULL;
	}

	for (int i = 0; i < data.size(); i++)
	{
		int sizeI = data.at(i)->get_size();
		for (int j = 0; j < data.size(); j++)
		{
			int sizeJ = data.at(j)->get_size();			
			MM_table[sizeI-1][sizeJ-1].insertTraversals(data.at(i), data.at(j), G);
			// MM_table[sizeJ][sizeI].insertTraversals(data.at(j), data.at(i));
		}
	}

	return MM_table;
}