#ifndef LSH_FUNCTIONS_HPP
#define LSH_FUNCTIONS_HPP

#include <random>
#include <unordered_map> 
#include "../utilities.hpp"

class H{
		vector<double> s;
		int m;
		double w;
		int M;

		int modulo(int a, int b);
		int modPow(int b, int e, int m);
	public:
		H(int dimension, int m, double w, int M);
		inline ~H() { this->s.clear(); }
		int operator[](Point* point);
};

class G{
		vector<H*> h;
	public:
		G(int k, int dimension, int m, double w);
		~G();
		unsigned int operator[](Point* point);
};

class LSH{
		vector<Point*> pointset;
		vector<G*> g;
		vector<unordered_map<unsigned int, vector<Point*>>> hash_tables;

		void train();
	public:
		LSH(vector<Point*>* points, int L, int k, int dimension);
		~LSH();
		NN* predict(Point * point, int r = 0);
		void print_points(){
			for (int i = 0; i < this->pointset.size(); ++i)
			{
				for (int j = 0; j < pointset[i]->get_dimension(); ++j)
				{
					cout << (*pointset[i])[j] << " ";
				}
				cout << endl << endl;
			}
		}
};

#endif