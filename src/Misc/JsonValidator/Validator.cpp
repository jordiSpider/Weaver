#include "Misc/JsonValidator/Validator.h"


using namespace std;
using json = nlohmann::json;
namespace fs = std::filesystem;


JsonValidator::JsonValidator(std::string_view schemaContent, const std::string& schemaName)
{
	json schema;

	try
	{
		schema = json::parse(
			schemaContent, /* Embedded JSON string */
			nullptr,	   /* Callback */
			true,		   /* Allow exceptions */
			true		   /* Ignore comments */
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
		e.addPreMessage("(" + schemaName + ") 'root");
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
