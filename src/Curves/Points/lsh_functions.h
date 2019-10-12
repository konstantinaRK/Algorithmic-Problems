#include <random>
#include <unordered_map> 
#include "./utilities.h"

class H{
		vector<int> s;
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
		int operator[](Point* point);
};

NN* lsh(Point* point,vector<G*>* g, vector<unordered_map<int, vector<Point*>>>* hash_tables, int w);