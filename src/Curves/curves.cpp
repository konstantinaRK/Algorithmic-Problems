#include <iostream>
#include <fstream>

#include "DataHandling.hpp"

using namespace std;

int main(int argc, char * argv[]){
	
	fstream queries;
	ofstream output;

	if (file_handling(argc, argv, &queries, &output))
	{
		return errno;
	}

	return errno;
}