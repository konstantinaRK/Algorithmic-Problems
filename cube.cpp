#include "utilities.h"

using namespace std;

int main(int argc, char* argv[]){

	// Check validity of arguments
	int k, M, probes;
	string dataset_file = "";
	string queries_file = "";
	string output_file = "";

	if ( !check_arguments_cube(argc, argv, &dataset_file, &queries_file, &k, &M, &probes, &output_file) )
	{
		cerr << "Usage ./cube -d <input_file> -q <query_file> -k <int> -M <int> -p <int> -o <output_file>" << endl;
		return 1;
	}

	// Read the dataset file
	vector<Point*> pointset;
	if ( !read(dataset_file, &pointset) ){
		delete_vector<Point>(&pointset);
		return 1;
	}
	
	// Create the hash tables

	bool stop = false;
	while (!stop)
	{

		// Check the queries

			// Print the result of a query

		// Check for a new queries file
		string answer;
		cout << "Do you want to search the nearest neighbors in a new query file? y or n" << endl;
		cin >> answer;

		if ( (answer.compare("y") == 0) || (answer.compare("yes") == 0) ){
			cout << "Insert the name of the queries file" << endl;
			cin >> queries_file;
		}
		else if ( (answer.compare("n") == 0) || (answer.compare("no") == 0) )
			stop = true;
		else
			cout << "Invalid answer try again" << endl;
	}

	delete_vector<Point>(&pointset);
}