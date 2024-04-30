#include "Utilities.h"
#include <limits>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <regex>
#include <sstream>
#include "GlobalVariable.h"
#include "ValidatorJSON.h"
#include "LineInfoException.h"
#include <stack>
#include <set>


using namespace std;
using json = nlohmann::json;


unordered_map<string, OutputStreamConverter::OutputStream> OutputStreamConverter::stringToEnum = {
    {"terminal", Terminal},
    {"log", Log}
};

string getResultFolderName(const string& resultDirectory, const string& baseName)
{
	vector<unsigned int> vectorIds;
    regex pattern("^" + baseName + "_([0-9]+)$");
	regex folderIdPattern("([0-9]+)$");

    for (const auto& entry : fs::directory_iterator(resultDirectory)) {
        if(entry.is_directory()) {
            string entryName = entry.path().filename().string();
            if(std::regex_match(entryName, pattern)) {
				smatch matchfolderId;
				regex_search(entryName, matchfolderId, folderIdPattern);
                vectorIds.push_back(stoi(matchfolderId[0]));
            }
        }
    }
	
    sort(vectorIds.begin(), vectorIds.end());

	return baseName + "_" + to_string(vectorIds.empty() ? 0 : vectorIds.back() + 1);
}

fs::path obtainOutputDirectory(const json& configuration)
{
	string resultDirectory;
	try
	{
		resultDirectory = configuration["simulation"].at("resultDirectory");
	}
	catch(const json::out_of_range& e)
	{
		resultDirectory = RESULT_SIMULATION_FOLDER;
	}

	string resultName;
	try
	{
		resultName = configuration["simulation"].at("resultName");
	}
	catch(const json::out_of_range& e)
	{
		resultName = configuration["simulation"]["configName"];
	}
	
	if(!fs::is_directory(resultDirectory)) 
	{
		throwLineInfoException("The specified path \"" + resultDirectory + "\" does not exist or it is not a directory");
	}

	return fs::path(resultDirectory) / fs::path(getResultFolderName(resultDirectory, resultName));
}

string createOutputFile(ofstream &file, fs::path filenameRoot, string filename, string extension, ios_base::openmode) {
	fs::path file_path = filenameRoot / fs::path(filename + "." + extension);
    file.open(file_path);

    return file_path.string();
}

string createOutputFile(ofstream &file, fs::path filenameRoot, string extension, date_type timeStep, unsigned int recordEach, ios_base::openmode) {
	stringstream ss;
	if((timeStep%recordEach==0) || timeStep==0){
		ss << std::setw(MAX_NUM_DIGITS_DAY) << std::setfill('0') << timeStep;
	}else{
		ss << "dummy_file";	
	}

    fs::path file_path = fs::path(filenameRoot.string() + ss.str() + "." + extension);
	file.open(file_path);

    return file_path.string();
}

void setOutputStream(ofstream& logFile, const fs::path& outputDirectory, const string& outputStreamType)
{
	switch (OutputStreamConverter::stringToEnumValue(outputStreamType))
	{
		case OutputStreamConverter::Log: {
			createOutputFile(logFile, outputDirectory, LOG_FILENAME, LOG_EXTENSION);
        	cout.rdbuf(logFile.rdbuf());
			break;
		}
		case OutputStreamConverter::Terminal: {
			cout.rdbuf(cout.rdbuf());
			break;
		}
	}
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
	string filename = configPath.filename().string();
	fs::path schemaPath = fs::path(SCHEMA_FOLDER) / fs::path(filename.substr(0, filename.find(".")) + ".schema.json");

	return readConfigFile(configPath, schemaPath);
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
