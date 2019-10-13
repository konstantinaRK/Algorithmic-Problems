#ifndef HYPERCUBE_HPP
#define HYPERCUBE_HPP

#include "hypercube.hpp"

F::F(int dim, int m , int w, int k)
{
	// Create k (=d') g functions
	for (int i = 0; i < k; ++i)
	{
		try{
			this->g.push_back(new G(k, dim, m, w));
			this->f_g.push_back(map <int, int>());
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
	default_random_engine generator;
	uniform_int_distribution<int> distribution(0,1);

	// f
	unsigned int key = 0;
// cout << "allalala" << endl;
	for (int i = 0; i < (this->g).size(); i++)
	{
		// fi calculation
		int result = (*(this->g[i]))[point];
// cout << result << endl;
		// If it's the first time we find the value
		if ((this->f_g)[i].find(result) == (this->f_g)[i].end())
		{
			int new_value = distribution(generator);
// cout << "dist " << new_value << endl;
			(this->f_g)[i].insert({result, new_value});
			key = key << 1;
			key = key | new_value;
// cout << "key 1 " << key << endl;
		}
		// If it already has been calculated
		else
		{
			key = key << 1;
			key = key | (this->f_g)[i].find(result)->second;
// cout << "key 2 " << key << endl;
		}
	}
// cout << key << endl;
	return key;
}

NN * hypercube_calc(Point * point, F * f_g, unordered_map<int, vector<Point*>>* hypercube, int probes)
{
	int min_distance;
	string min_id = "";

	int key = f_g->calc_F(point);
	if ( hypercube->find(key) != hypercube->end() )
	{
		vector<Point*> buckets_points = hypercube->at(key);
		Point* p;

		for (int i = 0; i < buckets_points.size(); i++)
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
// cout << "id = " << p->get_id() << " distance = " << distance << endl;
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