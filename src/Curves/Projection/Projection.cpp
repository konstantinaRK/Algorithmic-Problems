#include "Projection.hpp"

#include <chrono>
#include <random>
#include <cstdlib>

// Class Traversal functions
Traversal::Traversal(vector <pair<unsigned int, unsigned int>> traversal)
{
	this->traversal = traversal;
}

Traversal::~Traversal()
{
	this->traversal.clear();
}

void Traversal::curve_snap(vector <pair<double, double>>* curve_points, double ** G, unsigned int Gd_dim, unsigned int Gk_dim, vector <double>* new_vector)
{
	for (int i = 0; i < curve_points->size(); i++)
	{
		matrix_mult(curve_points->at(i), G, Gd_dim, Gk_dim, new_vector);
	}
	return;
}

vector <pair<unsigned int, unsigned int>> Traversal::get_traversal(void)
{
	return this->traversal;
}

int Traversal::get_traversal_size(void)
{
	return this->traversal.size();
}

// Class Traversal_LSH functions
Traversal_LSH::Traversal_LSH(vector <pair<unsigned int, unsigned int>> traversal): Traversal(traversal)
{
	this->lsh = NULL;
}

Traversal_LSH::~Traversal_LSH()
{
	delete this->lsh;
}

void Traversal_LSH::train(unsigned int curve_size, vector <Curve*> dataset, int L, int k, double ** G, unsigned int Gd_dim, unsigned int Gk_dim)
{
	// Create pointset
	vector <Point *> lsh_dataset;
	for (int curve = 0; curve < dataset.size(); curve++)
	{
		Curve * cur_curve = dataset.at(curve);
		vector <pair<double, double>> curve_points;
		vector <double>	new_vector;
		if (cur_curve->get_length() == curve_size)
		{
			for (int point = 0; point < this->get_traversal_size(); point++)
			{
				curve_points.push_back((*cur_curve)[get<0>(this->get_traversal().at(point))]);
			}
			this->curve_snap(&curve_points, G, Gd_dim, Gk_dim, &new_vector);

			lsh_dataset.push_back(new Point(to_string(curve), &new_vector));
			curve_points.clear();
			new_vector.clear();
		}
	}

	// Train lsh
	if (lsh_dataset.size() > 0)
	{
		this->lsh = new LSH(&lsh_dataset, L, k, lsh_dataset.at(0)->get_dimension());
	}	

	lsh_dataset.clear();
	return;
}

NN * Traversal_LSH::predict(Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim)
{
// cout << "mpainei predict3" << endl;

	vector <pair<double, double>> curve_points;
	vector <double>	new_vector;
// cout << "pairnei pragmata tou traversal" << endl;
	for (int point = 0; point < this->get_traversal_size(); point++)
	{
		curve_points.push_back((*query)[get<1>(this->get_traversal().at(point))]);
	}
// cout << "teleiwnei pragmata toy traversal mpainei snap" << endl;
	this->curve_snap(&curve_points, G, Gd_dim, Gk_dim, &new_vector);
// cout << "vgenei snap" << endl;
	Point * lsh_query = new Point(query->get_id(), &new_vector);
// cout << "mpainei lsh predict" << endl;
	NN * return_value = this->lsh->predict(lsh_query);
// cout << "bgainei lsh predict" << endl;
	curve_points.clear();
	new_vector.clear();
	delete lsh_query;
// cout << "vgenei predict3" << endl;
// cout << "                          edw mwrh " << return_value->get_id() << endl;
	return return_value;
}

// void Hypercube_LSH::train(unsigned int curve_size, vector <Curve*> dataset, int L, int k, double ** G, unsigned int Gd_dim, unsigned int Gk_dim)
// {
// 	// Create pointset
// 	vector <Point *> lsh_dataset;
// 	for (int curve = 0; curve < dataset.size(); curve++)
// 	{
// 		Curve * cur_curve = dataset.at(curve);
// 		vector <pair<double, double>> curve_points;
// 		vector <double>	new_vector;
// 		if (cur_curve->get_length() == curve_size)
// 		{
// 			for (int point = 0; point < this->get_traversal_size(); point++)
// 			{
// 				curve_points.push_back((*cur_curve)[get<0>(this->get_traversal().at(point))]);
// 			}
// 			this->curve_snap(&curve_points, G, Gd_dim, Gk_dim, &new_vector);

