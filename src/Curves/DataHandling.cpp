#include "DataHandling.hpp"

using namespace std;

Curve::Curve(int id){
	this->curve_id = id;
}

void Curve::add_point(double x, double y){

	this->points.push_back(make_pair(x, y));
}

void Curve::clear(void){

	this->points.clear();
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