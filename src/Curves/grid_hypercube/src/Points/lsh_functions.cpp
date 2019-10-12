#include "./lsh_functions.h"

using namespace std;

H::H(int dimension, int m, int w, int M){

	// Create s
	default_random_engine generator;
	uniform_real_distribution<double> distribution(0.0,w/1.0);

	for (int i = 0; i < dimension; ++i)
	{
		this->s.push_back(distribution(generator));
	}

	this->m = m;
	this->w = w;
	this->M = M;
}

int H::operator[](Point* point){

	int sum = 0;
	
	int a;
	int d = point->get_dimension();
	for (int i = 0; i < d; ++i)
	{
		a = modulo((floor(((*point)[i] - this->s[i])/this->w))*(modulo(pow(this->m,d-i-1), this->M)), this->M);
		sum += a;
	}
	return sum;
}

G::G(int k,int dimension, int m, int w){

	//Create h
	for (int i = 0; i < k; ++i)
	{
		try{
			this->h.push_back(new H(dimension, m, w, floor(32/k)));
		}
		catch(std::bad_alloc&){

			for (int j = 0; j < i; ++j)
			{
				delete this->h[j];
			}
			throw;
		}
	}
}

G::~G(){

	for (int i = 0; i < this->h.size(); ++i)
	{
		delete this->h[i];
	}

	this->h.clear();
}

int G::operator[](Point* point){

	int g = 0;
	int k = this->h.size();
	for (int i = 0; i < k; ++i)
	{
		g << k;
		g = g | (*(this->h[i]))[point];	
	}

	return g;
}

NN* lsh(Point* point,vector<G*>* g, vector<unordered_map<int, vector<Point*>>>* hash_tables, int w){

	int min_distance;
	string min_id = "";

	int L = (*g).size();
	for (int i = 0; i < L; ++i)
	{
		int key = (*(*g)[i])[point];
		// If the key exists
		if ( (*hash_tables)[i].find(key) != (*hash_tables)[i].end() )
		{
			vector<Point*> buckets_points = (*hash_tables)[i].at(key);
			Point* p;

			for (int j = 0; j < buckets_points.size(); ++j)
			{

				if ( j > 3*L )
					break;
				
				p = buckets_points[j];
				// If this is the first point
				if ( min_id == "" )
				{
					min_id = p->get_id();
					min_distance = manhattan_dist(p, point);
				}
				else
				{
					int distance = manhattan_dist(p, point);
					if ( distance < min_distance )
					{
						min_distance = distance;
						min_id = p->get_id();
					}
				}
			}
		}
	}

	NN * nearest_neighbor;
	try{
		nearest_neighbor = new NN(min_id, min_distance);
	}
	catch(bad_alloc&)
	{
		cerr << "No memory available" << endl;
		nearest_neighbor = NULL;
	}

	return nearest_neighbor;
}
