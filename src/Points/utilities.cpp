#include "./utilities.hpp"

using namespace std;

Point::Point(string id, vector<double>* X){

	this->id = id;
	for (int i=0; i<(*X).size(); i++){
		this->X.push_back((*X)[i]);
	}
}

int Point::get_dimension(void){
	return this->X.size();
}

double Point::operator[](int i){
	 if ( i>=0 && i<this->X.size() )
	 	return this->X[i];
	 else
	 	return -1; 
}

NN::NN(string id, double distance, set<string>* neighbors){

	this->id = id;
	this->distance = distance;

	if ( neighbors != NULL ){
		(this->r_near_neighbors).insert((this->r_near_neighbors).end(), (*neighbors).begin(), (*neighbors).end());
		(*neighbors).clear();
	}
}

string NN::get_near_neighbor(int i){

	if ( i<0 || i >= (this->r_near_neighbors).size())
		return "";
	return (this->r_near_neighbors)[i];
}

unsigned int NN::r_near_neighbors_size(){
	return (this->r_near_neighbors).size();
}

vector<string> NN::get_near_neighbors(void){
	return this->r_near_neighbors;
}

void NN::add_neighbors(vector<string>* new_neighbors){

	for (int i = 0; i < (*new_neighbors).size(); ++i)
	{
		(this->r_near_neighbors).push_back((*new_neighbors)[i]);
	}
}

NN* brute_force(Point* point, vector<Point*>* pointset){

	string min_id = "";
	int min_distance;

	int current_distance;
	for (int i = 0; i < (*pointset).size(); ++i)
	{
		if ( (point->get_id()).compare((*pointset)[i]->get_id()) != 0 )
		{
			// Calculate the distance
			current_distance = manhattan_dist(point, (*pointset)[i]);

			// Replace min if this is the first point or if current distance < min distance
			if ( min_id.compare("") == 0 || current_distance < min_distance )
			{
				min_distance = current_distance;
				min_id = (*pointset)[i]->get_id();
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
	else	// If there is no point except this point in the pointset
		return NULL;
}

double manhattan_dist(Point* x, Point* y){

	double distance = 0;
	int xi, yi;
	double d;
	for (int i = 0; i < x->get_dimension(); ++i)
	{
		xi = (*x)[i];
		yi = (*y)[i];

		d = xi - yi;
		if ( d >= 0 )
			distance += d;
		else
			distance += yi - xi; 
	}

	return distance;
}

double average_distance(vector<Point*>* pointset){


	int size = (*pointset).size();
	int step = floor(sqrt(size));

	int sum_d = 0;
	NN * nearest_neighbor;
	for (int i = 0; i < size; i+=step)
	{
		nearest_neighbor = brute_force((*pointset)[i], &(*pointset));
		sum_d += nearest_neighbor->get_distance();
		delete nearest_neighbor;
	}

	return sum_d/(size/step);
}

void print_points(vector<Point*> points){

	for (int i = 0; i < points.size(); ++i)
	{
		cout << points[i]->get_id() << "\t";
		for (int j = 0; j < points[i]->get_dimension(); ++j)
		{
			cout << (*points[i])[j];
			if ( j != points[i]->get_dimension()-1)
				cout << "\t";
		}
		cout << endl;
	}
} 

void print_hash_tables(vector<unordered_map<unsigned int, vector<Point*>>>* hash_tables){

	int num_of_hash_tables;
   for (auto hash_table : ((*hash_tables)[0])) 
   {
      	// cout << hash_table.first << endl;
      	num_of_hash_tables ++;
      	vector<Point*> points = hash_table.second;
      	// print_points(points);
      	// cout << endl << endl;
	}

	cout << "num_of_hash_tables " << num_of_hash_tables << endl;
}

bool check_for_new_queries(string* queries_file, string* output_file){

	string answer;
	bool right_answer = false;

	while ( !right_answer ){
		cout << "Do you want to search the nearest neighbors in a new query file? y or n" << endl;
		cin >> answer;

		if ( (answer.compare("y") == 0) || (answer.compare("yes") == 0) ){
			// right_answer = true;
			cout << "Insert the name of the queries file" << endl;
			cin >> (*queries_file);

			cout << "Insert the name of the output_file" << endl;
			cin >> (*output_file);
			return true;
		}
		else if ( (answer.compare("n") == 0) || (answer.compare("no") == 0) ){
			right_answer = true;
			return false;
		}
		else{
			right_answer = false;
			cout << "Invalid answer try again" << endl;
		}
	}
}