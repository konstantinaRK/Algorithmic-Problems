#include "DataHandling.hpp"

using namespace std;

void Curve::add_point(double x, double y){

	this->points.push_back(make_pair(x, y));
}


int file_handling(int argc, char * argv[], string * input, string * queries, string * output)
{

	if (argc == 2)
	{
		/* code */
	}

	return 1;
}

vector <Curve*> struct_initialization(string file){

	ifstream data;

	vector <Curve *> data_vector;

	data.open(file);

	string line;
	int i;
	if (data.is_open())
	{
		while ( getline (data, line) )
		{
			data_vector.push_back(new Curve());

			// Find coordinate x
			int pos1 = line.find("("); 
			int pos2 = line.find(","); 
			string sub = line.substr(pos1 + 1, pos2 - pos1 - 1); 
 
cout << "String is: " << sub << endl;;

			// Move line
			line = line.substr(pos2 + 2);

			// Find coordinate y
			pos2 = line.find(")");
  
			sub = line.substr(0, pos2); 
cout << "String is: " << sub << endl;

			// Move line
			line = line.substr(pos2 + 1);
cout << line << endl;

			// data_vector.at(i).add_point()

			i++;
			break;
		}
		data.close();
	}

	cout << data_vector.size() << endl;

	return data_vector;

}