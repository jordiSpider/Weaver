#include "Misc/Utilities.h"


using namespace std;
using json = nlohmann::json;
namespace fs = boost::filesystem;


string getResultFolderName(const string& baseName)
{
	auto tiempoActual = std::chrono::system_clock::now();
    std::time_t tiempoT = std::chrono::system_clock::to_time_t(tiempoActual);

	std::tm tmInfo = {};
    std::stringstream ss;
    ss << std::put_time(std::localtime(&tiempoT), "%Y%m%d_%H%M%S");
    ss >> std::get_time(&tmInfo, "%Y%m%d_%H%M%S");

	string filename = baseName + "__";
    filename += to_string(1900 + tmInfo.tm_year) + "-"; // Año
    filename += to_string(1 + tmInfo.tm_mon) + "-";    	// Mes
    filename += to_string(tmInfo.tm_mday) + "_";       	// Día
    filename += to_string(tmInfo.tm_hour) + "-";       	// Hora
    filename += to_string(tmInfo.tm_min) + "-";        	// Minuto
    filename += to_string(tmInfo.tm_sec);        		// Segundo

	return filename;
}

fs::path obtainResultFolder(const string& baseName, fs::path outputFolder)
{
	if(!fs::is_directory(outputFolder)) 
	{
		throwLineInfoException("The specified path \"" + outputFolder.string() + "\" does not exist or it is not a directory");
	}

	return fs::path(outputFolder) / fs::path(getResultFolderName(baseName));
}

string createOutputFile(ofstream &file, fs::path filenameRoot, string filename, string extension, ios_base::openmode openMode) {
	fs::path file_path = filenameRoot / fs::path(filename + "." + extension);
    file.open(file_path, openMode);

    return file_path.string();
}

string createOutputFile(ofstream &file, fs::path filenameRoot, string filename, string extension, date_type timeStep, unsigned int recordEach, ios_base::openmode openMode) {
	string ss;
	if((timeStep%recordEach==0) || timeStep==0){
		ss = string(MAX_NUM_DIGITS_DAY - to_string(timeStep).length(), '0') + to_string(timeStep);
	}else{
		ss = "dummy_file";	
	}

    fs::path file_path = filenameRoot / fs::path(filename + ss + "." + extension);
	file.open(file_path, openMode);

    return file_path.string();
}

/*
	He añadido esta función porque nlohmann json no incluye la comprobación
	de claves repetidas.

	Para más información ver la dicusión (https://github.com/nlohmann/json/issues/375)

	Para implementar la función me he basado en la documentación (https://json.nlohmann.me/api/basic_json/parser_callback_t/#nlohmannbasic_jsonparser_callback_t)
*/
stack<set<json>> parse_stack;
json::parser_callback_t check_duplicate_keys = [](int depth, json::parse_event_t event, json &parsed) {
	switch (event) {
		case json::parse_event_t::object_start: {
			parse_stack.push(set<json>());
			break;
		}
		case json::parse_event_t::object_end: {
			parse_stack.pop();
			break;
		} 
		case json::parse_event_t::key: {
			const auto result = parse_stack.top().insert(parsed);
			if (!result.second) {
				throwValidatorJSONException("'" + string(parsed) + "': Key duplicated");
			}
			break;
		}
		default: {
			break;
		}
	}

	// yes, store parsed value
	return true;
};

json readConfigFile(fs::path configPath) {
	try {
        // Read configuration file
		ifstream configFile(configPath);

        if (!configFile.is_open()) {
			throwLineInfoException("Error al abrir el archivo \"" + configPath.string() + "\".");
        }

        json configuration;

		try
		{
			configuration = json::parse(
				configFile, /* Configuration file */
				check_duplicate_keys, /* Callback */
				true, /* Allow exceptions */
				true /* Ignore comments */
			);
		}
		catch (json::exception &e)
		{
			cout << e.what() << endl;
		}

		return configuration;

    } catch (const std::ifstream::failure& e) {
		string message = string("Error de ifstream: ") + e.what();
		throwLineInfoException(message);
    } catch (const std::exception& e) {
		string message = string("Error general: ") + e.what();
        throwLineInfoException(message);
    } catch (...) {
		throwLineInfoException("Error desconocido.");
    }
}

json readConfigFile(fs::path configPath, fs::path schemaPath) {
	// Read configuration file
	ifstream configFile(configPath);

	json configuration;

	try
	{
		try
		{
			configuration = json::parse(
				configFile, /* Configuration file */
				check_duplicate_keys, /* Callback */
				true, /* Allow exceptions */
				true /* Ignore comments */
			);
		}
		catch (json::exception &e)
		{
			cout << e.what() << endl;
		}

		ifstream schemaFile(schemaPath);

		json schema;

		try
		{
			schema = json::parse(
				schemaFile, /* Schema file */
				nullptr, /* Callback */
				true, /* Allow exceptions */
				true /* Ignore comments */
			);
		}
		catch (json::exception &e)
		{
			cout << e.what() << endl;
		}

		// Realizar la validación
    
        ValidatorJSON::validateJSON(configuration, schema);
    } 
	catch (ValidatorConfigJSONException& e) {
		e.addPreMessage("(" + configPath.filename().string() + ") ");
		throw;
    }
	catch (ValidatorSchemaJSONException& e) {
		e.addPreMessage("(" + schemaPath.filename().string() + ") ");
		throw;
    }

	return configuration;
}

void saveConfigFile(fs::path configPath, json fileContent) {
	ofstream outputFile(configPath);

    if(outputFile.is_open()) 
	{
        outputFile << fileContent.dump(4) << endl;
		outputFile.close();
    }
}

unsigned int projectVersionStringToNumber(  std::string versionString){
        unsigned int year, month, day;

        std::istringstream stream(versionString);

        std::getline(stream, versionString, '.');
        year = std::stoi(versionString);

        std::getline(stream, versionString, '.');
        month = std::stoi(versionString);

        std::getline(stream, versionString);
        day = std::stoi(versionString);

        unsigned int result = year * 10000 + month * 1000 + day;

        return result;
}