// 			lsh_dataset.push_back(new Point(to_string(curve), &new_vector));
// 			curve_points.clear();
// 			new_vector.clear();
// 		}
// 	}

// 	// Train lsh
// 	if (lsh_dataset.size() > 0)
// 	{
// 		this->lsh = new LSH(&lsh_dataset, L, k, lsh_dataset.at(0)->get_dimension());
// 	}	

// 	lsh_dataset.clear();
// 	return;
// }

// NN * Hypercube_LSH::predict(Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim)
// {
// // cout << "mpainei predict3" << endl;

// 	vector <pair<double, double>> curve_points;
// 	vector <double>	new_vector;
// // cout << "pairnei pragmata tou traversal" << endl;
// 	for (int point = 0; point < this->get_traversal_size(); point++)
// 	{
// 		curve_points.push_back((*query)[get<1>(this->get_traversal().at(point))]);
// 	}
// // cout << "teleiwnei pragmata toy traversal mpainei snap" << endl;
// 	this->curve_snap(&curve_points, G, Gd_dim, Gk_dim, &new_vector);
// // cout << "vgenei snap" << endl;
// 	Point * lsh_query = new Point(query->get_id(), &new_vector);
// // cout << "mpainei lsh predict" << endl;
// 	NN * return_value = this->lsh->predict(lsh_query);
// // cout << "bgainei lsh predict" << endl;
// 	curve_points.clear();
// 	new_vector.clear();
// 	delete lsh_query;
// // cout << "vgenei predict3" << endl;
// // cout << "                          edw mwrh " << return_value->get_id() << endl;
// 	return return_value;
// }

// Private class IJ_Traversals functions
void IJ_Cell::calc_diagon(set <pair<unsigned int, unsigned int>> * diagonal, unsigned int sizeI, unsigned int sizeJ, unsigned int dist)
/* If distance > 1 we add matrix elements with distance = dist from the diagonal */
{
	if (sizeI == 0 || sizeJ == 0)
	{
		return;
	}

	double angle = (double)sizeJ/sizeI;
	double rev_angle = (double)sizeI/sizeJ;
	for (int i = 0; i < sizeI; i++)
	{
		diagonal->insert(make_pair(i, floor(i * angle)));
	}
	for (int j = 0; j < sizeJ; j++)
	{
		diagonal->insert(make_pair(floor(j * rev_angle), j));
	}

	return;
}

void IJ_Cell::calc_traversals(set <pair<unsigned int, unsigned int>> * diagonal, pair<unsigned int, unsigned int> start, pair<unsigned int, unsigned int> end, vector<pair<unsigned int, unsigned int>> traversal, string type)
{
// cout << get<0>(start) << "  " <<  get<1>(start)<< endl;
	if (start == end)
	{
		traversal.push_back(start);

		if (!type.compare("LSH"))
		{
			this->traversals.insert(make_pair(traversal.size(), new Traversal_LSH(traversal)));
		}
		else if(!type.compare("Hypercube"))
		{
			this->traversals.insert(make_pair(traversal.size(), new Traversal_Hypercube(traversal)));
		}
		else
		{
			cerr << "WRONG TYPE OF FUNCTION IN IJ_Cell::calc_traversals" << endl;
			exit(1);
		}
		
// for (auto i = traversal.begin(); i != traversal.end(); ++i)
    // std::cout << get<1>(*i) << ' ' << get<0>(*i) << endl;
// cout << "            graefjaweiulfhweliaudhfiwleha" << endl;

		traversal.clear();

	}
	else if (diagonal->find(start) != diagonal->end())
	{
		traversal.push_back(start);
		this->calc_traversals(diagonal, make_pair(get<0>(start)+1, get<1>(start)), end, traversal, type);
		this->calc_traversals(diagonal, make_pair(get<0>(start), get<1>(start)+1), end, traversal, type);
		this->calc_traversals(diagonal, make_pair(get<0>(start)+1, get<1>(start)+1), end, traversal, type);
	}

	return;
}

