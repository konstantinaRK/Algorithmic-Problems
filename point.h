#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <unistd.h> 

using namespace std;

class Point{
		string id;
		vector<int> X;
	public:
		Point(string id, vector<int> X);
		inline string get_id(void) { return this->id; }
		inline int get_dimension(void) { return this->X.size(); }
		int operator[](int i);
};
