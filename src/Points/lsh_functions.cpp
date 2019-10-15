#include "./lsh_functions.hpp"

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
	for (int i = 0; i < d; i++)
	{
// cout << this->m << " " << d << " " << this->M << endl;
		a = modulo((floor(((*point)[i] - this->s[i])/(this->w/1.0)))*(modPow(this->m, d-i-1, this->M)), this->M);
// if (a!= 0) cout << a << endl;
		sum += a;
	}
// cout << modulo(sum, this->M) << endl;
	return modulo(sum, this->M);
}

G::G(int k,int dimension, int m, int w){

	//Create h
	for (int i = 0; i < k; ++i)
	{
		try{
			this->h.push_back(new H(dimension, m, w, pow(2, floor(32/k))));
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

unsigned int G::operator[](Point* point){

	unsigned int g = 0;
	int k = this->h.size();
	int shift = floor(32/k);
	for (int i = 0; i < k; ++i)
	{
		g = g << shift;
		g = g | (*(this->h[i]))[point];
// cout << "h is " << (*(this->h[i]))[point] << endl;
	}
// cout << "g is " << g << endl;
	return g;
}

NN* lsh(Point* point,vector<G*>* g, vector<unordered_map<unsigned int, vector<Point*>>>* hash_tables, int w){

	int min_distance;
	string min_id = "";

	int L = (*g).size();
	int break_point;
	for (int i = 0; i < L; ++i)
	{
		unsigned int key = (*(*g)[i])[point];
		// If the key exists
		if ( (*hash_tables)[i].find(key) != (*hash_tables)[i].end() )
		{
			vector<Point*> buckets_points = (*hash_tables)[i].at(key);
			Point* p;

			break_point = buckets_points.size()/2;
			for (int j = 0; j < buckets_points.size(); ++j)
			{

				// if ( j >  break_point && j > 50 )
				// 	break;
				
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

	// If i have found a nearest neighbor
	if ( min_id.compare("") !=0 )
	{
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
	else
		return NULL;
}
