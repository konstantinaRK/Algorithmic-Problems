#include "DataHandling.hpp"
#include "../Projection.hpp"

using namespace std;

vector <Curve *> file_handling(int argc, char * argv[], string * queries, string * output, unsigned int * K, unsigned int * M_hyper, unsigned int * probes, double * e, unsigned int *M)
{
	string input = "";
	*queries = "";
	*output = "";

	*e = 0.5;
	*M_hyper = 10;
	*K = 3;
	*probes = 2;

	*M = 0;

	if (argc % 2 != 1)
	{
		cerr << "Invalid arguments" << endl;
		exit(1);
	}

	for (int i = 1; i < argc; i+=2)
	{
		if (strcmp(argv[i], "-d") == 0)
		{
			input = argv[i+1];
		}
		else if (strcmp(argv[i], "-q") == 0)
		{
			*queries = argv[i+1];
		}
		else if (strcmp(argv[i], "-k_hypercube") == 0)
		{
			*K = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-M") == 0)
		{
			*M_hyper = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-probes") == 0)
		{
			*probes = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-e") == 0)
		{
			*e = atof(argv[i+1]);
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			*output = argv[i+1];
		}
	}

	// Ask for input file name if it's not already given
	if (input.empty())
	{
		cout << "Give input file path." << endl;
		cin >> input;
	}
	// Ask for query file name if it's not already given
	if ((*queries).empty())
	{
		cout << "Give queries file path." << endl;
		cin >> *queries;
	}

	// Initialize dataset
	vector <Curve*> dataset = struct_initialization(input);

	// Calculate M for MM_matrix
	for (int i = 0; i < dataset.size(); i++)
	{
		if ((*M) < dataset.at(i)->get_length())
		{
			(*M) = dataset.at(i)->get_length();
		}
	}

	// Ask for result file name if it's not already given
	if ((*output).empty())
	{
		cout << "Give results file path" << endl;
		cin >> *output;
	}

	return dataset;
}