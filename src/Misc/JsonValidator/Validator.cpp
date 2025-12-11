#include "Misc/JsonValidator/Validator.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;


JsonValidator::JsonValidator(const fs::path& schemaPath)
{
	ifstream schemaFile(schemaPath.string());

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


	try 
	{
		root = new Object(schema);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage("(" + schemaPath.filename().string() + ") 'root");
		throw;
	}
}

JsonValidator::~JsonValidator()
{
	delete root;
}

void JsonValidator::validate(const std::string& configFilename, const json& config) const {
	try
	{
        root->validate(config);
	}
	catch(ValidatorConfigJSONException& e)
	{
		e.addPreMessage("(" + configFilename + ") 'root");
		throw;
	}
}
