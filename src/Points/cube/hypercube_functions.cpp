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
/*	Calculate F using f(g) values, according to uniform distribution
	Store new values in order to have same results if a g reappears	*/
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

unsigned int F::getK(void)
{
	return this->g.size();
}

Hypercube::Hypercube(vector<Point*>* points, int dimension, unsigned int k_hypercube, unsigned int M, unsigned int probes)
{
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

	// Fill the cube with data
	this->train();

	// Find <probes> nearest cube tops
	unsigned int max_distance = this->calc_maxdistance(F_g->getK(), probes);
	this->create_cube_neighbors(F_g->getK(), max_distance);
}

Hypercube::~Hypercube()
{
	this->cube_neighbors.clear();
	delete_vector<Point>(&(this->pointset));
	delete this->F_g;
	this->cube.clear();
}

void Hypercube::train()
/*	Fill the cube with data */
{
	// For each Point find the key and add it to the proper bucket/vertice
	unsigned int key;
	for (int i = 0; i < (this->pointset).size(); i++)
	{
		key = (this->F_g)->calc_F(this->pointset[i]);
		// If key doesn't already exist
		if ( this->cube.find(key) == this->cube.end() )
		{
			// Insert the key
			vector<Point*> p;
			p.push_back(this->pointset[i]);
			this->cube.insert(make_pair(key, p));
			p.clear();
		}
		else	// If key already exists
		{
			this->cube.at(key).push_back(this->pointset[i]);
		}
	}
}

unsigned int Hypercube::calc_maxdistance(unsigned int k_hypercube, unsigned int probes)
/* Calculates the maximum hamming distance we have to check in order to cover all the probes, depending of hypercube keys bits	*/
{
	unsigned int div = 1;
	unsigned int mult = k_hypercube;
	unsigned int sum = k_hypercube;	// Number of probes checked
	double limit = (double)k_hypercube;
	unsigned int distance = 1;	// Hamming distance

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

void Hypercube::create_cube_neighbors(unsigned int k_hypercube, unsigned int distance)
/*	Create the masks that produce all the neighbors with hamming distance in range [1,distance]	*/
{
	// The limit is used to stop the production of mask permutations in 
	unsigned int limit = 0x1 << k_hypercube;
	unsigned int start_mask = 1;	// Each loop has a starting number of 1 bits equal to the current distance checked
	for (int i = 1; i <= distance; i++)
	{
		// Mystery code aquired from stackoverflow! It works!!!
		unsigned int mask = start_mask;
		while (mask < limit)
		{
			(this->cube_neighbors).push_back(mask);
			unsigned int t = (mask | (mask - 1)) + 1;
			mask = t | ((((t & -t) / (mask & -mask)) >> 1) - 1);
		}

		// Get ready for next loop
		start_mask = (start_mask << 1) | 1;
	}
}

NN * Hypercube::find_vertice_NN(Point * point,unsigned int* cur_M, unsigned int key)
/* Find the nearest neighbor in this find_vertice_NN	*/
{
	int min_distance;
	string min_id = "";

	// Check if value exists in map
	if ( (this->cube).find(key) != (this->cube).end() )
	{
		// Check all points to find the one with min distance
		vector<Point*> buckets_points = (this->cube).at(key);
		Point* p;
		for (int i = 0; (*cur_M) > 0 && i < buckets_points.size(); (*cur_M)--, i++)
		{				
			p = buckets_points[i];
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
	else
	{
		return NULL;
	}

	// Create the nearest neighbor struct
	NN * nearest_neighbor = NULL;
	try
	{
		nearest_neighbor = new NN(min_id, min_distance);
	}
	catch(bad_alloc&)
	{
		cerr << "hypercube::No memory available" << endl;
		return NULL;
	}

	// Return nearest neighbor
	return nearest_neighbor;
}

NN* Hypercube::predict(Point* point)
/*	Depending on probes check all vertices to find the nearest neighbor */
{
	NN *nearest_neighbor = NULL, *temp_neighbor = NULL;
	unsigned int M = this->M;
	unsigned int probes = this->probes;

	// Create query key and search the nearest neighbor
	unsigned int key = (this->F_g)->calc_F(point);
	nearest_neighbor = this->find_vertice_NN(point, &M, key);
	probes --;

	// Check if near vertices have a closer neighbor
	if (M > 0 && probes > 0)
	{
		// Create the vertice
		int i = 0;
		while(i < (this->cube_neighbors).size() && M > 0 && probes > 0)
		{
			unsigned int temp_key = key ^ (this->cube_neighbors).at(i);	

			// Find the nearest neighbor in current vertice
			temp_neighbor = this->find_vertice_NN(point, &M, temp_key);

			// Check if the neighbor is closer than the previous one
			if (nearest_neighbor == NULL)
			{
				nearest_neighbor = temp_neighbor;
			}
			else if ((temp_neighbor != NULL) && (nearest_neighbor->get_distance() > temp_neighbor->get_distance()))
			{
				delete nearest_neighbor;
				nearest_neighbor = temp_neighbor;
			}
			else if(temp_neighbor != NULL)
			{
				delete temp_neighbor;
			}
			i++;
			probes--;
		}
	}

	// Return nearest neighbor
	return nearest_neighbor;

}