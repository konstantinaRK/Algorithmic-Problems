#include <map>
#include <unordered_map>
#include "../lsh/lsh_functions.hpp"

using namespace std;

class F {
	private:
		vector<G *> g;
		vector <unordered_map <unsigned int, int>> f_g;
	public:
		F(unsigned int dim, unsigned int m , unsigned int w, unsigned int k = 4);
		~F();
		unsigned int calc_F(Point * point);
		unsigned int getK(void);
// void print(void);
};

class Hypercube{
		vector<Point*> pointset;
		F* F_g;
		unordered_map<unsigned int, vector<Point*>> cube;
		unsigned int M;
		unsigned int probes;
		vector <unsigned int> cube_neighbors;

		void train();
		unsigned int calc_maxdistance(unsigned int k_hypercube, unsigned int probes);
		void create_cube_neighbors(unsigned int k_hypercube, unsigned int distance);
		NN * find_vertice_NN(Point * point,unsigned int* cur_M, unsigned int key, int r);

	public:
		Hypercube(vector<Point*>* points, int dimension, unsigned int k_hypercube, unsigned int M, unsigned int probes);
		~Hypercube();
		NN* predict(Point* point, int r=0);
};

NN * hypercube_calc(Point * point, F * f_g, unordered_map<int, vector<Point*>>* hypercube, unsigned int M, unsigned int probes, vector<unsigned int>* neighbors);
unsigned int calc_maxdistance(unsigned int k, unsigned int probes);
void create_vector(vector <unsigned int > * neighbors, unsigned int k, unsigned int distance);