#include "./utilities.h"

using namespace std;

// να κανω μια συναρτηση τα check arguments

Point::Point(string id, vector<int>* X){

	this->id = id;
	for (int i=0; i<(*X).size(); i++){
		this->X.push_back((*X)[i]);
	}
}

int Point::operator[](int i){
	 if ( i>=0 && i<this->X.size() )
	 	return this->X[i];
	 else
	 	return -1; 
}

bool check_arguments_lsh(int argc, char* argv[], string * input_file, string * queries_file, int* k, int* L, string * output_file){

	// Initialize parameters in case they are not given
	(*k) = 4;
	(*L) = 5;

	int opt;
	bool neccessary_arg = false;
	while ( (opt = getopt(argc, argv, "d:q:k:L:o:")) != -1 || opt == '?' )
	{
		switch (opt)
		{
			case 'd':
				(*input_file) = optarg;
				break;
			case 'q':
				(*queries_file) = optarg;
				break;
			case 'k':
				(*k) = atoi(optarg);
				break;
			case 'L':
				(*L) = atoi(optarg);
				break;
			case 'o':
				neccessary_arg = true;
				(*output_file) = optarg;
				break;
			default:
				cerr << "Usage ./lsh -d <input_file> -q <query_file> -k <int> -L <int> -o <output_file>" << endl;
				return false;
		}
	}

	if ( opt == '?' || neccessary_arg == false ){
		cerr << "Usage ./lsh -d <input_file> -q <query_file> -k <int> -L <int> -o <output_file>" << endl;
		return false;
	}

	// If input file was not in command line
	if ( (*input_file).compare("") == 0 )
	{
		cout << "Please give the name of the input file" << endl;
		cin >> (*input_file);
	}

	// If queries file is not given in command line
	if ( (*queries_file).compare("") == 0 )
	{
		cout << "Please give the name of the queries file" << endl;
		cin >> (*queries_file);
	}

	return true;
}

bool check_arguments_cube(int argc, char* argv[], string * input_file, string * queries_file, int* k, int* M, int* probes, string * output_file){

	// Initialize parameters in case they are not given
	(*k) = 3;
	(*M) = 10;
	(*probes) = 2;

	int opt;
	bool neccessary_arg = false;
	while ( (opt = getopt(argc, argv, "d:q:k:M:p:o:")) != -1 || opt == '?' )
	{
		switch (opt)
		{
			case 'd':
				(*input_file) = optarg;
				break;
			case 'q':
				(*queries_file) = optarg;
				break;
			case 'k':
				(*k) = atoi(optarg);
				break;
			case 'M':
				(*M) = atoi(optarg);
				break;
			case 'p':
				(*probes) = atoi(optarg);
				break;
			case 'o':
				neccessary_arg = true;
				(*output_file) = optarg;
				break;
			default:
				return false;
		}
	}

	if ( opt == '?' || neccessary_arg == false )
		return false;

	// If input file was not in command line
	if ( (*input_file).compare("") == 0 )
	{
		cout << "Please give the name of the input file" << endl;
		cin >> (*input_file);
	}

	// If queries file is not given in command line
	if ( (*queries_file).compare("") == 0 )
	{
		cout << "Please give the name of the queries file" << endl;
		cin >> (*queries_file);
	}

	return true;
}

bool read(string file_name, vector<Point*>* points){

	string line;
	int d;

  	ifstream myfile;
  	myfile.open(file_name);

  	if (myfile.is_open())
  	{
  		//Read the first line and store d
  		getline(myfile, line);
    	if ( !point_proccessing(points, line) ){
    		return false;
    	}

    	d = (*points)[0]->get_dimension();

  		// Read rest lines
    	while ( getline(myfile, line) ){
    	  if ( !point_proccessing(points, line, d) ){
    	  	return false;
    	  }
    	}
    	myfile.close();
  	}
  	else
  	{
  		cerr << "Unable to open file" << endl;
  		return false;
  	}
  	return true;
}

bool point_proccessing(vector<Point*>* points, string p, int d){

	stringstream L;
	L << p;
	string coordinate;

	if ( !getline(L, coordinate, ' ') )
		return false;

	// Get the id and deside the type
	string id = coordinate;
	
	vector<int> X;
	while( getline(L, coordinate, ' ') ){
		X.push_back( atoi(coordinate.c_str()) );
	}

	if ( d != -1 && X.size() != d ){
		cerr << "Wrong input file" << endl;
		return false;
	}
	// Create a point
	Point* point;
	try{
		point = new Point(id, &X);
	}
	catch(std::bad_alloc&) {
		cerr << "No memory available" << endl;
		return false;   
	}
	X.clear();

	// Insert point in the list
	(*points).push_back(point);

	return true;
}

void update_output(string* output, string query_id, NN* lsh_nearest_neighbor, NN* true_nearest_neighbor, int duration_lsh, int duration_brute_force){

	string query = "Query: "+query_id+"\nFound Nearest neighbor: "+lsh_nearest_neighbor->get_id()+"\nTrue Nearest neighbor: "+true_nearest_neighbor->get_id()
					+"\ndistanceLSH: "+to_string(lsh_nearest_neighbor->get_distance())+"\ndistanceTrue: "+to_string(true_nearest_neighbor->get_distance())
					+"\ntLSH: "+to_string(duration_lsh)+"\ntTrue: "+to_string(duration_brute_force)+"\n\n";
	(*output) = (*output) + query;
}

bool write_output(string file_name, string output){

	ofstream myfile;
	myfile.open(file_name);

	if (myfile.is_open())
	{
		myfile << output;
		myfile.close();
	}
	else
	{
		cerr << "Unable to open file" << endl;
		return false;
	}

	return true;
}

NN* brute_force(Point* point, vector<Point*>* pointset){

	// // Initialize min
	// string min_id = (*pointset)[0]->get_id();
	// int min_distance = manhattan_dist(point, (*pointset)[0]);	

	// int current_distance;
	// for (int i = 1; i < (*pointset).size(); ++i)
	// {
	// 	// Calculate the distance
	// 	current_distance = manhattan_dist(point, (*pointset)[i]);

	// 	// Replace min if neccessary
	// 	if ( current_distance < min_distance )
	// 	{
	// 		min_distance = current_distance;
	// 		min_id = (*pointset)[i]->get_id();
	// 	}
	// }

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

int manhattan_dist(Point* x, Point* y){

	int distance = 0;
	int xi, yi, d;
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

int average_distance(vector<Point*>* pointset){

	int size = (*pointset).size();
	int step = floor(sqrt(size));

	int sum_d = 0;
	for (int i = 0; i < size; i+=step)
	{
		sum_d += brute_force((*pointset)[i], pointset)->get_distance();
	}

	return floor(sum_d/(size/step));

	// int size = (*pointset).size();
	// int step = floor(sqrt(size));

	// int sum_d = 0;
	// for (int i = 0; i < size; i++)
	// {
	// 	sum_d += brute_force((*pointset)[i], pointset)->get_distance();
	// }

	// return floor(sum_d/size);

}

int modulo(int a, int b) {
  int m = a % b;
  if (m < 0) {
    m = (b < 0) ? m - b : m + b;
  }
  return m;
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

void print_hash_tables(vector<unordered_map<int, vector<Point*>>>* hash_tables){

   for (auto hash_table : ((*hash_tables)[0])) 
   {
      	// cout << hash_table.first << endl; 
      	vector<Point*> points = hash_table.second;
      	// print_points(points);
      	cout << endl << endl;
	}
}