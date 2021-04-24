#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <boost/filesystem.hpp>

using namespace std;

int main()
{
	for(boost::filesystem::directory_iterator it("./"), end; it != end; ++it)
	{
		if(it->path().extension() == ".txt")
		{
			stringstream strstream;
			strstream << *it;
			string filename = strstream.str();
			cout << "name: " << filename << endl;
			double eps, P;
			cout << "enter eps, p:" << endl;
			cin >> eps >> P;
			double a, b, c, d, hx, hy;
			ifstream file;
			file.open(filename.c_str(), ios::in);
			unsigned long int count = 0;
			while (!file.eof() && file.good())
			{
				if(file.eof()) break;
				if(!file.good()) break;
				file >> a >> b >> c >> d >> hx >> hy;
				if(file.eof()) break;
				if(!file.good()) break;
				double P_eps = eps*eps / (hx * hy);
				unsigned long int tests_N = (double)(log(1.0 - P) / log(1.0 - P_eps));
				count += tests_N;
			}
			cout << "numbers = " << count << endl;
			file.close();
		}
	}
	getchar();
	return 0;
}

