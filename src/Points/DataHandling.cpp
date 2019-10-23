#include "./DataHandling.hpp"

using namespace std;

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

bool check_arguments_cube(int argc, char* argv[], string * input_file, string * queries_file, unsigned int* k, unsigned int* M, unsigned int* probes, string * output_file){

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

bool read(string file_name, vector<Point*>* points, int* r){

	string line;
	int d;

  	ifstream myfile;
  	myfile.open(file_name);

  	if (myfile.is_open())
  	{
  		getline(myfile, line);
  		// If i am looking for range
  		if ( r!=NULL )
  			if (find_range(line, r)){	// If there is a range
  				// Read a new line
  				getline(myfile, line);
  			}

  		//Read the first point and store d
    	if ( !point_proccessing(points, line) ){
    		return false;
    	}
    	d = ((*points)[0])->get_dimension();

  		// Read rest points
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

bool find_range(string line, int* r){

	stringstream L;
	L << line;
	string name;
	string r_string;

	getline(L, name, ' ');
	if ( name.compare("Radius:") != 0 )
		return false;
	else{
		getline(L, r_string, ' ');
		(*r) = stoi(r_string);
		return true;
	}

}

bool point_proccessing(vector<Point*>* points, string p, int d){

	stringstream L;
	L << p;
	string coordinate;

	if ( !getline(L, coordinate, ' ') )
		return false;

	// Get the id and deside the type
	string id = coordinate;
	vector<double> X;
	while( getline(L, coordinate, ' ') ){
		if ( coordinate.compare("\r") == 0 ) break;
		X.push_back( stod(coordinate) );
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

void update_output_lsh(string* output, string query_id, NN* lsh_nearest_neighbor, NN* true_nearest_neighbor, int duration_lsh, int duration_brute_force){

	string query = "Query: "+query_id+"\nFound Nearest neighbor: "+lsh_nearest_neighbor->get_id()+"\nTrue Nearest neighbor: "+true_nearest_neighbor->get_id()
					+"\ndistanceLSH: "+to_string(lsh_nearest_neighbor->get_distance())+"\ndistanceTrue: "+to_string(true_nearest_neighbor->get_distance())
					+"\ntLSH: "+to_string(duration_lsh)+"\ntTrue: "+to_string(duration_brute_force)+"\n";

	query += "R-near neighbors:\n";
	for (int i = 0; i < lsh_nearest_neighbor->r_near_neighbors_size(); ++i)
	{
		query += lsh_nearest_neighbor->get_near_neighbor(i);
	}
	query += "\n";
	(*output) = (*output) + query;
}

void update_output_cube(string* output, string query_id, NN* hypercube_nearest_neighbor, NN* true_nearest_neighbor, int duration_hypercube, int duration_brute_force){

	string query = "Query: "+query_id+"\nFound Nearest neighbor: "+hypercube_nearest_neighbor->get_id()+"\nTrue Nearest neighbor: "+true_nearest_neighbor->get_id()
					+"\ndistanceHypercube: "+to_string(hypercube_nearest_neighbor->get_distance())+"\ndistanceTrue: "+to_string(true_nearest_neighbor->get_distance())
					+"\ntHypercube: "+to_string(duration_hypercube)+"\ntTrue: "+to_string(duration_brute_force)+"\n";
	
	query += "R-near neighbors:\n";
	for (int i = 0; i < hypercube_nearest_neighbor->r_near_neighbors_size(); ++i)
	{
		query += hypercube_nearest_neighbor->get_near_neighbor(i);
	}
	query += "\n";
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