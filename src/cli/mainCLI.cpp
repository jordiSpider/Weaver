
#include <cxxopts.hpp>

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <limits>
#include <memory>
#include <string>
#include <thread>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#endif

#include "App/Model/Model.h"
#include "App/View/Console.h"



using namespace std;
namespace fs = std::filesystem;


#ifdef _WIN32
namespace
{
	void waitForConsoleKey()
	{
		cout << "\nPress any key to close this window..." << flush;
		_getch();
	}

	void keepOwnConsoleOpen()
	{
		DWORD processes[2];
		DWORD inputMode, outputMode;
		if (GetConsoleProcessList(processes, 2) == 1 &&
			GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &inputMode) &&
			GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &outputMode))
		{
			if (std::atexit(waitForConsoleKey) != 0)
			{
				cerr << "Could not register the console close prompt." << endl;
			}
		}
	}
}
#endif


int runCLI(int argc, char ** argv)
{
	cxxopts::Options options("Weaver", "Weaver description");

	string mode;
	string inputConfig, outputFolder, checkpointPath;
	bool silent = false, verbose = false, enableDiskOutputMock = false, schema_version = false, help = false;
	unsigned int requestedThreads;

	options.add_options("Common")
		("mode", "Description mode", cxxopts::value<string>(mode)->default_value("FromConfig"))
		("I,input_config", "Description input config", cxxopts::value<string>(inputConfig)->default_value(""))
		("O,output_folder", "Description output folder", cxxopts::value<string>(outputFolder)->default_value(""))
		("silent", "Run the program in silent mode", cxxopts::value<bool>(silent)->implicit_value("true"))
		("verbose", "Run the program in verbose mode", cxxopts::value<bool>(verbose)->implicit_value("true"))
		("enableDiskOutputMock", "Enable disk output mock execution", cxxopts::value<bool>(enableDiskOutputMock)->implicit_value("true"))
		("parallel", "Specify the number of threads to use", cxxopts::value<unsigned int>(requestedThreads)->default_value("0"))
		("schema_version", "Print schema version", cxxopts::value<bool>(schema_version)->implicit_value("true"))
		("h,help", "Print usage", cxxopts::value<bool>(help)->implicit_value("true"));

	auto result = options.parse(argc, argv);

	if(help)
	{
		cout << options.help() << endl;

		return 0;
	}

	if(silent && verbose)
	{
		throwLineInfoException("Error: Both silent and verbose modes cannot be enabled simultaneously.");
	}

	if (schema_version)
	{
		cout << "Schema version: " << WEAVER_SCHEMA_VERSION << endl;
		return 0;
	}
	


	unique_ptr<Model> model(Model::createInstance(Model::Type::IBM));
	Console view(model.get());
	view.run(mode, inputConfig, outputFolder, silent, verbose, enableDiskOutputMock, requestedThreads);
	return 0;
}

int main(int argc, char ** argv)
{
	cout << setprecision(numeric_limits<double>::max_digits10) << fixed;
	cerr << setprecision(numeric_limits<double>::max_digits10) << fixed;

#ifdef _WIN32
	keepOwnConsoleOpen();
#endif

	try {
		return runCLI(argc, argv);
	}
	catch (const LineInfoException& e) {
		cerr << e.what() << endl;
		return 1;
	}
	catch (const std::exception& e) {
		cerr << "Caught std::exception: " << e.what() << endl;
		return 2;
	}
	catch (...) {
		cerr << "Caught unknown exception." << endl;
		return 3;
	}
}

