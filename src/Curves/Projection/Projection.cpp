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
/* Multiplying all points of the curve with function G (k*d matrix * d*1 point) = k*number_of_points vector */
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
/*	Fetching curves' points according to traversal, multiplying with G, Feeding data to LSH */
{
	// Fetching curves' points
	vector <Point *> lsh_dataset;
	for (int curve = 0; curve < dataset.size(); curve++)
	{
		// Useful temp structs
		Curve * cur_curve = dataset.at(curve);
		vector <pair<double, double>> curve_points;
		vector <double>	new_vector;

		if (cur_curve->get_length() == curve_size) // We care for curves of size curve_size because we are in a certain cell of MM_matrix
		{
			for (int point = 0; point < this->get_traversal_size(); point++)
			{
				curve_points.push_back((*cur_curve)[get<0>(this->get_traversal().at(point))]);
			}

			// Multiplying with G
			this->curve_snap(&curve_points, G, Gd_dim, Gk_dim, &new_vector);

			// Creating vector with data for LSH
			lsh_dataset.push_back(new Point(to_string(curve), &new_vector));

			// Clearing memory
			curve_points.clear();
			new_vector.clear();
		}
	}

	// Feed data to lsh
	if (lsh_dataset.size() > 0)
	{
		this->lsh = new LSH(&lsh_dataset, L, k, lsh_dataset.at(0)->get_dimension());
	}	

	// Clearing memory
	lsh_dataset.clear();
	return;
}

NN * Traversal_LSH::predict(Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim)
/*	Fetching curve's points according to traversal, multiplying with G, Predicting query */
{

	// Fetch curve points
	vector <pair<double, double>> curve_points;
	vector <double>	new_vector;
	for (int point = 0; point < this->get_traversal_size(); point++)
	{
		curve_points.push_back((*query)[get<1>(this->get_traversal().at(point))]);
	}

	// Multiply with G
	this->curve_snap(&curve_points, G, Gd_dim, Gk_dim, &new_vector);

	// Predict
	Point * lsh_query = new Point(query->get_id(), &new_vector);
	NN * return_value = this->lsh->predict(lsh_query);

	// Free memory
	curve_points.clear();
	new_vector.clear();
	delete lsh_query;

	// Return Prediction
	return return_value;
}

void Traversal_Hypercube::train(unsigned int curve_size, vector <Curve*> dataset, unsigned int M_hyper, unsigned int k, unsigned int probes, double ** G, unsigned int Gd_dim, unsigned int Gk_dim)
/*	Fetching curves' points according to traversal, multiplying with G, Feeding data to Hypercube */
{
	// Fetching curves' points
	vector <Point *> hypercube_dataset;
	for (int curve = 0; curve < dataset.size(); curve++)
	{
		// Useful temp structs
		Curve * cur_curve = dataset.at(curve);
		vector <pair<double, double>> curve_points;
		vector <double>	new_vector;

		if (cur_curve->get_length() == curve_size) // We care for curves of size curve_size because we are in a certain cell of MM_matrix
		{
			for (int point = 0; point < this->get_traversal_size(); point++)
			{
				curve_points.push_back((*cur_curve)[get<0>(this->get_traversal().at(point))]);
			}

			// Multiplying with G
			this->curve_snap(&curve_points, G, Gd_dim, Gk_dim, &new_vector);

			// Creating vector with data for LSH
			hypercube_dataset.push_back(new Point(to_string(curve), &new_vector));

			// Clearing memory
			curve_points.clear();
			new_vector.clear();
		}
	}

	// Train hypercube
	if (hypercube_dataset.size() > 0)
	{
		this->cube = new Hypercube(&hypercube_dataset, hypercube_dataset.at(0)->get_dimension(), k, M_hyper, probes);
	}	

	// Clearing memory
	hypercube_dataset.clear();
	return;
}

NN * Traversal_Hypercube::predict(Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim)
/*	Fetching curve's points according to traversal, multiplying with G, Predicting query */
{
	// Fetch curve points
	vector <pair<double, double>> curve_points;
	vector <double>	new_vector;
	for (int point = 0; point < this->get_traversal_size(); point++)
	{
		curve_points.push_back((*query)[get<1>(this->get_traversal().at(point))]);
	}

	// Multiply with G
	this->curve_snap(&curve_points, G, Gd_dim, Gk_dim, &new_vector);

	// Predict
	Point * hypercube_query = new Point(query->get_id(), &new_vector);
	NN * return_value = this->cube->predict(hypercube_query);

	// Free memory
	curve_points.clear();
	new_vector.clear();
	delete hypercube_query;

	// Return Prediction
	return return_value;
}

