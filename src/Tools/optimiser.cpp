
#include <cxxopts.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "Tools/Optimiser/Optimiser.h"
#include "Exceptions/LineInfoException.h"
#include "Misc/Utilities.h"


using namespace std;
using json = nlohmann::json;
namespace fs = filesystem;


int main(int argc, char ** argv)
{
    cxxopts::Options options("Optimiser", "Description");

	fs::path run_params_path;
    options.add_options()
        ("run_params_path", "Ruta a la configuración", cxxopts::value<fs::path>(run_params_path));

	bool overwrite;
    options.add_options()
        ("overwrite", "Flag para sobreescribir la configuración de salida", cxxopts::value<bool>(overwrite)->default_value("false"));

    options.add_options()
		("h,help", "Print usage");


	auto result = options.parse(argc, argv);

    if(result.count("help"))
	{
		cout << options.help() << endl;
		return 0;
	}

    if(!result.count("run_params_path"))
	{
		cerr << "Error: Required arguments are missing." << endl;
		cerr << "Try 'optimiser --help' for more information." << endl;
		return 1;
	}


    // Convert config folder relative path to absolute path
	try
	{
		run_params_path = fs::canonical(run_params_path);
	}
	catch(const std::filesystem::filesystem_error& e)
	{
		throwLineInfoException("The config directory does not exist or is not a valid directory");
	}


    json run_params = readConfigFile(run_params_path);


    Optimiser::runOptimiser(run_params, overwrite);

    return 0;
}
