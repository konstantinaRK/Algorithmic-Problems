#ifndef HYPERCUBE_HPP
#define HYPERCUBE_HPP

#include "./hypercube.hpp"

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

NN * hypercube_calc(Point * point, F * f_g, unordered_map<int, vector<Point*>>* hypercube, unsigned int M, unsigned int probes, vector<unsigned int>* neighbors)
{
	NN *nearest_neighbor = NULL, *temp_neighbor = NULL;

	unsigned int key = f_g->calc_F(point);
	nearest_neighbor = find_vertice_NN(point, f_g, hypercube, &M, key);
	probes --;
	if (M > 0 && probes > 0)
	{
		int i = 0;
		while(i < neighbors->size() && M > 0 && probes > 0)
		{
			unsigned int temp_key = key ^ neighbors->at(i);	

			temp_neighbor = find_vertice_NN(point, f_g, hypercube, &M, temp_key);
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

	return nearest_neighbor;
}

// void F::print(void){

// 	for (int i = 0; i < this->f_g.size(); i++)
// 	{
// 		for (auto it = this->f_g[i].begin(); it != this->f_g[i].end(); ++it)
// 		{ 
// 			cout << it->first << " = " << it->second << '\n';
// 		}
// 		cout << "\n\n" << endl;
// 	}
// }

// Private functions
unsigned int calc_maxdistance(unsigned int k, unsigned int probes)
{
	unsigned int div = 1;
	unsigned int mult = k;
	unsigned int sum = k;	// Number of probes checked
	double limit = (double)k;
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

void create_vector(vector <unsigned int > * neighbors, unsigned int k, unsigned int distance)
{
	unsigned int limit = 0x1 << k;
	unsigned int start_mask = 1;
	for (int i = 1; i <= distance; i++)
	{
		unsigned int mask = start_mask;

		while (mask < limit)
		{
			neighbors->push_back(mask);
			unsigned int t = (mask | (mask - 1)) + 1;
			mask = t | ((((t & -t) / (mask & -mask)) >> 1) - 1);
		}

		start_mask = (start_mask << 1) | 1;
	}
}

NN * find_vertice_NN(Point * point, F * f_g, unordered_map<int, vector<Point*>>* hypercube, unsigned int * M, unsigned int key){

	int min_distance;
	string min_id = "";

	if ( hypercube->find(key) != hypercube->end() )
	{
		vector<Point*> buckets_points = hypercube->at(key);
		Point* p;

		for (int i = 0; (*M) > 0 && i < buckets_points.size(); (*M)--, i++)
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

	NN * nearest_neighbor = NULL;

	try
	{
		nearest_neighbor = new NN(min_id, min_distance);
	}
	catch(bad_alloc&)
	{
		cerr << "hypercube::No memory available" << endl;
		exit(1);
	}

	return nearest_neighbor;
}

#endif