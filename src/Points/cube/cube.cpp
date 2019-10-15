// #include "../utilities.h"
#include "./hypercube.hpp"

using namespace std;
using namespace std::chrono;

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
	cout << "End of reading input" << endl;

	int w = 4*average_distance(&pointset);
	int m = 3;	// πρεπει να βαλω ενα νουμερο


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

	// Create the hypercube
	unordered_map<int, vector<Point*>> hypercube;

	// Create F function
	F F_g(pointset[0]->get_dimension(), m, w, 4);


	// Fill the hash table with the pointset
	int key;
	for (int j = 0; j < pointset.size(); j++)
	{
			key = F_g.calc_F(pointset[j]);
			if ( hypercube.find(key) == hypercube.end() )
			{
				// Insert the key
				vector<Point*> p;
				p.push_back(pointset[j]);
				hypercube.insert(make_pair(key, p));
			}
			else
			{
				hypercube.at(key).push_back(pointset[j]);
			}
	}

cout << "filled hash tables" << endl;
// F_g.print();
// getchar();
// cout << hypercube.size() << endl;

	double average_af = 0;
	double max_af;
	double time_af = 0;
		
	bool stop = false;
	while (!stop)
	{

		string output = "";

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
			NN* true_nearest_neighbor = brute_force(queries[i], &pointset);
			auto stop = high_resolution_clock::now();

			if ( true_nearest_neighbor == NULL )
			{
				delete_vector<Point>(&queries);
				delete_vector<Point>(&pointset);
				return 1;
			}

			auto duration_brute_force = duration_cast<microseconds>(stop - start); 
			cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << true_nearest_neighbor->get_id() << ", " << true_nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_brute_force.count() << endl;

// 			// Hypercube
			start = high_resolution_clock::now();
			NN * hypercube_neighbor = hypercube_calc(queries[i], &(F_g), &hypercube, probes);
			stop = high_resolution_clock::now();

			if ( hypercube_neighbor == NULL )
			{
				delete true_nearest_neighbor;
				delete_vector<Point>(&queries);
				delete_vector<Point>(&pointset);
				return 1;
			}

			auto duration_hypercube = duration_cast<microseconds>(stop - start); 
			cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << hypercube_neighbor->get_id() << ", " << hypercube_neighbor->get_distance() << endl; 
			cout << "time : " << duration_hypercube.count() << endl << endl;
		
			// Store the result of a query
			update_output_cube(&output, queries[i]->get_id(), hypercube_neighbor, true_nearest_neighbor, duration_hypercube.count(), duration_brute_force.count());

			double af = (double) hypercube_neighbor->get_distance()/true_nearest_neighbor->get_distance();
			double time = (double) duration_hypercube.count()/duration_brute_force.count();

			if ( average_af == 0 )
			{
				average_af = af;
				max_af = af;
				time_af = time;
			}
			else{
				if (max_af < af)
					max_af = af;
				average_af += af;
				time_af += time;
			}


			delete true_nearest_neighbor;
			delete hypercube_neighbor;
		}

		average_af = average_af/queries.size();
		time_af = time_af/queries.size();

cout << "max_af is " << max_af << " and average af is " << average_af << " time comp. is : " << time_af << endl;


		// Store output in output_file
		if (!write_output(output_file, output)){
			delete_vector<Point>(&queries);
			delete_vector<Point>(&pointset);
			return 1;
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

				cout << "Insert the name of the output_file" << endl;
				cin >> output_file;
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

	delete_vector<Point>(&pointset);
}