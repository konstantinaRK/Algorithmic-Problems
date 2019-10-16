#include <map>
#include <unordered_map>
#include "../lsh_functions.hpp"

using namespace std;

class F {
	private:
		vector<G *> g;
		vector <map <unsigned int, int>> f_g;
	public:
		F(int dim, int m , int w, int k = 4);
		~F();
		unsigned int calc_F(Point * point);
// void print(void);
};

NN * hypercube_calc(Point * point, F * f_g, unordered_map<int, vector<Point*>>* hypercube, int M, int probes);