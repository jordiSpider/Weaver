
#include <iostream>
#include <cxxopts.hpp>
#include <nana/gui.hpp>

#include "OutputAnalyser/MainView.h"


using namespace std;
namespace fs = boost::filesystem;




int main(int argc, char ** argv)
{
	cxxopts::Options options("Output Analyser", "Output Analyser description");

	options.add_options()
		("h,help", "Print usage");


	auto result = options.parse(argc, argv);

	if(result.count("help"))
	{
		cout << options.help() << endl;
		return 0;
	}


	MainView view;

	view.run();

	return 0;
}

