#include "./lsh_functions.hpp"

using namespace std;

H::H(int dimension, int m, double w, int M){

	// Create s

	random_device rd;  //Will be used to obtain a seed for the random number engine
	mt19937 generator(rd()); //Standard mersenne_twister_engine seeded with rd()
	uniform_real_distribution<> distribution(0.0,w/1.0);

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
	int a = 0;
	int d = point->get_dimension();
	for (int i = 0; i < d; i++)
	{
		a = modulo((floor(((*point)[i] - this->s[i])/(this->w)))*(modPow(this->m, d-i-1, this->M)), this->M);
		sum += a;
// cout << (floor(((*point)[i] - this->s[i])/(this->w))) << " ";
	}
	return modulo(sum, this->M);
}

int H::modulo(int a, int b) {
  int m = a % b;
  if (m < 0) {
    m = (b < 0) ? m - b : m + b;
  }
  return m;
}

int H::modPow(int b, int e, int m){

	int r = 1;
	for (int i = 0; i < e; ++i)
	{
		r = (r*b)%m;
	}
	return r;
}

G::G(int k,int dimension, int m, double w){

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


LSH::LSH(vector<Point*>* points, int L, int k, int dimension){

	// Create pointset
	for (int i = 0; i < (*points).size(); ++i)
	{
		this->pointset.push_back((*points)[i]);
	}
	// (*points).clear();

	double w = 4*average_distance(&(this->pointset)) + 7;
	int m = 3;

	// Create the g functions and the hash tables
	for (int i = 0; i < L; ++i)
	{
		// try{
		// 	this->g.push_back(new G(k, dimension, m, w));
		// }
		// catch(bad_alloc&){
		// 	delete_vector<G>(&(this->g));
		// 	delete_vector<Point>(&(this->pointset));
		// 	cerr << "LSH : No memory available" << endl;
		// 	return;
		// }
		this->g.push_back(new G(k, dimension, m, w));
		unordered_map<unsigned int, vector<Point*>> hash_table;
		this->hash_tables.push_back(hash_table);
	}

	// Fill the hashtables
	this->train();
}

LSH::~LSH(){

	delete_vector<Point>(&pointset);
	delete_vector<G>(&g);
}

void LSH::train(){

	int L = (this->g).size();
	// Fill the hash tables with the pointset
	unsigned int key = 0;
	for (int j = 0; j < this->pointset.size(); ++j)
	{
		for (int i = 0; i < L; ++i)
		{
			key = (*(this->g[i]))[(this->pointset)[j]];
			if ( this->hash_tables[i].find(key) == this->hash_tables[i].end() )
			{
				// Insert the key
				vector<Point*> p;
				p.push_back(this->pointset[j]);
				this->hash_tables[i].insert(make_pair(key, p));
			}
			else
				this->hash_tables[i].at(key).push_back(this->pointset[j]);
		}		
	}

}

NN* LSH::predict(Point* point, int r){

	int min_distance, distance;
	string min_id = "";
	set<string> near_neighbors;

	int L  = (this->g).size();

	for (int i = 0; i < L; ++i)
	{
		unsigned int key = (*(this->g[i]))[point];
		// If the key exists
		if ( (this->hash_tables)[i].find(key) != (this->hash_tables)[i].end() )
		{
			vector<Point*> buckets_points = (this->hash_tables)[i].at(key);
			Point* p;

			int size = buckets_points.size();
			int step = 1;
			if ( r != 0 )
				size/500 + 1;

			for (int j = 0; j < size; j+=step)
			{	
				p = buckets_points[j];
				distance = manhattan_dist(p, point);
				if ( distance <= r )
					near_neighbors.insert(p->get_id());

				// If this is the first point
				if ( min_id == "" )
				{
					min_id = p->get_id();
					min_distance = distance;
				}
				else
				{
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
		near_neighbors.erase(min_id);

		NN * nearest_neighbor;
		try{
			nearest_neighbor = new NN(min_id, min_distance, &near_neighbors);
		}
		catch(bad_alloc&)
		{
			cerr << "No memory available" << endl;
			return NULL;
		}

		return nearest_neighbor;
	}
	else
		return NULL;
}
