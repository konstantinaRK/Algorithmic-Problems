#ifndef HYPERCUBE_HPP
#define HYPERCUBE_HPP

#include "./hypercube_functions.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
NN * find_vertice_NN(Point * point, F * f_g, unordered_map<int, vector<Point*>>* hypercube, unsigned int *M, unsigned int key);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

F::F(unsigned int dim, unsigned int m , unsigned int w, unsigned int k)
{
	// Create k (=d') g functions
	for (int i = 0; i < k; ++i)
	{
		try{
			this->g.push_back(new G(4, dim, m, w));
			this->f_g.push_back(unordered_map <unsigned int, int>());
		}
		catch(bad_alloc&){
			delete_vector<G>(&(this->g));
			for (int i = 0; i < this->f_g.size(); ++i)
			{
				this->f_g[i].clear();
			}
			this->f_g.clear();
			cerr << "F creation::No memory available" << endl;
			exit(1);
		}
	}
}

F::~F()
{
	delete_vector<G>(&(this->g));
	(this->f_g).clear();

}

unsigned int F::calc_F(Point * point)
{
	// Production of 0 and 1 with equal probability
	random_device rd;  //Will be used to obtain a seed for the random number engine
	mt19937 generator(rd()); //Standard mersenne_twister_engine seeded with rd()
	uniform_int_distribution<> distribution(0, 1);

	// f
	unsigned int key = 0;
	for (int i = 0; i < (this->g).size(); i++)
	{
		// fi calculation
		unsigned int result = (*(this->g[i]))[point];

		// If it's the first time we find the value
		if ((this->f_g)[i].find(result) == (this->f_g)[i].end())
		{
			int new_value = distribution(generator);

			(this->f_g)[i].insert({result, new_value});
			key = key << 1;
			key = key | new_value;
		}
		// If it already has been calculated
		else
		{
			key = key << 1;
			key = key | (this->f_g)[i].find(result)->second;
		}
	}

	return key;
}

unsigned int F::getK(void){
	return this->g.size();
}

Hypercube::Hypercube(vector<Point*>* points, int dimension, unsigned int k_hypercube, unsigned int M, unsigned int probes){

	this->M = M;
	this->probes = probes;

	// Create pointset
	for (int i = 0; i < (*points).size(); ++i)
	{
		this->pointset.push_back((*points)[i]);
	}
	//(*points).clear();

	double w = 4*average_distance(&(this->pointset)) + 4;
	int m = 3;

	// Create the F
	F_g = new F(dimension, m, w, k_hypercube); 

	this->train();

	// Find <probes> nearest cube tops
	unsigned int max_distance = this->calc_maxdistance(F_g->getK(), probes);
	this->create_cube_neighbors(F_g->getK(), max_distance);
}

Hypercube::~Hypercube(){

	delete_vector<Point>(&(this->pointset));
	delete this->F_g;
}

void Hypercube::train(){

	// Fill the hash table with the pointset
	unsigned int key;
	for (int i = 0; i < (this->pointset).size(); i++)
	{
			key = (this->F_g)->calc_F(this->pointset[i]);
			if ( this->cube.find(key) == this->cube.end() )
			{
				// Insert the key
				vector<Point*> p;
				p.push_back(this->pointset[i]);
				this->cube.insert(make_pair(key, p));
			}
			else
			{
				this->cube.at(key).push_back(this->pointset[i]);
			}
	}
}

unsigned int Hypercube::calc_maxdistance(unsigned int k_hypercube, unsigned int probes){

	unsigned int div = 1;
	unsigned int mult = k_hypercube;
	unsigned int sum = k_hypercube;	// Number of probes checked
	double limit = (double)k_hypercube;
	unsigned int distance = 1;	// Humming distance

	// k!/(k-n)!n!
	while (probes > sum && mult > 0){
		div ++;
		mult --;
		distance ++;
		limit = (double) limit * mult / div;
		sum += limit;
	}
	return distance;
}

void Hypercube::create_cube_neighbors(unsigned int k_hypercube, unsigned int distance){

	unsigned int limit = 0x1 << k_hypercube;
	unsigned int start_mask = 1;
	for (int i = 1; i <= distance; i++)
	{
		unsigned int mask = start_mask;

		while (mask < limit)
		{
			(this->cube_neighbors).push_back(mask);
			unsigned int t = (mask | (mask - 1)) + 1;
			mask = t | ((((t & -t) / (mask & -mask)) >> 1) - 1);
		}

		start_mask = (start_mask << 1) | 1;
	}
}

NN * Hypercube::find_vertice_NN(Point * point,unsigned int* cur_M, unsigned int key, int r){

	int min_distance, distance;
	string min_id = "";
	set<string> near_neighbors;

	if ( (this->cube).find(key) != (this->cube).end() )
	{
		vector<Point*> buckets_points = (this->cube).at(key);
		Point* p;

		for (int i = 0; (*cur_M) > 0 && i < buckets_points.size(); (*cur_M)--, i++)
		{				
			p = buckets_points[i];
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
	else
	{
		return NULL;
	}

	NN * nearest_neighbor = NULL;
	if ( min_id.compare("") != 0 ){

		near_neighbors.erase(min_id);
		try
		{
			nearest_neighbor = new NN(min_id, min_distance, &near_neighbors);
		}
		catch(bad_alloc&)
		{
			cerr << "hypercube::No memory available" << endl;
			return NULL;
		}
	}
	return nearest_neighbor;
}

NN* Hypercube::predict(Point* point, int r){

	NN *nearest_neighbor = NULL, *temp_neighbor = NULL;

	unsigned int M = this->M;
	unsigned int probes = this->probes;
	unsigned int key = (this->F_g)->calc_F(point);
	nearest_neighbor = this->find_vertice_NN(point, &M, key, r);
	probes --;
	if (M > 0 && probes > 0)
	{
		int i = 0;
		while(i < (this->cube_neighbors).size() && M > 0 && probes > 0)
		{
			unsigned int temp_key = key ^ (this->cube_neighbors).at(i);	

			temp_neighbor = this->find_vertice_NN(point, &M, temp_key, r);
			if (nearest_neighbor == NULL)
			{
				nearest_neighbor = temp_neighbor;
			}
			else if ((temp_neighbor != NULL) && (nearest_neighbor->get_distance() > temp_neighbor->get_distance()))
			{
				vector<string> near_neighbors = nearest_neighbor->get_near_neighbors();
				delete nearest_neighbor;
				nearest_neighbor = temp_neighbor;
				nearest_neighbor->add_neighbors(&near_neighbors);
			}
			else if(temp_neighbor != NULL)
			{
				delete temp_neighbor;
			}
			i++;
			probes--;
		}
	}

	return nearest_neighbor;

}

#endif