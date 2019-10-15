#ifndef LSH_FUNCTIONS_HPP
#define LSH_FUNCTIONS_HPP

#include <random>
#include <unordered_map> 
#include "./utilities.hpp"

class H{
		vector<double> s;
		int m;
		int w;
		int M;
	public:
		H(int dimension, int m, int w, int M);
		inline ~H() { this->s.clear(); }
		int operator[](Point* point);
};

class G{
		vector<H*> h;
	public:
		G(int k, int dimension, int m, int w);
		~G();
		unsigned int operator[](Point* point);
};

NN* lsh(Point* point,vector<G*>* g, vector<unordered_map<unsigned int, vector<Point*>>>* hash_tables, int w);

#endif