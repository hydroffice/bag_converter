#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include "helper.h"
#include "converter.h"

using namespace std;

bool skip_pause = false;

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		cerr << "Usage: " << argv[0] << " in_path out_path" << endl;
		bag::msvc_pause(skip_pause);
		return -1;
	}
	try
	{
		std::string in_path = argv[1];
		cout << "Input: " << in_path << endl;;
		std::string out_path = argv[2];
		cout << "Output: " << out_path << endl;
		std::string configdata_path = bag::join_path( bag::cwd(), "configdata" );
		cout << "configdata: " << configdata_path << endl;
		if(!bag::path_exists(configdata_path))
		{
			cerr << "Unable to locate the configdata folder: " << configdata_path << endl;
			bag::msvc_pause(skip_pause);
			return -1;
		}
		std::string metadata_path = bag::join_path( bag::cwd(), "metadata" );
		cout << "metadata: " << metadata_path << endl;
		if(!bag::path_exists( metadata_path ))
		{
			cerr << "Unable to locate the metadata folder: " << metadata_path << endl;
			bag::msvc_pause(skip_pause);
			return -1;
		}

		bag::Converter converter;
		bool success = converter.convert(in_path, out_path, configdata_path, metadata_path );
		cout << "Converted: " << std::boolalpha << success << endl;
	}
	catch(const exception& e)
	{
		cout << e.what() << endl;
	}

	bag::msvc_pause(skip_pause);
}
