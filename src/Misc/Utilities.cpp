#include "Misc/Utilities.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;



string createOutputFile(std::ofstream &file, fs::path file_path, ios_base::openmode openMode) {
	file.open(file_path.string(), openMode);

	file << setprecision(numeric_limits<double>::max_digits10) << fixed;

    return file_path.string();
}

string createOutputFile(std::ofstream &file, fs::path filenameRoot, string filename, string extension, ios_base::openmode openMode) {
	fs::path file_path = filenameRoot / fs::path(filename + "." + extension);
    file.open(file_path.string(), openMode);

	file << setprecision(numeric_limits<double>::max_digits10) << fixed;

    return file_path.string();
}

string createOutputFile(std::ofstream &file, fs::path filenameRoot, string filename, string extension, unsigned int numberOfTimeSteps, unsigned int recordEach, ios_base::openmode openMode) {
	string ss;
	if((numberOfTimeSteps % recordEach == 0) || numberOfTimeSteps == 0) {
		ss = string(MAX_NUM_DIGITS_DAY - to_string(numberOfTimeSteps).length(), '0') + to_string(numberOfTimeSteps);
	}else{
		ss = "dummy_file";	
	}

    fs::path file_path = filenameRoot / fs::path(filename + ss + "." + extension);
	file.open(file_path.string(), openMode);

	file << setprecision(numeric_limits<double>::max_digits10) << fixed;

    return file_path.string();
}

json readConfigFile(fs::path configPath) {
	
	try {
        // Read configuration file
		ifstream configFile(configPath.string());

        if (!configFile.is_open()) {
			throwLineInfoException("Error al abrir el archivo \"" + configPath.string() + "\".");
        }

        json configuration;

		try
		{
			configuration = json::parse(configFile, nullptr, true, true);

			return configuration;
		}
		catch (json::exception &e)
		{
			throwLineInfoException(e.what());
		}

    } catch (const std::ifstream::failure& e) {
		string message = string("ifstream error : ") + e.what();
		throwLineInfoException(message);
    } catch (const std::exception& e) {
		string message = string("General error: ") + e.what();
        throwLineInfoException(message);
    } catch (...) {
		throwLineInfoException("Unknown error.");
    }
}

void saveConfigFile(fs::path configPath, json fileContent) {
	std::ofstream outputFile(configPath.string());

    if(outputFile.is_open()) 
	{
        outputFile << fileContent.dump(4) << endl;
		outputFile.close();
    }
}

