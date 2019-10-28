#include "DataHandling.hpp"

#include <cstring>

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
				if ( (*k) == 0 ){
					cerr << "Wrong value of k" << endl;
					return false;
				}
				break;
			case 'L':
				(*L) = atoi(optarg);
				if ( (*L) == 0 ){
					cerr << "Wrong value of L" << endl;
					return false;
				}
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

	// If input file is not given from the command line
	if ( (*input_file).compare("") == 0 )
	{
		cout << "Please give the name of the input file" << endl;
		cin >> (*input_file);
	}

	// If queries file is not given from the command line
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
				if ( (*k) == 0 ){
					cerr << "Wrong value of k" << endl;
					return false;
				}
				break;
			case 'M':
				(*M) = atoi(optarg);
				if ( (*M) == 0 ){
					cerr << "Wrong value of M" << endl;
					return false;
				}
				break;
			case 'p':
				(*probes) = atoi(optarg);
				if ( (*probes) == 0 ){
					cerr << "Wrong value of probes" << endl;
					return false;
				}
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

	// If input file is not given from the command line
	if ( (*input_file).compare("") == 0 )
	{
		cout << "Please give the name of the input file" << endl;
		cin >> (*input_file);
	}

	// If queries file is not given from the command line
	if ( (*queries_file).compare("") == 0 )
	{
		cout << "Please give the name of the queries file" << endl;
		cin >> (*queries_file);
	}

	return true;
}

// Read points from a file
bool read(string file_name, vector<Point*>* points, int* r){

	string line;
	int d;

  	ifstream myfile;
  	myfile.open(file_name);

  	if (myfile.is_open())
  	{
  		getline(myfile, line);
  		// If this file can contain range
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
// Transform data to Point structure
bool point_proccessing(vector<Point*>* points, string p, int d){

	stringstream L;
	L << p;
	string coordinate;

	if ( !getline(L, coordinate, ' ') )
		return false;

	// Get the id
	string id = coordinate;

	// Get the coordinates
	vector<double> X;
	while( getline(L, coordinate, ' ') ){
		if ( coordinate.compare("\r") == 0 ) break;
		X.push_back( stod(coordinate) );
	}

	// If this is not the first point and the dimension is different in this point
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
		query += lsh_nearest_neighbor->get_near_neighbor(i) +"\n";
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
		query += hypercube_nearest_neighbor->get_near_neighbor(i) +"\n";
	}
	query += "\n";
	(*output) = (*output) + query;
}

vector <Curve *> file_handling_grid_lsh(int argc, char * argv[], string * queries, string * output, int * K, int * L, int* max_d, int * min_d)
{
	string input = "";
	*queries = "";
	*output = "";

	*L = 4;
	*K = 3;

	if (argc % 2 != 1)
	{
		cerr << "Invalid arguments" << endl;
		exit(1);
	}

	for (int i = 1; i < argc; i+=2)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			input = argv[i+1];
		}
		else if (strcmp(argv[i], "-q") == 0)
		{
			*queries = argv[i+1];
		}
		else if (strcmp(argv[i], "-k_vec") == 0)
		{
			*K = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-L_grid") == 0)
		{
			*L = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			*output = argv[i+1];
		}
	}

	if (input.empty())
	{
		cout << "Give input file path." << endl;
		cin >> input;
	}
	if ((*queries).empty())
	{
		cout << "Give queries file path." << endl;
		cin >> *queries;
	}

	vector <Curve*> dataset = struct_initialization_grid(input, max_d, min_d);

	if ((*output).empty())
	{
		cout << "Give results file path" << endl;
		cin >> *output;
	}

	return dataset;
}

