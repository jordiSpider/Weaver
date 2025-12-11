
#include <cxxopts.hpp>

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

#include "App/Model/Model.h"
#include "App/View/Console.h"


using namespace std;
namespace fs = std::filesystem;




int main(int argc, char ** argv)
{
	cout << setprecision(numeric_limits<double>::max_digits10) << fixed;
	cerr << setprecision(numeric_limits<double>::max_digits10) << fixed;


	cxxopts::Options options("Weaver", "Weaver description");

	string mode;
	string inputConfig, outputFolder, checkpointPath;
	bool silent = false, help = false;

	options.add_options("Common")
		("mode", "Description mode", cxxopts::value<string>(mode)->default_value("FromConfig"))
		("I,input_config", "Description input config", cxxopts::value<string>(inputConfig)->default_value(""))
		("O,output_folder", "Description output folder", cxxopts::value<string>(outputFolder)->default_value(""))
		("silent", "Run the program in silent mode", cxxopts::value<bool>(silent)->implicit_value("true"))
		("h,help", "Print usage", cxxopts::value<bool>(help)->implicit_value("true"));

	auto result = options.parse(argc, argv);

	if(help)
	{
		cout << options.help() << endl;

		return 0;
	}
	
	Console view;

	Model* model = Model::createInstance(Model::Type::IBM, view);

	view.setModel(*model);

	unsigned char returnCode;

	try {
        view.run(mode, inputConfig, outputFolder, silent);
		returnCode = 0;
    } catch (const LineInfoException& e) {
        cerr << e.what() << endl;
        returnCode = 1;
    } catch (const std::exception& e) {
        cerr << "Caught std::exception: " << e.what() << endl;
        returnCode = 2;
    } catch (...) {
        cerr << "Caught unknown exception." << endl;
        returnCode = 3;
    }

	delete model;

    return returnCode;
}

