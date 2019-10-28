#include "../../../../include/DataHandling.hpp"
#include "../../../../include/Grid.hpp"

#include <chrono>

using namespace std;
using namespace std::chrono;

int main(int argc, char * argv[]){
	
	string queries_file, output_file;
	int k_vec, L_grid;
	int max_d, min_d;
	int m, w;

	vector <Curve*> dataset = file_handling_grid_lsh(argc, argv, &queries_file, &output_file, &k_vec, &L_grid, &max_d, &min_d);
	if (dataset.empty())
	{
		cerr << "Could not find data!" << endl;
		return -1;
	}

	cout << "End of reading" << endl;

	// Create grid_lsh structure
	Grid_LSH* grid_lsh;
	try{
		grid_lsh = new Grid_LSH(&dataset, L_grid, k_vec, max_d, min_d);
	}
	catch(bad_alloc&){
		cerr << "main: No memory available" << endl;
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
		vector<Curve*> queries = struct_initialization_grid(queries_file);
		if (queries.empty())
		{
			cerr << "Could not find queries!" << endl;
			stop = true;
			break;
		}
		cout << "End of reading queries" << endl;

		// Check the queries
		for (int i = 0; i < queries.size(); ++i)
		{
			// Brute force
			auto start = high_resolution_clock::now();
			NN* true_nearest_neighbor = curves_brute_force(queries[i], &dataset);
		    auto stop = high_resolution_clock::now();

		    if ( true_nearest_neighbor == NULL )
		    {
		    	delete_vector<Curve>(&queries);
		    	delete grid_lsh;
				cerr << "no neighbor found" << endl;
				return 1;
		    }

		    auto duration_brute_force = duration_cast<microseconds>(stop - start); 
			cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << true_nearest_neighbor->get_id() << ", " << true_nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_brute_force.count() << endl;

			// Grid_LSH
			start = high_resolution_clock::now();
			NN* grid_lsh_nearest_neighbor = grid_lsh->predict(queries[i],max_d);
		    stop = high_resolution_clock::now();


		   	if ( grid_lsh_nearest_neighbor == NULL )
		    {
				cout << "nearest neighbor is null" << endl << endl;
		    	delete true_nearest_neighbor;
				continue;
		    }
		    auto duration_lsh = duration_cast<microseconds>(stop - start); 
		    cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << grid_lsh_nearest_neighbor->get_id() << ", " << grid_lsh_nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_lsh.count() << endl << endl;

			// Store the result of a query
			update_output_curves(&output, queries[i]->get_id(), grid_lsh_nearest_neighbor, true_nearest_neighbor, "Grid", "LSH");

			// Update metrics
			double af = (double)(grid_lsh_nearest_neighbor->get_distance()+1)/(true_nearest_neighbor->get_distance()+1);
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
		    delete grid_lsh_nearest_neighbor;
		}

		average_af = average_af/queries.size();
		time_af = time_af/queries.size();

		cout << "max_af is " << max_af << " and average af is " << average_af << " time comp. is : " << time_af << endl;

		// Store output in output_file
		if (!write_output(output_file, output)){
			delete_vector<Curve>(&queries);
			delete grid_lsh;
			return 1;
		}

		// Check for a new queries file
		stop = !check_for_new_queries(&queries_file, &output_file);
		delete_vector<Curve>(&queries);
	}

	delete grid_lsh;
	return 0;
}