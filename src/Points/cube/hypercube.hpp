#include <map>
#include <unordered_map>
#include "../lsh_functions.h"

using namespace std;

class F {
	private:
		vector<G *> g;
		vector <map <int, int>> f_g;
	public:
		F(int dim, int m , int w, int k = 4);
		~F();
		unsigned int calc_F(Point * point);
};

NN * hypercube_calc(Point * point, F * f_g, unordered_map<int, vector<Point*>>* hypercube, int probes);