vector <Curve *> file_handling_grid_hypercube(int argc, char * argv[], string * queries, string * output, unsigned int * K, unsigned int * M, unsigned int * probes, unsigned int * L, int* max_d, int* min_d)
{
	string input = "";
	*queries = "";
	*output = "";

	*K = 3;
	*L = 4;
	*M = 10;
	*probes = 2;

	if (argc % 2 != 1)
	{
		cerr << "Invalid arguments" << endl;
		exit(1);
	}

	for (int i = 1; i < argc; i+=2)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			input = argv[i+1];
		}
		else if (strcmp(argv[i], "-q") == 0)
		{
			*queries = argv[i+1];
		}
		else if (strcmp(argv[i], "-k_hypercube") == 0)
		{
			*K = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-M") == 0)
		{
			*M = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-probes") == 0)
		{
			*probes = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-L_grid") == 0)
		{
			*L = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			*output = argv[i+1];
		}
	}

	if (input.empty())
	{
		cout << "Give input file path." << endl;
		cin >> input;
	}
	if ((*queries).empty())
	{
		cout << "Give queries file path." << endl;
		cin >> *queries;
	}

	vector <Curve*> dataset = struct_initialization_grid(input, max_d, min_d);

	if ((*output).empty())
	{
		cout << "Give results file path" << endl;
		cin >> *output;
	}

	return dataset;
}

vector <Curve *> file_handling_projection_lsh(int argc, char * argv[], string * queries, string * output, unsigned int * K, unsigned int * L, double * e, unsigned int *M)
{
	string input = "";
	*queries = "";
	*output = "";

	*e = 0.5;
	*K = 3;
	*L = 5;
	*M = 0;

	if (argc % 2 != 1)
	{
		cerr << "Invalid arguments" << endl;
		exit(1);
	}

	for (int i = 1; i < argc; i+=2)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			input = argv[i+1];
		}
		else if (strcmp(argv[i], "-q") == 0)
		{
			*queries = argv[i+1];
		}
		else if (strcmp(argv[i], "-k_vec") == 0)
		{
			*K = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-L_vec") == 0)
		{
			*L = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-e") == 0)
		{
			*e = atof(argv[i+1]);
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			*output = argv[i+1];
		}
	}

	if (input.empty())
	{
		cout << "Give input file path." << endl;
		cin >> input;
	}
	if ((*queries).empty())
	{
		cout << "Give queries file path." << endl;
		cin >> *queries;
	}

	vector <Curve*> dataset = struct_initialization_projection(input);

	for (int i = 0; i < dataset.size(); i++)
	{
		if ((*M) < dataset.at(i)->get_length())
		{
			(*M) = dataset.at(i)->get_length();
		}
	}

	if ((*output).empty())
	{
		cout << "Give results file path" << endl;
		cin >> *output;
	}

	return dataset;
}

vector <Curve *> file_handling_projection_hypercube(int argc, char * argv[], string * queries, string * output, unsigned int * K, unsigned int * M_hyper, unsigned int * probes, double * e, unsigned int *M)
{
	string input = "";
	*queries = "";
	*output = "";

	*e = 0.5;
	*M_hyper = 10;
	*K = 3;
	*probes = 2;

	*M = 0;

	if (argc % 2 != 1)
	{
		cerr << "Invalid arguments" << endl;
		exit(1);
	}

	for (int i = 1; i < argc; i+=2)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			input = argv[i+1];
		}
		else if (strcmp(argv[i], "-q") == 0)
		{
			*queries = argv[i+1];
		}
		else if (strcmp(argv[i], "-k_hypercube") == 0)
		{
			*K = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-M") == 0)
		{
			*M_hyper = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-probes") == 0)
		{
			*probes = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-e") == 0)
		{
			*e = atof(argv[i+1]);
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			*output = argv[i+1];
		}
	}

	// Ask for input file name if it's not already given
	if (input.empty())
	{
		cout << "Give input file path." << endl;
		cin >> input;
	}
	// Ask for query file name if it's not already given
	if ((*queries).empty())
	{
		cout << "Give queries file path." << endl;
		cin >> *queries;
	}

	// Initialize dataset
	vector <Curve*> dataset = struct_initialization_projection(input);

	// Calculate M for MM_matrix
	for (int i = 0; i < dataset.size(); i++)
	{
		if ((*M) < dataset.at(i)->get_length())
		{
			(*M) = dataset.at(i)->get_length();
		}
	}

	// Ask for result file name if it's not already given
	if ((*output).empty())
	{
		cout << "Give results file path" << endl;
		cin >> *output;
	}

	return dataset;
}

vector <Curve*> struct_initialization_grid(string file, int* max_d, int* min_d){

	ifstream data;

	vector <Curve *> data_vector;

	data.open(file);

	string line;
	int i = 0;
	int cur_d;
	if (data.is_open())
	{
		while ( getline (data, line) )
		{
			int pos1, pos2;
			string sub;

			pos2 = line.find("\t");
			sub = line.substr(0, pos2);
			string id = sub;

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
			if ( max_d!=NULL && min_d!=NULL ){
				cur_d = data_vector[i]->get_length();
				if ( i==0 || cur_d > (*max_d)){
					(*max_d) = cur_d;
				}
				if ( i==0 || cur_d < (*min_d) )
					(*min_d) = cur_d;
			}

			i++;
		}
		data.close();
	}

	return data_vector;

}

vector <Curve*> struct_initialization_projection(string file)
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

void update_output_curves(string* output, string curve_id, NN* found_nearest_neighbor, NN* true_nearest_neighbor,/*int duration, int duration_brute_force,*/ string method, string hashfunction){

	string query = "Query: "+curve_id+"\nMethod: "+method+"\nHashFunction: "+hashfunction+"\nFound Nearest neighbor: "+found_nearest_neighbor->get_id()+"\nTrue Nearest neighbor: "+true_nearest_neighbor->get_id()
					+"\ndistanceFound: "+to_string(found_nearest_neighbor->get_distance())+"\ndistanceTrue: "+to_string(true_nearest_neighbor->get_distance());
					//+"\ntHypercube: "+to_string(duration_hypercube)+"\ntTrue: "+to_string(duration_brute_force)+"\n\n";
	(*output) = (*output) + query;
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