// Private class IJ_Traversals functions
void IJ_Cell::calc_diagon(set <pair<unsigned int, unsigned int>> * diagonal, unsigned int sizeI, unsigned int sizeJ, unsigned int dist)
/*	Calculating which element form the diagonal of the matrix, by using the linear equation.
	If distance > 1 we add matrix elements with distance = dist from the diagonal */
{
	// No diagonal if one of the dimensions equals zero
	if (sizeI == 0 || sizeJ == 0)
	{
		return;
	}

	// Finding the elements that intersect with the line in the x axis
	double angle = (double)sizeJ/sizeI;
	for (int i = 0; i < sizeI; i++)
	{
		diagonal->insert(make_pair(i, floor(i * angle)));
	}

	// Finding the elements that intersect with the line in the y axis
	double rev_angle = (double)sizeI/sizeJ;
	for (int j = 0; j < sizeJ; j++)
	{
		diagonal->insert(make_pair(floor(j * rev_angle), j));
	}

	return;
}

void IJ_Cell::calc_traversals(set <pair<unsigned int, unsigned int>> * diagonal, pair<unsigned int, unsigned int> start, pair<unsigned int, unsigned int> end, vector<pair<unsigned int, unsigned int>> traversal, string type)
/*	Producing all traversals consisting of elements in the diagonal that start from (0,0) and end in (i,j)
	Using recursion. Problem with big data!!	*/
{
	// End of search. Insert the traversal in the list
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

		traversal.clear();

	}
	else if (diagonal->find(start) != diagonal->end())	// Haven't reached the end
	{
		traversal.push_back(start);
		// Check if the bottom element belongs in the diagonal and can form a traversal
		this->calc_traversals(diagonal, make_pair(get<0>(start)+1, get<1>(start)), end, traversal, type);
		// Check if the right element belongs in the diagonal and can form a traversal
		this->calc_traversals(diagonal, make_pair(get<0>(start), get<1>(start)+1), end, traversal, type);
		// Check if the bottom right element belongs in the diagonal and can form a traversal
		this->calc_traversals(diagonal, make_pair(get<0>(start)+1, get<1>(start)+1), end, traversal, type);
	}

	return;
}

// Class IJ_Traversals
IJ_Cell::IJ_Cell(string type, vector <Curve*> dataset, unsigned int L, unsigned int k, double ** G, unsigned int Gd_dim, unsigned int Gk_dim, unsigned int i, unsigned int j, unsigned int dist)
/*	IJ_Cell constructor for LSH structure*/
{
	// Calculating diagonal elements
	set <pair<unsigned int, unsigned int>> diagonal;
	this->calc_diagon(&diagonal, i ,j ,dist);

	// Calculating all possible traversals
	vector<pair<unsigned int, unsigned int>> temp;
	this->calc_traversals(&diagonal, make_pair(0,0), make_pair(i-1,j-1), temp, type);

	// Feeding data to algorithm
	unordered_multimap<int , Traversal *>::iterator it;
	for (it = this->traversals.begin(); it != this->traversals.end(); ++it) 
    {
    	it->second->train(i, dataset, L, k, G, Gd_dim, Gk_dim);
    }

    // Freeing memory
	diagonal.clear();
}

