#include "./hypercube_functions.hpp"
#include "../DataHandling.hpp"

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]){

	// Check validity of arguments
	unsigned int k, M, probes;
	string dataset_file = "";
	string queries_file = "";
	string output_file = "";

	if ( !check_arguments_cube(argc, argv, &dataset_file, &queries_file, &k, &M, &probes, &output_file) )
	{
		cerr << "Usage ./cube -d <input_file> -q <query_file> -k <int> -M <int> -p <int> -o <output_file>" << endl;
		return 1;
	}

	int r = 0;

	// Read the dataset file
	vector<Point*> pointset;
	if ( !read(dataset_file, &pointset, &r) ){
		delete_vector<Point>(&pointset);
		return 1;
	}
	cout << "End of reading input" << endl;

	// Calculating default k = logn
	if (k == 0)
	{
		k = log2(pointset.size());
	}

	// Creating hypercube structure
	Hypercube* hypercube;
	try{
		hypercube = new Hypercube(&pointset, pointset[0]->get_dimension(), k, M, probes);
	}
	catch(bad_alloc&){
		delete_vector<Point>(&pointset);
		return 1;
	}

	// Variable used for statistics
	double average_af = 0;
	double max_af;
	double time_af = 0;
		
	// Check queries
	bool stop = false;
	while (!stop)
	{
		string output = "";

		// Read the queries file
		vector<Point*> queries;
		if ( !read(queries_file, &queries) )
		{
			delete_vector<Point>(&queries);
			delete hypercube;
			return 1;
		}
		cout << "End of reading queries" << endl;

		// Find queries nearest neighbor with brute force and hypercube
		for (int i = 0; i < queries.size(); ++i)
		{

			// Brute force
			auto start = high_resolution_clock::now();
			NN* true_nearest_neighbor = brute_force(queries[i], &pointset);
			auto stop = high_resolution_clock::now();

			if ( true_nearest_neighbor == NULL )
			{
				delete_vector<Point>(&queries);
				delete hypercube;
				return 1;
			}

			auto duration_brute_force = duration_cast<microseconds>(stop - start); 
			cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << true_nearest_neighbor->get_id() << ", " << true_nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_brute_force.count() << endl;

			// Hypercube neighbor calculation
			start = high_resolution_clock::now();
			NN * hypercube_neighbor = hypercube->predict(queries[i], r);
			stop = high_resolution_clock::now();

			if ( hypercube_neighbor == NULL )
			{
				delete true_nearest_neighbor;
				delete_vector<Point>(&queries);
				delete hypercube;
				return 1;
			}

			auto duration_hypercube = duration_cast<microseconds>(stop - start); 
			cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << hypercube_neighbor->get_id() << ", " << hypercube_neighbor->get_distance() << endl; 
			cout << "time : " << duration_hypercube.count() << endl << endl;
		
			// Store the result of a query
			update_output_cube(&output, queries[i]->get_id(), hypercube_neighbor, true_nearest_neighbor, duration_hypercube.count(), duration_brute_force.count());

			// Statistics calculation
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
			delete hypercube;
			return 1;
		}

		// Check for a new queries file
		stop = !check_for_new_queries(&queries_file, &output_file);
		delete_vector<Point>(&queries);
	}

	delete hypercube;
}