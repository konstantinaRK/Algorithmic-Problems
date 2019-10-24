#include "./lsh_functions.hpp"
#include "../DataHandling.hpp"

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]){

	// Check validity of arguments
	int k, L;
	string dataset_file = "";
	string queries_file = "";
	string output_file = "";

	if ( !check_arguments_lsh(argc, argv, &dataset_file, &queries_file, &k, &L, &output_file) )
		return 1;

	int r = 0;
	// Read the dataset file
	vector<Point*> pointset;
	if ( !read(dataset_file, &pointset, &r) ){
		delete_vector<Point>(&pointset);
		return 1;
	}
	cout << "End of reading input" << endl;

	int dimension = pointset[0]->get_dimension();

	// Create LSH ANN
	LSH* lsh;
	try{
		lsh = new LSH(&pointset, L, k, dimension);}
	catch(bad_alloc&){
		cerr << "main: No memory_available" << endl;
		return 1;
	}
		
	// Initialize metrics
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

		// Check the queries
		for (int i = 0; i < queries.size(); ++i)
		{
cout << queries[i]->get_id() << endl;

			// Brute force
			auto start = high_resolution_clock::now();
			NN* true_nearest_neighbor = brute_force(queries[i], &pointset);
		    auto stop = high_resolution_clock::now();

		    if ( true_nearest_neighbor == NULL )
		    {
				delete lsh;
		    	delete_vector<Point>(&queries);
				cerr << "no neighbor found" << endl;
				return 1;
		    }

		    auto duration_brute_force = duration_cast<microseconds>(stop - start); 
			cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << true_nearest_neighbor->get_id() << ", " << true_nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_brute_force.count() << endl;

			// LSH
			start = high_resolution_clock::now();
			NN* lsh_nearest_neighbor = lsh->predict(queries[i], r);
		    stop = high_resolution_clock::now();

		   	if ( lsh_nearest_neighbor == NULL )
		    {
		    	delete true_nearest_neighbor;
		    	continue;
		    }
		    auto duration_lsh = duration_cast<microseconds>(stop - start); 
		    cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << lsh_nearest_neighbor->get_id() << ", " << lsh_nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_lsh.count() << endl << endl;
		
			// Store the result of a query
			update_output_lsh(&output, queries[i]->get_id(), lsh_nearest_neighbor, true_nearest_neighbor, duration_lsh.count(), duration_brute_force.count());

			// Update metrics
			double af = (double)lsh_nearest_neighbor->get_distance()/true_nearest_neighbor->get_distance();
			double time = (double)duration_lsh.count()/duration_brute_force.count();
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
		    delete lsh_nearest_neighbor;
		}

		average_af = average_af/queries.size();
		time_af = time_af/queries.size();

cout << "max_af is " << max_af << " and average af is " << average_af << " time comp. is : " << time_af << endl;

		// Store output in output_file
		if (!write_output(output_file, output)){
			delete_vector<Point>(&queries);
			delete lsh;
			return 1;
		}

		// Check for a new queries file
		stop = !check_for_new_queries(&queries_file, &output_file);
		delete_vector<Point>(&queries);
	}

	delete lsh;
}