// Class IJ_Traversals
IJ_Cell::IJ_Cell(string type, vector <Curve*> dataset, unsigned int L, unsigned int k, double ** G, unsigned int Gd_dim, unsigned int Gk_dim, unsigned int i, unsigned int j, unsigned int dist)
{
	set <pair<unsigned int, unsigned int>> diagonal;
	this->calc_diagon(&diagonal, i ,j ,dist);

// if (i == 7 && j == 5)
// {
	
// cout << "traversal " << i << " " << j << endl;
	vector<pair<unsigned int, unsigned int>> temp;
	this->calc_traversals(&diagonal, make_pair(0,0), make_pair(i-1,j-1), temp, type);
// cout << "end traversal" << endl;
// }

	unordered_multimap<int , Traversal *>::iterator it;
	for (it = this->traversals.begin(); it != this->traversals.end(); ++it) 
    {
    	it->second->train(i, dataset, L, k, G, Gd_dim, Gk_dim);
    }

// cout << i << " " << j << endl;
// cout << this->traversals.size() << endl;


	diagonal.clear();
}

IJ_Cell::~IJ_Cell()
{
	unordered_multimap<int , Traversal *>::iterator it;
	for (it = this->traversals.begin(); it != this->traversals.end(); ++it) 
    {
    	delete it->second;
    }
	this->traversals.clear();
}

NN * IJ_Cell::predict(vector <Curve*> data, Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim)
{
// cout << "mpainei predict2" << endl;
	NN *nearest_neighbor = NULL, *temp_neighbor = NULL;
	string min_id;
	double min_dist;
	for ( auto trav = this->traversals.begin(); trav != this->traversals.end(); ++trav ) 
	// for (int i = 0; i < this->traversals.size(); i++)
	{
// cout << "mpainei traversal for tou predict2" << endl;
// cout << "mpainei predict3" << endl;
		temp_neighbor = trav->second->predict(query, G, Gd_dim, Gk_dim);
// cout << "vgenei predict3 mpainei dtw" << endl;
		if (temp_neighbor != NULL)
		{
// cout << "lalalalaalalalalalal " << temp_neighbor->get_id() << endl;
			double new_dist = DTW_distance(data.at(stoi(temp_neighbor->get_id())), query);
// cout << "vgenei dtw" << endl;
			if ((nearest_neighbor != NULL) && (min_dist > new_dist))
			{
				min_dist = new_dist;
				delete nearest_neighbor;
				nearest_neighbor = temp_neighbor;
				temp_neighbor = NULL;
			}
			else if (nearest_neighbor == NULL)
			{
				min_dist = new_dist;
				nearest_neighbor = temp_neighbor;
				temp_neighbor = NULL;
			}
			else
			{
				delete temp_neighbor;
				temp_neighbor = NULL;
			}
		}
// cout << "vgainei traversal for tou predict2" << endl;

	}

// cout << "vgainei predict2" << endl;
// cout << "                          edw mwrh " << nearest_neighbor->get_id() << endl;
	return nearest_neighbor;
}

// Functions
double ** createG(int k, int d){

	double ** G;

	int pos = 0;
	try {
		G = new  double *[k];
		for (pos = 0; pos < k; pos++)
		{
			G[pos] = new double[d];
		}
	}
	catch(std::bad_alloc& ba){
		for (int i = 0; i < pos; i++)
		{
			delete [] G[i];
		}
		delete [] G;
		return NULL;
	}

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	default_random_engine generator (seed);
	normal_distribution<double> distribution(0.0,1.0);

	for (int i = 0; i < k; i++)
	{
		for (int j = 0; j < d; j++)
		{
			G[i][j] = distribution(generator);
		}
	}

	return G;
}

