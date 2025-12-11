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

string createOutputFile(std::ofstream &file, fs::path filenameRoot, string filename, string extension, const TimeStep numberOfTimeSteps, unsigned int recordEach, ios_base::openmode openMode) {
	string ss;
	if((static_cast<unsigned int>(round(numberOfTimeSteps.getValue())) % recordEach == 0) || static_cast<unsigned int>(round(numberOfTimeSteps.getValue())) == 0) {
		ss = string(MAX_NUM_DIGITS_DAY - to_string(numberOfTimeSteps.getValue()).length(), '0') + to_string(numberOfTimeSteps.getValue());
	}else{
		ss = "dummy_file";	
	}

    fs::path file_path = filenameRoot / fs::path(filename + ss + "." + extension);
	file.open(file_path.string(), openMode);

	file << setprecision(numeric_limits<double>::max_digits10) << fixed;

    return file_path.string();
}

json readConfigFile(fs::path configPath, const JsonValidator& validator) {
	
	try {
        // Read configuration file
		ifstream configFile(configPath.string());

        if (!configFile.is_open()) {
			throwLineInfoException("Error al abrir el archivo \"" + configPath.string() + "\".");
        }

        json configuration;

		try
		{
			/*
				He añadido esta función porque nlohmann json no incluye la comprobación
				de claves repetidas.

				Para más información ver la dicusión (https://github.com/nlohmann/json/issues/375)

				Para implementar la función me he basado en la documentación (https://json.nlohmann.me/api/basic_json/parser_callback_t/#nlohmannbasic_jsonparser_callback_t)
			*/
			stack<set<json>> parse_stack;
			json::parser_callback_t check_duplicate_keys = [&parse_stack](int, json::parse_event_t event, json &parsed) {
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
					case json::parse_event_t::array_start:
					case json::parse_event_t::array_end:
					case json::parse_event_t::value:
						break;
					default: {
						break;
					}
				}

				// yes, store parsed value
				return true;
			};

			configuration = json::parse(
				configFile, /* Configuration file */
				check_duplicate_keys, /* Callback */
				true, /* Allow exceptions */
				true /* Ignore comments */
			);

			// Realizar la validación
			validator.validate(configPath.filename().string(), configuration);

			if(configuration["version"].get<string>() != SCHEMA_VERSION)
			{
				throwLineInfoException("The configuration file '" + configPath.filename().string() + "' version (" + configuration["version"].get<string>() + ") does not match the software schema version (" + SCHEMA_VERSION + ").");
			}

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