IJ_Cell::IJ_Cell(string type, vector <Curve*> dataset, unsigned int M_hyper, unsigned int k, unsigned int probes, double ** G, unsigned int Gd_dim, unsigned int Gk_dim, unsigned int i, unsigned int j, unsigned int dist)
/*	IJ_Cell constructor for Hypercube structure*/
{
	// Calculating diagonal elements
	set <pair<unsigned int, unsigned int>> diagonal;
	this->calc_diagon(&diagonal, i ,j ,dist);

	// Calculating all possible traversals
	vector<pair<unsigned int, unsigned int>> temp;
	this->calc_traversals(&diagonal, make_pair(0,0), make_pair(i-1,j-1), temp, type);

	// Feeding data to algorithm
	unordered_multimap<int , Traversal *>::iterator it;
	for (it = this->traversals.begin(); it != this->traversals.end(); ++it) 
    {
    	it->second->train(i, dataset, M_hyper, k, probes, G, Gd_dim, Gk_dim);
    }

	// Freeing memory
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
/*	Check all possible neighbors. Find the nearest using curve distance (DTW)	*/
{
	// For each traversal in the cell find the nearest neighbor
	NN *nearest_neighbor = NULL, *temp_neighbor = NULL;
	string min_id;
	double min_dist;
	for ( auto trav = this->traversals.begin(); trav != this->traversals.end(); ++trav ) 
	{
		// Get the nearest neighbor for traversal
		temp_neighbor = trav->second->predict(query, G, Gd_dim, Gk_dim);
		if (temp_neighbor != NULL)
		{
			// Calculate the real distance of the curves
			double new_dist = DTW_distance(data.at(stoi(temp_neighbor->get_id())), query); // Point id is the real position of the curve in the matrix

			// Keep the smallest distance
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
	}

	// Return closest neighbor found in cell
	return nearest_neighbor;
}

// Functions
double ** createG(int k, int d)
/* Matrix of size k * d, consisting of random doubles which follow normal distribution */
{
	double ** G = NULL;

	// Memory allocation
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

	// Production of random double numbers, following normal distribution
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();	// Using seed in order to have more realistic randomness
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

IJ_Cell *** train_LSH(vector <Curve*> data, unsigned int M, double **G, unsigned int Gd_dim, unsigned int Gk_dim, unsigned int L, unsigned int k)
/*	Creating MM_table and initializing boxes of certain diagonals for LSH use	*/
{
	// Memory allocation
	IJ_Cell *** MM_table = NULL;
	unsigned int pos1 = 0, pos2 = 0;
	try {
		MM_table = new IJ_Cell ** [M];
		for (pos1 = 0; pos1 < M; pos1++)
		{
			MM_table[pos1] = new IJ_Cell * [M];
			for (pos2 = 0; pos2 < M; pos2++)
			{
				// Initializing the needed diagonals |i-j|>=2
				if (abs((int)pos1 - (int)pos2) <= 2)
				{
					MM_table[pos1][pos2] = new IJ_Cell("LSH", data, L, k, G, Gd_dim, Gk_dim, pos1+1, pos2+1);
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

IJ_Cell *** train_Hypercube(vector <Curve*> data, unsigned int M, double **G, unsigned int Gd_dim, unsigned int Gk_dim, unsigned int M_hyper, unsigned int k, unsigned int probes)
/*	Creating MM_table and initializing boxes of certain diagonals for Hypercube use	*/
{
	// Memory allocation
	IJ_Cell *** MM_table = NULL;
	unsigned int pos1 = 0, pos2 = 0;
	try {
		MM_table = new IJ_Cell ** [M];
		for (pos1 = 0; pos1 < M; pos1++)
		{
			MM_table[pos1] = new IJ_Cell * [M];
			for (pos2 = 0; pos2 < M; pos2++)
			{
				// Initializing the needed diagonals |i-j|>=2
				if (abs((int)pos1 - (int)pos2) <= 2)
				{
					MM_table[pos1][pos2] = new IJ_Cell("Hypercube", data, M_hyper, k, probes, G, Gd_dim, Gk_dim, pos1+1, pos2+1);
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

NN * predict(IJ_Cell *** MM_table, unsigned int M, vector <Curve*> data, Curve * query, double ** G, unsigned int Gd_dim, unsigned int Gk_dim)
/*	Visiting the cell which represents the curves with length i,j and the near cells (different i, same j)
	and finding the nearest neighbor.
	j = dimension of query curve
	i = dimension near the dimension of query curve	*/
{
	// Finding curve dimension
	unsigned int query_size = query->get_length();
	string min_id = "none";
	double min_dist;
	NN *temp_neighbor = NULL;

	// Search all the cell with dimensions near query dimension
	for (int i = query_size-2 ; i <= query_size +2 ; i++)
	{
		// If the cell address is valid
		if ((i < M) && (i > 0) && (query_size > 0) && (query_size < M) && (MM_table[i-1][query_size-1] != NULL))
		{
			// Find the nearest neighbor of the cell
			temp_neighbor = MM_table[i-1][query_size-1]->predict(data, query, G, Gd_dim, Gk_dim);	// size-1 because curves start from size 1 and matrix from 0
			if (temp_neighbor != NULL)
			{
				// Calculate real distance
				double new_dist = DTW_distance(data.at(stoi(temp_neighbor->get_id())), query);

				// Save the nearest neighbor
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
	}

	// If no neighbor found
	if (!min_id.compare("none"))
	{
		return NULL;
	}

	// Return closest neighbor
	return new NN(data.at(stoi(min_id))->get_id(), min_dist);

}

// For file handling
vector <Curve*> struct_initialization(string file)
/*	Initializing data vector, using all the curves with dimension <= 10	
	File structure = curve_id \t curve_dimension \t (x,y) (x,y) ......	*/
{
	ifstream data;

	vector <Curve *> data_vector;
	data.open(file);
	
	string line;
	int i = 0;
	if (data.is_open())
	{
		// Get line. Each line is a curve
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

			// We need curves with size <= 10
			if (stoi(sub) > 10)
			{
				continue;
			}

			// Add curve to data vector
			data_vector.push_back(new Curve(id));

			// Find curve points
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

				// Add point in curve
				data_vector.at(i)->add_point(x, y);
			}

			i++;
		}
		data.close();
	}

	return data_vector;

}

// For utilities
bool matrix_mult(pair <double, double> pair, double ** G, unsigned int Gd_dim, unsigned int Gk_dim, vector <double> * mult_res)
// Multiplication of matrices of size k*d and d*1
{
	// In our project we have d_dim = 2 by default. 
	if (Gd_dim != 2)
	{
		return false;
	}

	// Multiplication process
	for (int i = 0; i < Gk_dim; i++)
	{
		mult_res->push_back(G[i][0] * get<0>(pair) + G[i][1] * get<1>(pair));
	}
	
	return true;
}