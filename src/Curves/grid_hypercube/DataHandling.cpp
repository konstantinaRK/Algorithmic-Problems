#include "DataHandling.hpp"

using namespace std;

vector <Curve *> file_handling(int argc, char * argv[], string * queries, string * output, int * K, int * M, int * probes, int * L)
{
	string input = "";
	*queries = "";
	*output = "";

	*L = 4;

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
			*M = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-probes") == 0)
		{
			*probes = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-L_grid") == 0)
		{
			*L = atoi(argv[i+1]);
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			*output = argv[i+1];
		}
	}

	if (input.empty())
	{
		cout << "Give input file path." << endl;
		cin >> input;
	}
	if ((*queries).empty())
	{
		cout << "Give queries file path." << endl;
		cin >> *queries;
	}

	vector <Curve*> dataset = struct_initialization(input);

	if ((*output).empty())
	{
		cout << "Give results file path" << endl;
		cin >> *output;
	}

	return dataset;
}

vector <Curve*> struct_initialization(string file){

	ifstream data;

	vector <Curve *> data_vector;

	data.open(file);

	string line;
	int i = 0;
	if (data.is_open())
	{
		while ( getline (data, line) )
		{
			int pos1, pos2;
			string sub;

			pos2 = line.find("\t");
			sub = line.substr(0, pos2);
			int id = stoi(sub);

			data_vector.push_back(new Curve(id));

			double x,y;
			while (!line.empty())
			{
				// Find coordinate x
				pos1 = line.find("("); 
				pos2 = line.find(","); 
				sub = line.substr(pos1 + 1, pos2 - pos1 - 1); 
				x = stod(sub);

				// Move line
				line = line.substr(pos2 + 2);

				// Find coordinate y
				pos2 = line.find(")");
				sub = line.substr(0, pos2); 
				y = stod(sub);

				// Move line
				line = line.substr(pos2 + 1);

				data_vector.at(i)->add_point(x, y);
			}

			i++;
		}
		data.close();
	}

	return data_vector;

}