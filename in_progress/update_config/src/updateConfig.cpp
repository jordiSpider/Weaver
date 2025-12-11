
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <cxxopts.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

#include "Tools/Config/Config.h"
#include "Tools/Config/VersionNumber.h"
#include "Tools/Updater/Versions/Updater_23_11_05.h"
#include "Tools/Updater/Versions/Updater_23_12_04.h"
#include "Exceptions/LineInfoException.h"


using namespace std;
using json = nlohmann::json;
namespace fs = boost::filesystem;

int main(int argc, char ** argv)
{
    cxxopts::Options options("Updater config", "Description");

	fs::path config_path;
    options.add_options()
        ("config_path", "Ruta a la configuración", cxxopts::value<fs::path>(config_path));

	string output_version;
    options.add_options()
        ("output_version", "Versión a la que actualizar la configuración", cxxopts::value<string>(output_version)->default_value(PROJECT_VERSION));

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

    if(!result.count("config_path"))
	{
		cerr << "Error: Required arguments are missing." << endl;
		cerr << "Try 'updateConfig --help' for more information." << endl;
		return 1;
	}


    // Convert config folder relative path to absolute path
	try
	{
		config_path = fs::canonical(config_path);
	}
	catch(const boost::filesystem::filesystem_error& e)
	{
		throwLineInfoException("The config directory does not exist or is not a valid directory");
	}


    Config initialConfig = Config(config_path);


    vector<Updater*> updaters_list = {
        new Updater_23_11_05(),
        new Updater_23_12_04()
    };


    unsigned int updater_index;


    int initial_updater_index = -1;
    updater_index = 0;
    while(updater_index < updaters_list.size() && initial_updater_index == -1)
    {
        if(initialConfig.getVersion() == updaters_list[updater_index]->getVersion())
        {
            initial_updater_index = updater_index + 1;
        }

        updater_index += 1;
    }


    if(initial_updater_index == -1)
    {
        throwLineInfoException("Error: Configuration version does not exist.");
    }



    int final_updater_index = -1;
    VersionNumber final_version = VersionNumber(output_version);

    updater_index = 0;
    while(updater_index < updaters_list.size() && final_updater_index == -1)
    {
        if(final_version == updaters_list[updater_index]->getVersion())
        {
            final_updater_index = updater_index + 1;
        }

        updater_index += 1;
    }

    if(final_updater_index == -1)
    {
        throwLineInfoException("Error: Output version does not exist.");
    }



    for(int index = initial_updater_index; index < final_updater_index; index++)
    {
        updaters_list[index]->update(initialConfig);
    }


    initialConfig.save(fs::path(config_path.string() + "_updated"), overwrite);


    for(Updater* elem : updaters_list)
    {
        delete elem;
    }


    return 0;
}