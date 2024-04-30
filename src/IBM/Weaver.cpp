/*
 * main.cpp
 *
 *  Created on: Apr 3, 2012
 *      Author: jrbcast
 */
//#define _CRTDBG_MAP_ALLOC


#include <boost/filesystem.hpp>
#include <cxxopts.hpp>
#include <string>

#include "IBM/IBM.h"
#include "Misc/GlobalVariable.h"


using namespace std;
namespace fs = boost::filesystem;



int main(int argc, char ** argv)
{
	cxxopts::Options options("Weaver", "Weaver description");

	fs::path inputConfigPath;
    options.add_options()
        ("I,inputConfig", "Input config path", cxxopts::value<fs::path>(inputConfigPath));

	
	fs::path outputFolder;
    options.add_options()
        ("O,outputFolder", "Result output folder path", cxxopts::value<fs::path>(outputFolder)->default_value(string(RESULT_SIMULATION_FOLDER)));

	
	int burnIn;
	options.add_options()
        ("optimization-burn-in", "Burn-in value", cxxopts::value<int>(burnIn)->default_value("-1"));

	bool silentMode;
	options.add_options()
        ("silent", "Run the program in silent mode, suppressing standard output", cxxopts::value<bool>(silentMode)->default_value("false"));

	options.add_options()
		("h,help", "Print usage");


	auto result = options.parse(argc, argv);

	if(result.count("help"))
	{
		cout << options.help() << endl;
		return 0;
	}

	if(!result.count("inputConfig"))
	{
		cerr << "Error: Required arguments are missing." << endl;
		cerr << "Try 'Weaver --help' for more information." << endl;
		return 1;
	}

	if(result.count("optimization-log"))
	{
		if(!result.count("burn-in"))
		{
			cerr << "Error: Required arguments are missing: 'burn-in'." << endl;
			cerr << "Try 'Weaver --help' for more information." << endl;
			return 1;
		}
	}
	else
	{
		if(result.count("burn-in"))
		{
			cerr << "Error: Required arguments are missing: 'optimization-log'." << endl;
			cerr << "Try 'Weaver --help' for more information." << endl;
			return 1;
		}
	}


	IBM::run(inputConfigPath, outputFolder, silentMode, burnIn);
	
	return 0;
}
