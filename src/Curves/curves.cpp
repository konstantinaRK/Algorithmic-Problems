#include "DataHandling.hpp"

#include <unistd.h>
#include <stdio.h>
#include <limits.h>

int main(int argc, char * argv[]){
	
	string input_file, query_file, output_file;

	if (!file_handling(argc, argv, &input_file, &query_file, &output_file))
	{
		return errno;
	}

input_file = "./Datasets/trajectories_dataset";

	vector <Curve*> dataset = struct_initialization(input_file);


	for (int i = 0; i < dataset.size(); i++)
	{
		dataset.at(i)->clear();
		delete dataset.at(i);
	}
	dataset.clear();

	return errno;
}