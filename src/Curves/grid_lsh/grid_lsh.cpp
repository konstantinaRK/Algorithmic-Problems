#include "DataHandling.hpp"

#include <unistd.h>
#include <stdio.h>
#include <limits.h>

int main(int argc, char * argv[]){
	
	string query_file, output_file;
	int K, L;

	vector <Curve*> dataset = file_handling(argc, argv, &query_file, &output_file, &K, &L);

	// Free data memory
	for (int i = 0; i < dataset.size(); i++)
	{
		dataset.at(i)->clear();
		delete dataset.at(i);
	}
	dataset.clear();

	return 0;
}