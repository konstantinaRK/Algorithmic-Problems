#include "lsh_functions.h"
#include <chrono> 

using namespace std;
using namespace std::chrono;

// οταν δημιουργω τα g πρεπει να αλλαξω λιγο τα exceptions

int main(int argc, char* argv[]){

	// Check validity of arguments
	int k, L;
	string dataset_file = "";
	string queries_file = "";
	string output_file = "";

	if ( !check_arguments_lsh(argc, argv, &dataset_file, &queries_file, &k, &L, &output_file) )
	{
		cerr << "Usage ./lsh -d <input_file> -q <query_file> -k <int> -L <int> -o <output_file>" << endl;
		return 1;
	}

	// Read the dataset file
	vector<Point*> pointset;
	if ( !read(dataset_file, &pointset) ){
		delete_vector<Point>(&pointset);
		return 1;
	}
	cout << "End of reading input" << endl;

	// int w = average_distance(&pointset);
	int w = 10;

	int m = 20;	// πρεπει να βαλω ενα νουμερο

	// Create the hash tables
	vector<unordered_map<int, vector<Point*>>> hash_tables;
	for (int i = 0; i < L; ++i)
	{
		unordered_map<int, vector<Point*>> hash_table;
		hash_tables.push_back(hash_table);
	}

	// Create L g functions
	vector<G*> g;
	for (int i = 0; i < L; ++i)
	{
		try{
			g.push_back(new G(k, pointset[0]->get_dimension(), m, w));
		}
		catch(bad_alloc&){
			delete_vector<G>(&g);
			delete_vector<Point>(&pointset);
			cerr << "No memory available" << endl;
			return 1;
		}
	}

cout << "g functions created " << endl;

	// Fill the hash table with the pointset
	int key;
	for (int j = 0; j < pointset.size(); ++j)
	{
		for (int i = 0; i < L; ++i)
		{
			key = (*g[i])[pointset[j]];
			if ( hash_tables[i].find(key) == hash_tables[i].end() )
			{
				// Insert the key
				vector<Point*> p;
				p.push_back(pointset[j]);
				hash_tables[i].insert(make_pair(key, p));
			}
			else
				hash_tables[i].at(key).push_back(pointset[j]);
		}		
	}
cout << "filled hash tables" << endl;
// print_hash_tables(&hash_tables);
		
	bool stop = false;
	while (!stop)
	{
		// Read the queries file
		vector<Point*> queries;
		if ( !read(queries_file, &queries) )
		{
			delete_vector<Point>(&queries);
			return 1;
		}
		cout << "End of reading queries" << endl;

		// print_points(queries);

		// Check the queries
		for (int i = 0; i < queries.size(); ++i)
		{

			// Brute force
			auto start = high_resolution_clock::now();
			NN* nearest_neighbor = brute_force(queries[i], &pointset);
		    auto stop = high_resolution_clock::now();

		    if ( nearest_neighbor == NULL )
		    {
		    	delete_vector<G>(&g);
		    	delete_vector<Point>(&queries);
				delete_vector<Point>(&pointset);
				return 1;
		    }

		    auto duration_brute_force = duration_cast<microseconds>(stop - start); 
			cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << nearest_neighbor->get_id() << ", " << nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_brute_force.count() << endl;

		    delete nearest_neighbor;

			// LSH
			start = high_resolution_clock::now();
			nearest_neighbor = lsh(queries[i], &g, &hash_tables, w);
		    stop = high_resolution_clock::now();

		   	if ( nearest_neighbor == NULL )
		    {
		    	delete_vector<G>(&g);
		    	delete_vector<Point>(&queries);
				delete_vector<Point>(&pointset);
				return 1;
		    }
		    auto duration_lsh = duration_cast<microseconds>(stop - start); 
		    cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << nearest_neighbor->get_id() << ", " << nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_lsh.count() << endl << endl;
		
		    delete nearest_neighbor;

			// Store the result of a query
		}

		// Check for a new queries file
		string answer;
		bool right_answer = false;
		while ( !right_answer ){
			cout << "Do you want to search the nearest neighbors in a new query file? y or n" << endl;
			cin >> answer;

			if ( (answer.compare("y") == 0) || (answer.compare("yes") == 0) ){
				right_answer = true;
				cout << "Insert the name of the queries file" << endl;
				cin >> queries_file;
			}
			else if ( (answer.compare("n") == 0) || (answer.compare("no") == 0) ){
				right_answer = true;
				stop = true;
			}
			else{
				right_answer = false;
				cout << "Invalid answer try again" << endl;
			}
		}
		delete_vector<Point>(&queries);
	}

	delete_vector<G>(&g);
	delete_vector<Point>(&pointset);
}