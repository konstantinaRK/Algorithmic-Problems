#include <random>
#include <unordered_map> 
#include "utilities.h"

class H{
		vector<int> s;
		int m;
		int w;
		int M;
	public:
		H(int dimension, int m, int w, int M);
		int operator[](Point* point);
		// inline int get_m(void) { return this->m; }
		// inline int get_w(void) { return this->w; }
		// inline int get_M(void) { return this->M; }
};

class G{
		vector<H*> h;
	public:
		G(int k, int dimension, int m, int w);
		~G();
		int operator[](Point* point);
};

NN* lsh(Point* point,vector<G*>* g, vector<unordered_map<int, vector<Point*>>>* hash_tables, int w);