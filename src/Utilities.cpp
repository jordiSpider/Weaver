#include "Utilities.h"
#include <limits>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <regex>
#include <sstream>
#include "GlobalVariable.h"
#include <stack>
#include <set>


using namespace std;
using json = nlohmann::json;


string getResultFolderName(const string& resultFolder, const string& baseName)
{
	vector<unsigned int> vectorIds;
    regex pattern("^" + baseName + "_([0-9]+)$");
	regex folderIdPattern("([0-9]+)$");

    for (const auto& entry : fs::directory_iterator(resultFolder)) {
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

fs::path obtainResultFolder(const json& configuration, fs::path outputFolder)
{
	if(!fs::is_directory(outputFolder)) 
	{
		throwLineInfoException("The specified path \"" + outputFolder.string() + "\" does not exist or it is not a directory");
	}

	return fs::path(outputFolder) / fs::path(getResultFolderName(outputFolder.string(), (string)configuration["simulation"]["configName"]));
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

json readConfigFile1(fs::path configPath, fs::path WeaverFolder) {
	string filename = configPath.filename().string();
	fs::path schemaPath = WeaverFolder / fs::path(SCHEMA_FOLDER) / fs::path(filename.substr(0, filename.find(".")) + ".schema.json");

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
			Output::cout("{}\n", e.what());
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
			Output::cout("{}\n", e.what());
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
