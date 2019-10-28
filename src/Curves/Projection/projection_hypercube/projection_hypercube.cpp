#include "../../../../include/Projection.hpp"
#include "../../../../include/DataHandling.hpp"

#include <unistd.h>
#include <stdio.h>
#include <limits.h>

using namespace std;
using namespace std::chrono;

int main(int argc, char * argv[]){
	
	string query_file, output_file;
	unsigned int K, M_hyper, probes, M;
	double e;

	// Read dataset
	vector <Curve*> dataset = file_handling_projection_hypercube(argc, argv, &query_file, &output_file, &K, &M_hyper, &probes, &e, &M);
	if (dataset.empty())
	{
		cerr << "Could not find data!" << endl;
		return -1;
	}

	cout << "End of reading" << endl;
	
	// Create G matrix
	int d_dim = 2;
	int k_dim = (-1) * d_dim * log2(e)/e/e;
	double **G = createG(k_dim, d_dim);

	// Fill Matrix M*M
	IJ_Cell *** MM_table = train_Hypercube(dataset, M, G, d_dim, k_dim, M_hyper, K, probes);
	if (MM_table == NULL)
	{
		for (int i = 0; i < dataset.size(); i++)
		{
			dataset.at(i)->clear();
			delete dataset.at(i);
		}
		dataset.clear();
		return 1;
	}

	cout << "End of training" << endl;

	// Variable for statistics
	int counter = 0 /* Nearest neighbor not found */, counter_true = 0 /* Same neighbor with brute force found */;
	double average_af = 0;
	double max_af;
	double time_af = 0;

	// Read and execute queries
	bool stop = false;
	while (!stop)
	{
		string output = "";

		// Read the queries file
		vector<Curve*> queries = struct_initialization_projection(query_file);
		if (queries.empty())
		{
			cerr << "Could not find queries!" << endl;
			stop = true;
			break;
		}

		cout << "End of reading queries" << endl;

		// For all queries find the nearest neighbor with brute force and then with hypercube
		for (int i = 0; i < queries.size(); ++i)
		{
			// Brute force
			auto start = high_resolution_clock::now();
			NN* true_nearest_neighbor = curves_brute_force(queries[i], &dataset);
		    auto stop = high_resolution_clock::now();

		    if ( true_nearest_neighbor == NULL )
		    {
		    	for (int i = 0; i < M; i++)
				{
					delete [] MM_table[i];
				}
				delete [] MM_table;
				delete_vector<Curve>(&dataset);
				cerr << "no neighbor found" << endl;
				return 1;
		    }

		    auto duration_brute_force = duration_cast<microseconds>(stop - start); 
			cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << true_nearest_neighbor->get_id() << ", " << true_nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_brute_force.count() << endl;

			// Hypercube calculation
			start = high_resolution_clock::now();
			NN* hypercube_nearest_neighbor = predict(MM_table, M, dataset, queries[i], G, d_dim, k_dim);
		    stop = high_resolution_clock::now();

		   	if ( hypercube_nearest_neighbor == NULL )
		    {
				counter++;
		    	delete true_nearest_neighbor;
				continue;
		    }
		    auto duration_hypercube = duration_cast<microseconds>(stop - start); 
		    cout << "hypercube_nearest_neighbor of "<< queries[i]->get_id() << " is "  << hypercube_nearest_neighbor->get_id() << ", " << hypercube_nearest_neighbor->get_distance() << endl; 
			cout << "time : " << duration_hypercube.count() << endl << endl;
		
			// Store the result of a query
			update_output_curves(&output, queries[i]->get_id(), hypercube_nearest_neighbor, true_nearest_neighbor, "Projection", "Hypercube");
		
			if (hypercube_nearest_neighbor->get_id().compare(true_nearest_neighbor->get_id()) == 0)
			{
				counter_true++;
			}
		
			// Calculating average and max values
			double af = (hypercube_nearest_neighbor->get_distance()+1)/(true_nearest_neighbor->get_distance()+1);
			double time = duration_hypercube.count()/duration_brute_force.count();
			if ( average_af == 0 )
			{
				average_af = af;
				max_af = af;
				time_af = time;
			}
			else
			{
				if (max_af < af)
					max_af = af;
				average_af += af;
				time_af += time;
			}

		    delete true_nearest_neighbor;
		    true_nearest_neighbor = NULL;
		    delete hypercube_nearest_neighbor;
		    hypercube_nearest_neighbor = NULL;
		}

		average_af = average_af/(queries.size() - counter);
		time_af = time_af/(queries.size() - counter);

		cout << "max_af is " << max_af << " and average af is " << average_af << " time comp. is : " << time_af << endl;
		cout << "not found queries = " << counter << " of " << queries.size() << endl;
		cout << "same curve = " << counter_true << endl;
		
		// Store output in output_file
		if (!write_output(output_file, output)){
			for (int i = 0; i < M; i++)
				{
					delete [] MM_table[i];
				}
				delete [] MM_table;
			delete_vector<Curve>(&queries);
			delete_vector<Curve>(&dataset);
			return 1;
		}

		// Check for a new queries file
		stop = !check_for_new_queries(&query_file, &output_file);
		delete_vector<Curve>(&queries);
	}

	// Free data memory
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < M; j++)
		{
			delete MM_table[i][j];
		}
		delete [] MM_table[i];
	}
	delete [] MM_table;

	for (int i = 0; i < k_dim ; i++)
	{
		delete [] G[i];
	}
	delete [] G;

	delete_vector<Curve>(&dataset);

	return 0;
}