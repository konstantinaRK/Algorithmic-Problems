#include "../../../../include/utilities.hpp"
#include "../../../../include/DataHandling.hpp"
#include "../../../../include/Grid.hpp"

using namespace std;
using namespace std::chrono;

int main(int argc, char * argv[]){
	
	string queries_file, output_file;
	unsigned int k_hypercube, L_grid, M, probes;
	int max_d, min_d;

	vector <Curve*> dataset = file_handling_grid_hypercube(argc, argv, &queries_file, &output_file, &k_hypercube, &M, &probes, &L_grid, &max_d, &min_d);
	if (dataset.empty())
	{
		cerr << "Could not find data!" << endl;
		return -1;
	}

	cout << "End of reading" << endl;


	// Initialize metrics
	double average_af = 0;
	double max_af;
	double time_af = 0;


	Grid_Hypercube* grid_hypercube;
	try{
		grid_hypercube = new Grid_Hypercube(&dataset, k_hypercube, M, probes, L_grid, max_d, min_d);
	}
	catch(bad_alloc&){
		cerr << "main: No memory available" << endl;
		return 1;
	}

	bool stop = false;
	while (!stop)
	{
		string output = "";

		// Read queries file
		vector<Curve*> queries = struct_initialization_grid(queries_file);
		if (queries.empty())
		{
			cerr << "Could not find queries!" << endl;
			stop = true;
			break;
		}
		cout << "End of reading queries" << endl;

		// Find nearest neighbors of queries
		for (int i = 0; i < queries.size(); ++i)
		{
			// Brute force
			auto start = high_resolution_clock::now();
			NN* true_nearest_neighbor = curves_brute_force(queries[i], &dataset);
		    auto stop = high_resolution_clock::now();

		    if ( true_nearest_neighbor == NULL )
		    {
		    	delete_vector<Curve>(&queries);
		    	delete grid_hypercube;
				cerr << "no neighbor found" << endl;
				return 1;
		    }

		    auto duration_brute_force = duration_cast<microseconds>(stop - start); 
			cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << true_nearest_neighbor->get_id() << ", " << true_nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_brute_force.count() << endl;

			// Grid_hypercube
			start = high_resolution_clock::now();
			NN* grid_hypercube_nearest_neighbor = grid_hypercube->predict(queries[i], max_d);
		    stop = high_resolution_clock::now();

		   	if ( grid_hypercube_nearest_neighbor == NULL )
		    {
		    	delete true_nearest_neighbor;
				continue;
		    }
		    auto duration_lsh = duration_cast<microseconds>(stop - start); 
		    cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << grid_hypercube_nearest_neighbor->get_id() << ", " << grid_hypercube_nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_lsh.count() << endl << endl;

			update_output_curves(&output, queries[i]->get_id(), grid_hypercube_nearest_neighbor, true_nearest_neighbor, "Grid", "LSH");

			// Update metrics
			double af = (double)(grid_hypercube_nearest_neighbor->get_distance()+1)/(true_nearest_neighbor->get_distance()+1);
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
		    delete grid_hypercube_nearest_neighbor;
		}

		average_af = average_af/queries.size();
		time_af = time_af/queries.size();

		cout << "max_af is " << max_af << " and average af is " << average_af << " time comp. is : " << time_af << endl;

		// Store output in output_file
		if (!write_output(output_file, output)){
			delete_vector<Curve>(&queries);
			delete grid_hypercube;
			return 1;
		}		
		// Check for a new queries file
		stop = !check_for_new_queries(&queries_file, &output_file);
		delete_vector<Curve>(&queries);
	}

	// Free data memory
	delete grid_hypercube;
	return 0;
}