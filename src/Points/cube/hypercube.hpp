#include <map>
#include <unordered_map>
#include "../lsh_functions.hpp"

using namespace std;

class F {
	private:
		vector<G *> g;
		vector <map <unsigned int, int>> f_g;
	public:
		F(unsigned int dim, unsigned int m , unsigned int w, unsigned int k = 4);
		~F();
		unsigned int calc_F(Point * point);
		unsigned int getK(void);
// void print(void);
};

NN * hypercube_calc(Point * point, F * f_g, unordered_map<int, vector<Point*>>* hypercube, unsigned int M, unsigned int probes, vector<unsigned int>* neighbors);
unsigned int calc_maxdistance(unsigned int k, unsigned int probes);
void create_vector(vector <unsigned int > * neighbors, unsigned int k, unsigned int distance);