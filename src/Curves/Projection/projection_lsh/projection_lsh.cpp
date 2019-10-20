#include "DataHandling.hpp"
#include "../Projection.hpp"
#include "../../utilities.hpp"

#include <unistd.h>
#include <stdio.h>
#include <limits.h>

using namespace std::chrono;

int main(int argc, char * argv[]){

	string query_file, output_file;
	unsigned int K, L, M;
	double e;

	// Read dataset
	vector <Curve*> dataset = file_handling(argc, argv, &query_file, &output_file, &K, &L, &e, &M);

	// Create G matrix
/*!!!!!!!!!!*/
	int d_dim = 5;
	int k_dim = (-1) * log2(e)/e/e;
	double **G = createG(d_dim, k_dim);

	// Fill Matrix M*M
	IJ_Traversals ** MM_table = train(dataset, M, G);

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

	// Read and execute queries
	double average_af = 0;
	double max_af;
	double time_af = 0;

	bool stop = false;
	while (!stop)
	{
		string output = "";
		// Read the queries file
		vector<Curve*> queries = struct_initialization(query_file);
cout << "End of reading queries" << endl;

		for (int i = 0; i < queries.size(); ++i)
		{
			// Brute force
			auto start = high_resolution_clock::now();
			// NN* true_nearest_neighbor = brute_force(queries[i], &pointset);
		    auto stop = high_resolution_clock::now();

		  //   if ( true_nearest_neighbor == NULL )
		  //   {
		  //   	for (int i = 0; i < M; i++)
				// {
				// 	delete [] MM_table[i];
				// }
				// delete [] MM_table;
				// delete_vector<Curve>(&dataset);
				// cerr << "no neighbor found" << endl;
				// return 1;
		  //   }

		    auto duration_brute_force = duration_cast<microseconds>(stop - start); 
			// cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << true_nearest_neighbor->get_id() << ", " << true_nearest_neighbor->get_distance() << endl; 
			// cout << "time : " << duration_brute_force.count() << endl;

			// LSH
			start = high_resolution_clock::now();
			// NN* lsh_nearest_neighbor = lsh(queries[i], &g, &hash_tables, w);
		    stop = high_resolution_clock::now();

		   	// if ( lsh_nearest_neighbor == NULL )
		    // {
		    	// delete true_nearest_neighbor;
				// continue;
		    // }
		    auto duration_lsh = duration_cast<microseconds>(stop - start); 
		    // cout << "nearest_neighbor of "<< queries[i]->get_id() << " is "  << lsh_nearest_neighbor->get_id() << ", " << lsh_nearest_neighbor->get_distance() << endl; 
			// cout << "time : " << duration_lsh.count() << endl << endl;
		
			// Store the result of a query
			// update_output_lsh(&output, queries[i]->get_id(), lsh_nearest_neighbor, true_nearest_neighbor, duration_lsh.count(), duration_brute_force.count());

			// double af = lsh_nearest_neighbor->get_distance()/true_nearest_neighbor->get_distance();
			// double time = duration_lsh.count()/duration_brute_force.count();
			// if ( average_af == 0 )
			// {
			// 	average_af = af;
			// 	max_af = af;
			// 	time_af = time;
			// }
			// else{
			// 	if (max_af < af)
			// 		max_af = af;
			// 	average_af += af;
			// 	time_af += time;
			// }

		 //    delete true_nearest_neighbor;
		 //    delete lsh_nearest_neighbor;
		}

		// average_af = average_af/queries.size();
		// time_af = time_af/queries.size();

// cout << "max_af is " << max_af << " and average af is " << average_af << " time comp. is : " << time_af << endl;

		// Store output in output_file
		// if (!write_output(output_file, output)){
			// for (int i = 0; i < M; i++)
				// {
				// 	delete [] MM_table[i];
				// }
				// delete [] MM_table;
		// 	delete_vector<Curve>(&queries);
		// 	delete_vector<Curve>(&dataset);
		// 	return 1;
		// }

		// Check for a new queries file
		string answer;
		bool right_answer = false;
		while ( !right_answer ){
			cout << "Do you want to search the nearest neighbors in a new query file? y or n" << endl;
			cin >> answer;

			if ( (answer.compare("y") == 0) || (answer.compare("yes") == 0) ){
				right_answer = true;
				cout << "Insert the name of the queries file" << endl;
				cin >> query_file;

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
		delete_vector<Curve>(&queries);
	}

	// Free data memory
	for (int i = 0; i < M; i++)
	{
		delete [] MM_table[i];
	}
	delete [] MM_table;

	delete_vector<Curve>(&dataset);

	return 0;
}