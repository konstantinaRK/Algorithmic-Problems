#include "DataHandling.hpp"
#include "../Curve.hpp"

using namespace std;

vector <Curve *> file_handling(int argc, char * argv[], string * queries, string * output, int * K, int * L, int* max_d, int * min_d)
{
	string input = "";
	*queries = "";
	*output = "";

	*L = 4;
	*K = 3;

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
		else if (strcmp(argv[i], "-k_vec") == 0)
		{
			*K = atoi(argv[i+1]);
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

	vector <Curve*> dataset = struct_initialization(input, max_d, min_d);

	if ((*output).empty())
	{
		cout << "Give results file path" << endl;
		cin >> *output;
	}

	return dataset;
}

vector <Curve*> struct_initialization(string file, int * max_d, int * min_d){

	ifstream data;

	vector <Curve *> data_vector;

	data.open(file);

	string line;
	int i = 0;
	int cur_d;
	if (data.is_open())
	{
		while ( getline (data, line) && line.length() > 0 )
		{
			int pos1, pos2;
			string sub;

			pos2 = line.find("\t");
			sub = line.substr(0, pos2);
			string id = sub;

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
			if ( max_d!=NULL && min_d!=NULL ){
				cur_d = data_vector[i]->get_length();
				if ( i==0 || cur_d > (*max_d)){
					(*max_d) = cur_d;
				}
				if ( i==0 || cur_d < (*min_d) )
					(*min_d) = cur_d;
			}
			i++;
		}
		data.close();
	}

	return data_vector;

}