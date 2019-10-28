#ifndef PROJECTION_HPP
#define PROJECTION_HPP

#include <unordered_map>
#include <set>

#include "./LSH_Structure.hpp"
#include "./Hypercube_Structure.hpp"

class Traversal
{
	private:
		vector <pair<unsigned int, unsigned int>> traversal;
	public:
		Traversal(vector <pair<unsigned int, unsigned int>> traversal);
		virtual ~Traversal();
		vector <pair<unsigned int, unsigned int>> get_traversal(void);
		int get_traversal_size(void);
		void curve_snap(vector <pair<double, double>>* curve_points, double ** G, unsigned int Gd_dim, unsigned int Gk_dim, vector <double>* new_vector);
		virtual void train(unsigned int curve_size, vector <Curve*> dataset, int L, int k, double ** G, unsigned int Gd_dim, unsigned int Gk_dim) {};
		virtual void train(unsigned int curve_size, vector <Curve*> dataset, unsigned int M_hyper, unsigned int k, unsigned int probes, double ** G, unsigned int Gd_dim, unsigned int Gk_dim){};
		virtual NN * predict(Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim) { cout << " giati me gamas?" << endl;};
};

class Traversal_LSH: public Traversal
{
	private:
		LSH * lsh;
	public:
		Traversal_LSH(vector <pair<unsigned int, unsigned int>> traversal);
		virtual ~Traversal_LSH();
		virtual void train(unsigned int curve_size, vector <Curve*> dataset, int L, int k, double ** G, unsigned int Gd_dim, unsigned int Gk_dim);
		virtual NN * predict(Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim);
};

class Traversal_Hypercube: public Traversal
{
	private:
		Hypercube * cube;
	public:
		Traversal_Hypercube(vector <pair<unsigned int, unsigned int>> traversal);
		~Traversal_Hypercube();
		virtual void train(unsigned int curve_size, vector <Curve*> dataset, unsigned int M_hyper, unsigned int k, unsigned int probes, double ** G, unsigned int Gd_dim, unsigned int Gk_dim);
		virtual NN * predict(Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim);
};


class IJ_Cell
{
	private:
		unordered_multimap<int , Traversal *> traversals;

		void calc_diagon(set <pair<unsigned int, unsigned int>> * diagonal, unsigned int i, unsigned int j, unsigned int dist);
		void calc_traversals(set <pair<unsigned int, unsigned int>> * diagonal, pair<unsigned int, unsigned int> start, pair<unsigned int, unsigned int> end, vector<pair<unsigned int, unsigned int>> traversal, string type);
	public:
		// Constructor for LSH
		IJ_Cell(string type, vector <Curve*> dataset, unsigned int L, unsigned int k, double ** G, unsigned int Gd_dim, unsigned int Gk_dim, unsigned int i, unsigned int j, unsigned int dist = 0);
		// Constructor for hypercube
		IJ_Cell(string type, vector <Curve*> dataset, unsigned int M_hyper, unsigned int k, unsigned int probes, double ** G, unsigned int Gd_dim, unsigned int Gk_dim, unsigned int i, unsigned int j, unsigned int dist = 0);
		~IJ_Cell();
		NN * predict(vector <Curve*> data, Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim);
};

double ** createG(int d, int k);
IJ_Cell *** train_LSH(vector <Curve*> data, unsigned int M, double **G, unsigned int Gd_dim, unsigned int Gk_dim, unsigned int L, unsigned int k);
IJ_Cell *** train_Hypercube(vector <Curve*> data, unsigned int M, double **G, unsigned int Gd_dim, unsigned int Gk_dim, unsigned int M_hyper, unsigned int k, unsigned int probes);
NN * predict(IJ_Cell *** MM_table, unsigned int M, vector <Curve*> data, Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim);


// for utilities
bool matrix_mult(pair <double, double> pair, double ** G, unsigned int Gd_dim, unsigned int Gk_dim, vector <double> * mult_res);

#endif