IJ_Cell *** train(vector <Curve*> data, unsigned int M, double **G, unsigned int Gd_dim, unsigned int Gk_dim, unsigned int L, unsigned int k, string type){

	IJ_Cell *** MM_table;
	unsigned int pos1 = 0, pos2 = 0;
	try {
		MM_table = new IJ_Cell ** [M];
		for (pos1 = 0; pos1 < M; pos1++)
		{
			MM_table[pos1] = new IJ_Cell * [M];
			for (pos2 = 0; pos2 < M; pos2++)
			{
				if (abs((int)pos1 - (int)pos2) <= 2)
				{
					MM_table[pos1][pos2] = new IJ_Cell(type, data, L, k, G, Gd_dim, Gk_dim, pos1+1, pos2+1);
				}
				else
				{
					MM_table[pos1][pos2] = NULL;
				}
			}
		}
	}
	catch (std::bad_alloc& ba) {
		for (int i = 0; i < pos1; i++)
		{
			if (i < pos1-1)
			{
				for (int j = 0; j < M; j++)
				{
					delete MM_table[i][j];
				}
			}
			for (int j = 0; j < pos2; j++)
			{
				delete MM_table[i][j];
			}
			delete [] MM_table[i];
		}
		delete [] MM_table;
		return NULL;
	}

	return MM_table;
}

vector <Curve*> struct_initialization(string file){

	ifstream data;

	vector <Curve *> data_vector;
	data.open(file);
	
	string line;
	int i = 0;
	if (data.is_open())
	{
		while ( getline (data, line) && line.length() > 0 )
		{
			int pos1, pos2;
			string sub;

			// Find curve ID
			pos2 = line.find("\t");
			string id = line.substr(0, pos2);

			// Find curve size
			line = line.substr(pos2+1);
			pos2 = line.find("\t");
			sub = line.substr(0, pos2);

			if (stoi(sub) > 10)
			{
				continue;
			}

			data_vector.push_back(new Curve(id));

			double x,y;
			while (!line.empty())
			{
				// Find coordinate x
				pos1 = line.find("("); 
				pos2 = line.find(","); 
				sub = line.substr(pos1 + 1, pos2 - pos1 - 1);
				x = stod(sub);

				// Move line
				line = line.substr(pos2 + 2);

				// Find coordinate y
				pos2 = line.find(")");
				sub = line.substr(0, pos2); 
				y = stod(sub);

				// Move line
				line = line.substr(pos2 + 1);

				data_vector.at(i)->add_point(x, y);
			}

			i++;
		}
		data.close();
	}

	return data_vector;

}

NN * predict(IJ_Cell *** MM_table, unsigned int M, vector <Curve*> data, Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim)
{
// cout << "mpainei predict1" << endl;
	unsigned int query_size = query->get_length();
// cout << "query size = " << query_size << endl;
	string min_id = "none";
	double min_dist;
	NN *temp_neighbor = NULL;
	for (int i = query_size-2 ; i <= query_size +2 ; i++)
	{
// cout << "mpainei for predict1" << endl;
// cout << i-1 <<  "        " << query_size-1 << endl;
		if ((i < M) && (i > 0) && (query_size > 0) && (query_size < M) && (MM_table[i-1][query_size-1] != NULL))
		{
// cout << "mpainei predict2" << endl;
			temp_neighbor = MM_table[i-1][query_size-1]->predict(data, query, G, Gd_dim, Gk_dim);
// cout << "bgainei predict2 mpainei dtw" << endl;
			if (temp_neighbor != NULL)
			{
				double new_dist = DTW_distance(data.at(stoi(temp_neighbor->get_id())), query);
// cout << "bgainei dtw predict1" << endl;
				if ((min_id.compare("none")) && (min_dist > new_dist))
				{
					min_dist = new_dist;
					min_id = temp_neighbor->get_id();
				}
				else if (!min_id.compare("none"))
				{
					min_dist = new_dist;
					min_id = temp_neighbor->get_id();
				}

				delete temp_neighbor;
			}	
		}
// cout << "bgainei for predict1" << endl;
	}

	if (!min_id.compare("none"))
	{
		return NULL;
	}
// cout << min_id << endl;
// cout << stoi(min_id) << endl;
	return new NN(data.at(stoi(min_id))->get_id(), min_dist);

}

// For utilities
bool matrix_mult(pair <double, double> pair, double ** G, unsigned int Gd_dim, unsigned int Gk_dim, vector <double> * mult_res)
{
	if (Gd_dim != 2)
	{
		return false;
	}

	for (int i = 0; i < Gk_dim; i++)
	{
		mult_res->push_back(G[i][0] * get<0>(pair) + G[i][1] * get<1>(pair));
	}
	
	return true;
}