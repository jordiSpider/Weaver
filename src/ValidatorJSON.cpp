#include "ValidatorJSON.h"
#include <algorithm>
#include <iostream>


using namespace std;
using json = nlohmann::json;




unordered_map<string, TypeElemJSONConverter::TypeElemJSON> TypeElemJSONConverter::stringToEnum = {
    {"object", Object},
    {"array", Array},
    {"string", String},
    {"integer", Integer},
    {"unsigned_integer", Unsigned_Integer},
    {"float", Float},
    {"boolean", Boolean}
};

const string TypeElemJSONConverter::printValues()
{
	string values = stringToEnum.cbegin()->first;
	
	for(auto it = ++stringToEnum.cbegin(); it != stringToEnum.cend(); it++)
	{
		values += ", " + it->first;
	}

	return values;
}


unordered_map<string, OperatorsJSONConverter::OperatorsJSON> OperatorsJSONConverter::stringToEnum = {
	{"if", If},
    {"equal", Equal},
    {"not_equal", Not_Equal},
    {"less_than", Less_Than},
    {"greater_than", Greater_Than},
    {"less_equal_than", Less_Equal_Than},
    {"greater_equal_than", Greater_Equal_Than},
    {"not", Not},
    {"and", And},
    {"or", Or},
	{"property", Property},
	{"property_item", Property_Item},
	{"property_size", Property_Size},
	{"property_exists", Property_Exists},
    {"constant", Constant},
    {"subtract", Subtract},
    {"sum", Sum},
	{"division", Division},
	{"multiply", Multiply},
	{"module", Module}
};

const string OperatorsJSONConverter::printValues()
{
	string values = stringToEnum.cbegin()->first;
	
	for(auto it = ++stringToEnum.cbegin(); it != stringToEnum.cend(); it++)
	{
		values += ", " + it->first;
	}

	return values;
}

unordered_map<string, ItemsOrderConverter::ItemsOrder> ItemsOrderConverter::stringToEnum = {
    {"increasing", Increasing},
    {"strictly_increasing", Strictly_Increasing},
    {"decreasing", Decreasing},
    {"strictly_decreasing", Strictly_Decreasing}
}; 

const string ItemsOrderConverter::printValues()
{
	string values = stringToEnum.cbegin()->first;
	
	for(auto it = ++stringToEnum.cbegin(); it != stringToEnum.cend(); it++)
	{
		values += ", " + it->first;
	}

	return values;
}


RegexKey::RegexKey(const string& regexValue) {
    this->regexValue = regexValue;
    this->pattern = regex(regexValue);
}

bool RegexKey::operator==(const string &other) const { 
    return regex_match(other, pattern);
}


ListRegexKey::ListRegexKey(const json* list) {
    for(size_t i = 0; i < list->size(); i++) {
		try
		{
			if(!(*list)[i].is_string()) {
				throwValidatorSchemaJSONException("': Not an element of type 'string'");
			}

        	listRegex.push_back(RegexKey((*list)[i]));
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".item_" + to_string(i));
			throw;
		}
    }
}

void ListRegexKey::findRegexKey(const string& key) const {
    auto it = find(listRegex.cbegin(), listRegex.cend(), key);
    if(it == listRegex.cend()) {
        throw out_of_range("");
    }
}


MapRegexKey::MapRegexKey(const json* schema) {
    this->schema = schema;

    for(const auto& [key, value] : schema->items()) {
        listRegex.push_back(RegexKey(key));
    }
}

const json& MapRegexKey::at(const string& key, string& regexKey) const {
    auto it = find(listRegex.cbegin(), listRegex.cend(), key);
    if(it == listRegex.cend()) {
        throw out_of_range("");
    }

    regexKey = it->getRegexValue();

    return (*schema)[it->getRegexValue()];
}

const json& MapRegexKey::at(const string& key) const {
    auto it = find(listRegex.cbegin(), listRegex.cend(), key);
    if(it == listRegex.cend()) {
        throw out_of_range("");
    }

    return (*schema)[it->getRegexValue()];
}



std::ofstream ValidatorJSON::warningsFile("warnings.txt");


bool ValidatorJSON::isRequired(map<string, bool>& required, const string& property) {
	try
	{
		required.at(property);

		return true;
	}
	catch(const out_of_range& e)
	{
		return false;
	}
}

void ValidatorJSON::addObject(json& object, const map<string, json>& newObject, const string patternElem) {
	for(const auto& [key, value] : newObject) {
		try
		{
			if(!value.is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}

			if(patternElem != "" && key == PATTERN_ELEM) {
				object[patternElem] = value;
			}
			else {
				object[key] = value;
			}
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage("." + key);
			throw;
		}
	}
}

void ValidatorJSON::removeObject(json& object, const json& objectToRemove, const string patternElem)
{
	for(size_t i = 0; i < objectToRemove.size(); i++) {
		try
		{
			if(!objectToRemove[i].is_string()) {
				throwValidatorSchemaJSONException("': Not an element of type 'string'");
			}

			if(patternElem != "" && objectToRemove[i] == PATTERN_ELEM) {
				object.erase(patternElem);
			}
			else {
				object.erase(objectToRemove[i]);
			}
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".item_" + to_string(i));
			throw;
		}
	}
}

void ValidatorJSON::addArray(json& array, const json& newArray) {
	for(size_t i = 0; i < newArray.size(); i++) {
		try
		{
			if(!newArray[i].is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}

			array.push_back(newArray[i]);
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".item_" + to_string(i));
			throw;
		}
	}
}

void ValidatorJSON::addRequired(json& properties, const json& requiredToAdd, const string patternElem) 
{
	for(size_t i = 0; i < requiredToAdd.size(); i++) {
		try
		{
			if(!requiredToAdd[i].is_string()) {
				throwValidatorSchemaJSONException("': Not an element of type 'string'");
			}

			if(patternElem != "" && requiredToAdd[i] == PATTERN_ELEM) {
				properties[patternElem]["required"] = true;
			}
			else {
				properties[requiredToAdd[i]]["required"] = true;
			}
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".item_" + to_string(i));
			throw;
		}
	}
}

void ValidatorJSON::addPatternRequired(map<string,json>& properties, const json& requiredToAdd) 
{
	ListRegexKey listPatternProperties;
	listPatternProperties = ListRegexKey(&requiredToAdd);

	for(auto& [key, value] : properties) {
		try
		{
			listPatternProperties.findRegexKey(key);
			value["required"] = true;
		}
		catch(const std::out_of_range& e) {}
	}
}

void ValidatorJSON::removeRequired(json& properties, const json& requiredToRemove, const string patternElem) 
{
	for(size_t i = 0; i < requiredToRemove.size(); i++) {
		try
		{
			if(!requiredToRemove[i].is_string()) {
				throwValidatorSchemaJSONException("': Not an element of type 'string'");
			}

			if(patternElem != "" && requiredToRemove[i] == PATTERN_ELEM) {
				properties[patternElem]["required"] = false;
			}
			else {
				properties[requiredToRemove[i]]["required"] = false;
			}
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".item_" + to_string(i));
			throw;
		}
	}
}

void ValidatorJSON::removePatternRequired(map<string,json>& properties, const json& requiredToRemove) 
{
	ListRegexKey listPatternProperties;
	listPatternProperties = ListRegexKey(&requiredToRemove);

	for(auto& [key, value] : properties) {
		try
		{
			listPatternProperties.findRegexKey(key);
			value["required"] = false;
		}
		catch(const std::out_of_range& e) {}
	}
}

void ValidatorJSON::doConditionBlock(json& schema, const json& block, const json& config, const string patternElem) {
	json* dstShema = &schema;
	const json* dstConfig = &config;
	string dstName;

	try
	{
		try
		{
			if(!block.at("dst").is_array()) {
				throwValidatorSchemaJSONException("': Not an element of type 'array'");
			}

			for(size_t i = 0; i < block["dst"].size(); i++) {
				try
				{
					if(!block["dst"][i].is_string()) {
						throwValidatorSchemaJSONException("': Not an element of type 'string'");
					}

					if(i == 0) {
						dstName = block["dst"][i];
					}
					else {
						dstName += "." + (string)block["dst"][i];
					}

					try
					{
						dstShema = &(*dstShema).at("properties").at(block["dst"][i]);
					}
					catch(const json::out_of_range& e) 
					{
						throwValidatorSchemaJSONException("': Destination '" + dstName + "' does not exist in schema");
					}

					try
					{
						dstConfig = &(*dstConfig).at(block["dst"][i]);
					}
					catch(const json::out_of_range& e) 
					{
						throwValidatorSchemaJSONException("': Destination '" + dstName + "' does not exist in config");
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".item_" + to_string(i));
					throw;
				}
			}
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".dst");
			throw;
		}
	}
	catch(const json::out_of_range& e) {}

	try
	{
		try
		{
			if(!block.at("remove").is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}

			removeSchema(*dstShema, block["remove"], patternElem);
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".remove");
			throw;
		}
	}
	catch(const json::out_of_range& e) {}

	try
	{
		try
		{
			if(!block.at("add").is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}

			addSchema(*dstShema, block["add"], *dstConfig, patternElem);
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".add");
			throw;
		}
	}
	catch(const json::out_of_range& e) {}
}

void ValidatorJSON::checkConditionals(json& schema, const json& conditionals, const json& config) {
	for(size_t i = 0; i < conditionals.size(); i++) {
		try
		{
			if(!conditionals[i].is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}
			
			bool conditionResult;

			try
			{
				if(!conditionals[i].at("if").is_object()) {
					throwValidatorSchemaJSONException("': Not an element of type 'object'");
				}

				conditionResult = checkCondition(config, conditionals[i]["if"]);
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".if");
				throw;
			}
			catch(const json::out_of_range& e) 
			{
				throwValidatorSchemaJSONException("': Property 'if' not defined");
			}

			if(conditionResult) {
				try
				{
					if(!conditionals[i].at("then").is_array()) {
						throwValidatorSchemaJSONException("': Not an element of type 'array'");
					}

					for(size_t j = 0; j < conditionals[i]["then"].size(); j++) {
						try
						{
							if(!conditionals[i]["then"][j].is_object()) {
								throwValidatorSchemaJSONException("': Not an element of type 'object'");
							}

							doConditionBlock(schema, conditionals[i]["then"][j], config);
						}
						catch(ValidatorSchemaJSONException& e)
						{
							e.addPreMessage(".item_" + to_string(j));
							throw;
						}
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".then");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'then' not defined");
				}
			}
			else {
				try
				{
					if(!conditionals[i].at("else").is_array()) {
						throwValidatorSchemaJSONException("': Not an element of type 'array'");
					}

					for(size_t j = 0; j < conditionals[i]["else"].size(); j++) {
						try
						{
							if(!conditionals[i]["else"][j].is_object()) {
								throwValidatorSchemaJSONException("': Not an element of type 'object'");
							}

							doConditionBlock(schema, conditionals[i]["else"][j], config);
						}
						catch(ValidatorSchemaJSONException& e)
						{
							e.addPreMessage(".item_" + to_string(j));
							throw;
						}
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".else");
					throw;
				}
				catch(const json::out_of_range& e) {}
			}
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".item_" + to_string(i));
			throw;
		}
	}
}

void ValidatorJSON::checkPatternConditionals(json& schema, const json& conditionals, const json& config) {
	for(size_t i = 0; i < conditionals.size(); i++) {
		try
		{
			if(!conditionals[i].is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}

			try
			{
				if(!conditionals[i].at("pattern").is_string()) {
					throwValidatorSchemaJSONException("': Not an element of type 'string'");
				}
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".pattern");
				throw;
			}
			catch(const json::out_of_range& e) 
			{
				throwValidatorSchemaJSONException("': Property 'pattern' not defined");
			}

			RegexKey regexPattern(conditionals[i]["pattern"]);

			try
			{
				if(!conditionals[i].at("if").is_object()) {
					throwValidatorSchemaJSONException("': Not an element of type 'object'");
				}
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".if");
				throw;
			}
			catch(const json::out_of_range& e) 
			{
				throwValidatorSchemaJSONException("': Property 'if' not defined");
			}

			for(const auto& [key, value] : (map<string,json>)schema["properties"]) {
				if(regexPattern == key) {
					bool conditionResult;

					try
					{
						conditionResult = checkCondition(config, conditionals[i]["if"], key);
					}
					catch(ValidatorSchemaJSONException& e)
					{
						e.addPreMessage(".if");
						throw;
					}
					
					if(conditionResult) {
						try
						{
							if(!conditionals[i].at("then").is_array()) {
								throwValidatorSchemaJSONException("': Not an element of type 'array'");
							}

							for(size_t j = 0; j < conditionals[i]["then"].size(); j++) {
								try
								{
									if(!conditionals[i]["then"][j].is_object()) {
										throwValidatorSchemaJSONException("': Not an element of type 'object'");
									}

									doConditionBlock(schema, conditionals[i]["then"][j], config, key);
								}
								catch(ValidatorSchemaJSONException& e)
								{
									e.addPreMessage(".item_" + to_string(j));
									throw;
								}
							}
						}
						catch(ValidatorSchemaJSONException& e)
						{
							e.addPreMessage(".then");
							throw;
						}
						catch(const json::out_of_range& e) 
						{
							throwValidatorSchemaJSONException("': Property 'then' not defined");
						}
					}
					else {
						try
						{
							if(!conditionals[i].at("else").is_array()) {
								throwValidatorSchemaJSONException("': Not an element of type 'array'");
							}

							for(size_t j = 0; j < conditionals[i]["else"].size(); j++) {
								try
								{
									if(!conditionals[i]["else"][j].is_object()) {
										throwValidatorSchemaJSONException("': Not an element of type 'object'");
									}

									doConditionBlock(schema, conditionals[i]["else"][j], config, key);
								}
								catch(ValidatorSchemaJSONException& e)
								{
									e.addPreMessage(".item_" + to_string(j));
									throw;
								}
							}
						}
						catch(ValidatorSchemaJSONException& e)
						{
							e.addPreMessage(".else");
							throw;
						}
						catch(const json::out_of_range& e) {}
					}
				}
			}
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".item_" + to_string(i));
			throw;
		}
	}
}

void ValidatorJSON::addSchema(json& schema, const json& newSchema, const json& config, const string patternElem) {
	try
	{
		if(!newSchema.at("minProperties").is_number_unsigned()) {
			throwValidatorSchemaJSONException("': Not an element of type 'unsigned_integer'");
		}

		schema["minProperties"] = newSchema["minProperties"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".minProperties");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!newSchema.at("maxProperties").is_number_unsigned()) {
			throwValidatorSchemaJSONException("': Not an element of type 'unsigned_integer'");
		}

		schema["maxProperties"] = newSchema["maxProperties"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".maxProperties");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!newSchema.at("properties").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		addObject(schema["properties"], newSchema["properties"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".properties");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!newSchema.at("patternProperties").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		addPatternProperties(schema, newSchema["patternProperties"]);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".patternProperties");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!newSchema.at("required").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		addRequired(schema["properties"], newSchema["required"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".required");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!newSchema.at("patternRequired").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		addPatternRequired((map<string,json>&)schema["properties"], newSchema["patternRequired"]);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".patternRequired");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!newSchema.at("additionalProperties").is_boolean()) {
			throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
		}
		
		schema["additionalProperties"] = newSchema["additionalProperties"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".additionalProperties");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!newSchema.at("requiredConditions").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		addArray(schema["requiredConditions"], newSchema["requiredConditions"]);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".requiredConditions");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!newSchema.at("warningConditions").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		addArray(schema["warningConditions"], newSchema["warningConditions"]);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".warningConditions");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!newSchema.at("conditionals").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		checkConditionals(schema, newSchema["conditionals"], config);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".conditionals");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!newSchema.at("patternConditionals").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		checkPatternConditionals(schema, newSchema["patternConditionals"], config);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".patternConditionals");
		throw;
	}
	catch(const json::out_of_range& e) {}
}

void ValidatorJSON::removeSchema(json& schema, const json& schemaToRemove, const string patternElem)
{
	try
	{
		if(!schemaToRemove.at("properties").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		removeObject(schema["properties"], schemaToRemove["properties"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".properties");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schemaToRemove.at("patternProperties").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		removePatternProperties((map<string,json>&)schema["properties"], schemaToRemove["patternProperties"]);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".patternProperties");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schemaToRemove.at("required").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		removeRequired(schema["properties"], schemaToRemove["required"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".required");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schemaToRemove.at("patternRequired").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		removePatternRequired((map<string,json>&)schema["properties"], schemaToRemove["patternRequired"]);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".patternRequired");
		throw;
	}
	catch(const json::out_of_range& e) {}
}

const json ValidatorJSON::solveIfOperator(const json& config, const json& condition, const string patternElem) {
	bool conditionResult;

	try
	{
		if(!condition.at("condition").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		conditionResult = checkCondition(config, condition["condition"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".condition");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'condition' not defined");
	}
	
	if(conditionResult)
	{
		try
		{
			if(!condition.at("then").is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}

			return solveOperator(config, condition["then"], patternElem);
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".then");
			throw;
		}
		catch(const json::out_of_range& e) 
		{
			throwValidatorSchemaJSONException("': Property 'then' not defined");
		}
	}
	else
	{
		try
		{
			if(!condition.at("else").is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}

			return solveOperator(config, condition["else"], patternElem);
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".else");
			throw;
		}
		catch(const json::out_of_range& e) {}
	}

	return NULL;
}

const json ValidatorJSON::solveEqualOperator(const json& config, const json& condition, const string patternElem) {
	json lhsResult, rhsResult;
	
	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		lhsResult = solveOperator(config, condition["lhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}

	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		rhsResult = solveOperator(config, condition["rhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}

	return lhsResult == rhsResult;
}

const json ValidatorJSON::solveNotEqualOperator(const json& config, const json& condition, const string patternElem) {
	json lhsResult, rhsResult;
	
	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		lhsResult = solveOperator(config, condition["lhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}

	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		rhsResult = solveOperator(config, condition["rhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}

	return lhsResult != rhsResult;
}

const json ValidatorJSON::solveLessThanOperator(const json& config, const json& condition, const string patternElem) {
	json lhsResult, rhsResult;
	
	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		lhsResult = solveOperator(config, condition["lhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}

	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		rhsResult = solveOperator(config, condition["rhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}

	return lhsResult < rhsResult;
}

const json ValidatorJSON::solveGreaterThanOperator(const json& config, const json& condition, const string patternElem) {
	json lhsResult, rhsResult;
	
	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		lhsResult = solveOperator(config, condition["lhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}

	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		rhsResult = solveOperator(config, condition["rhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}

	return lhsResult > rhsResult;
}

const json ValidatorJSON::solveLessEqualThanOperator(const json& config, const json& condition, const string patternElem) {
	json lhsResult, rhsResult;
	
	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		lhsResult = solveOperator(config, condition["lhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}

	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		rhsResult = solveOperator(config, condition["rhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}

	return lhsResult <= rhsResult;
}

const json ValidatorJSON::solveGreaterEqualThanOperator(const json& config, const json& condition, const string patternElem) {
	json lhsResult, rhsResult;
	
	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		lhsResult = solveOperator(config, condition["lhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}

	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		rhsResult = solveOperator(config, condition["rhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}

	return lhsResult >= rhsResult;
}

const json ValidatorJSON::solveNotOperator(const json& config, const json& condition, const string patternElem) {
	json operatorResult;

	try
	{
		if(!condition.at("value").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		operatorResult = solveOperator(config, condition["value"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".value");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'value' not defined");
	}

	return !operatorResult;
}

const json ValidatorJSON::solveAndOperator(const json& config, const json& condition, const string patternElem) {
	bool operatorResult = true;

	try
	{
		if(!condition.at("values").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		for(size_t i = 0; i < condition["values"].size(); i++) {
			try
			{
				if(!condition["values"][i].is_object()) {
					throwValidatorSchemaJSONException("': Not an element of type 'object'");
				}

				operatorResult = operatorResult && solveOperator(config, condition["values"][i], patternElem);
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".values");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'values' not defined");
	}

	return operatorResult;
}

const json ValidatorJSON::solveOrOperator(const json& config, const json& condition, const string patternElem) {
	bool operatorResult = false;

	try
	{
		if(!condition.at("values").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		for(size_t i = 0; i < condition["values"].size(); i++) {
			try
			{
				if(!condition["values"][i].is_object()) {
					throwValidatorSchemaJSONException("': Not an element of type 'object'");
				}

				operatorResult = operatorResult || solveOperator(config, condition["values"][i], patternElem);
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".values");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'values' not defined");
	}

	return operatorResult;
}

template <typename T>
const T ValidatorJSON::solveSubtractOperator(const json& config, const json& condition, const string patternElem) {
	T lhsResult, rhsResult;
	
	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		lhsResult = solveOperator(config, condition["lhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}

	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		rhsResult = solveOperator(config, condition["rhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}

	return lhsResult - rhsResult;
}

template <typename T>
const T ValidatorJSON::solveSumOperator(const json& config, const json& condition, const string patternElem) {
	T lhsResult, rhsResult;
	
	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		lhsResult = solveOperator(config, condition["lhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}

	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		rhsResult = solveOperator(config, condition["rhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}

	return lhsResult + rhsResult;
}

template <typename T>
const T ValidatorJSON::solveDivisionOperator(const json& config, const json& condition, const string patternElem) {
	T lhsResult, rhsResult;
	
	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		lhsResult = solveOperator(config, condition["lhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}

	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		rhsResult = solveOperator(config, condition["rhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}

	return lhsResult / rhsResult;
}

template <typename T>
const T ValidatorJSON::solveMultiplyOperator(const json& config, const json& condition, const string patternElem) {
	T lhsResult, rhsResult;
	
	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		lhsResult = solveOperator(config, condition["lhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}

	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		rhsResult = solveOperator(config, condition["rhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}

	return lhsResult * rhsResult;
}

const int ValidatorJSON::solveModuleOperator(const json& config, const json& condition, const string patternElem) {
	int lhsResult, rhsResult;
	
	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		lhsResult = solveOperator(config, condition["lhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}

	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		rhsResult = solveOperator(config, condition["rhs"], patternElem);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}

	return lhsResult % rhsResult;
}

const json* ValidatorJSON::getProperty(const json& config, const json& listProperties, const string patternElem) {
	const json* property = &config;
	string dstName;

	for(size_t i = 0; i < listProperties.size(); i++) {
		try
		{
			if(!listProperties[i].is_string()) {
				throwValidatorSchemaJSONException("': Not an element of type 'string'");
			}

			if(patternElem != "" && listProperties[i] == PATTERN_ELEM) {
				if(i == 0) {
					dstName = patternElem;
				}
				else {
					dstName += "." + patternElem;
				}

				property = &(*property).at(patternElem);
			}
			else {
				if(i == 0) {
					dstName = listProperties[i];
				}
				else {
					dstName += "." + (string)listProperties[i];
				}

				property = &(*property).at(listProperties[i]);
			}
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".item_" + to_string(i));
			throw;
		}
		catch(const json::out_of_range& e)
		{	
			throwValidatorConfigJSONException("': Item '" + dstName + "' does not exist in config");
		}
	}

	return property;
}

const json ValidatorJSON::solveOperator(const json& config, const json& condition, const string patternElem) {
	try
	{
		if(!condition.at("type").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".type");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'type' not defined. Valid values are " + OperatorsJSONConverter::printValues());
	}
	
	try
	{
		switch (OperatorsJSONConverter::stringToEnumValue((string)condition["type"]))
		{
			case OperatorsJSONConverter::If: {
				return solveIfOperator(config, condition, patternElem);
				break;
			}
			case OperatorsJSONConverter::Equal: {
				return solveEqualOperator(config, condition, patternElem);
				break;
			}
			case OperatorsJSONConverter::Not_Equal: {
				return solveNotEqualOperator(config, condition, patternElem);
				break;
			}
			case OperatorsJSONConverter::Less_Than: {
				return solveLessThanOperator(config, condition, patternElem);
				break;
			}
			case OperatorsJSONConverter::Greater_Than: {
				return solveGreaterThanOperator(config, condition, patternElem);
				break;
			}
			case OperatorsJSONConverter::Less_Equal_Than: {
				return solveLessEqualThanOperator(config, condition, patternElem);
				break;
			}
			case OperatorsJSONConverter::Greater_Equal_Than: {
				return solveGreaterEqualThanOperator(config, condition, patternElem);
				break;
			}
			case OperatorsJSONConverter::Not: {
				return solveNotOperator(config, condition, patternElem);
				break;
			}
			case OperatorsJSONConverter::And: {
				return solveAndOperator(config, condition, patternElem);
				break;
			}
			case OperatorsJSONConverter::Or: {
				return solveOrOperator(config, condition, patternElem);
				break;
			}
			case OperatorsJSONConverter::Property: {
				try
				{
					if(!condition.at("value").is_array()) {
						throwValidatorSchemaJSONException("': Not an element of type 'array'");
					}

					return *getProperty(config, condition["value"], patternElem);
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".value");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'value' not defined");
				}
				break;
			}
			case OperatorsJSONConverter::Property_Item: {
				unsigned int itemValue;
				
				try
				{
					if(condition.at("item").is_object()) {
						itemValue = solveOperator(config, condition["item"]);
					}
					else if(condition.at("item").is_number_unsigned()) {
						itemValue = condition["item"];
					}
					else {
						throwValidatorSchemaJSONException("': Not an element of type 'object' or 'unsigned_integer'");
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".item");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'item' not defined");
				}

				try
				{
					if(!condition.at("value").is_array()) {
						throwValidatorSchemaJSONException("': Not an element of type 'array'");
					}

					return (*getProperty(config, condition["value"], patternElem))[itemValue];
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".value");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'value' not defined");
				}
				break;
			}
			case OperatorsJSONConverter::Property_Size: {
				try
				{
					if(!condition.at("value").is_array()) {
						throwValidatorSchemaJSONException("': Not an element of type 'array'");
					}

					return getProperty(config, condition["value"], patternElem)->size();
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".value");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'value' not defined");
				}
				break;
			}
			case OperatorsJSONConverter::Property_Exists: {
				try
				{
					if(!condition.at("value").is_array()) {
						throwValidatorSchemaJSONException("': Not an element of type 'array'");
					}

					try
					{
						getProperty(config, condition["value"], patternElem);
						return true;
					}
					catch(const ValidatorConfigJSONException& e)
					{
						return false;
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".value");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'value' not defined");
				}
				break;
			}
			case OperatorsJSONConverter::Constant: {
				try
				{
					if(condition.at("value").is_array() || condition.at("value").is_object()) {
						throwValidatorSchemaJSONException("': Not an element of type 'boolean', 'integer', 'unsigned_integer', 'float' or 'string'");
					}

					if(patternElem != "" && condition["value"] == PATTERN_ELEM) {
						return patternElem;
					}
					else {
						return condition["value"];
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".value");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'value' not defined");
				}
				break;
			}
			case OperatorsJSONConverter::Subtract: {
				try
				{
					if(!condition.at("result_type").is_string()) {
						throwValidatorSchemaJSONException("': Not an element of type 'string'");
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".result_type");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'result_type' not defined. Valid values are integer, unsigned_integer, float");
				}
				
				try
				{
					switch (TypeElemJSONConverter::stringToEnumValue((string)condition["result_type"])) {
						case TypeElemJSONConverter::Integer: {
							return solveSubtractOperator<int>(config, condition, patternElem);
							break;
						}
						case TypeElemJSONConverter::Unsigned_Integer: {
							return solveSubtractOperator<unsigned int>(config, condition, patternElem);
							break;
						}
						case TypeElemJSONConverter::Float: {
							return solveSubtractOperator<float>(config, condition, patternElem);
							break;
						}
						default: {
							throwValidatorSchemaJSONException(".type': Default case");
							break;
						}
					}
				}
				catch(const std::out_of_range& e) 
				{
					throwValidatorSchemaJSONException(".result_type': Unknown type '" + string(condition["result_type"]) + "'. Valid values are integer, unsigned_integer, float");
				}
				break;
			}
			case OperatorsJSONConverter::Sum: {
				try
				{
					if(!condition.at("result_type").is_string()) {
						throwValidatorSchemaJSONException("': Not an element of type 'string'");
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".result_type");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'result_type' not defined. Valid values are integer, unsigned_integer, float");
				}

				try
				{
					switch (TypeElemJSONConverter::stringToEnumValue((string)condition["result_type"])) {
						case TypeElemJSONConverter::Integer: {
							return solveSumOperator<int>(config, condition, patternElem);
							break;
						}
						case TypeElemJSONConverter::Unsigned_Integer: {
							return solveSumOperator<unsigned int>(config, condition, patternElem);
							break;
						}
						case TypeElemJSONConverter::Float: {
							return solveSumOperator<float>(config, condition, patternElem);
							break;
						}
						default: {
							throwValidatorSchemaJSONException(".type': Default case");
							break;
						}
					}
				}
				catch(const std::out_of_range& e) 
				{
					throwValidatorSchemaJSONException(".result_type': Unknown type '" + string(condition["result_type"]) + "'. Valid values are integer, unsigned_integer, float");
				}
				break;
			}
			case OperatorsJSONConverter::Division: {
				try
				{
					if(!condition.at("result_type").is_string()) {
						throwValidatorSchemaJSONException("': Not an element of type 'string'");
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".result_type");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'result_type' not defined. Valid values are integer, float");
				}

				try
				{
					switch (TypeElemJSONConverter::stringToEnumValue((string)condition["result_type"])) {
						case TypeElemJSONConverter::Integer: {
							return solveDivisionOperator<int>(config, condition, patternElem);
							break;
						}
						case TypeElemJSONConverter::Float: {
							return solveDivisionOperator<float>(config, condition, patternElem);
							break;
						}
						default: {
							throwValidatorSchemaJSONException(".type': Default case");
							break;
						}
					}
				}
				catch(const std::out_of_range& e) 
				{
					throwValidatorSchemaJSONException(".result_type': Unknown type '" + string(condition["result_type"]) + "'. Valid values are integer, float");
				}
				break;
			}
			case OperatorsJSONConverter::Multiply: {
				try
				{
					if(!condition.at("result_type").is_string()) {
						throwValidatorSchemaJSONException("': Not an element of type 'string'");
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".result_type");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'result_type' not defined. Valid values are integer, float");
				}

				try
				{
					switch (TypeElemJSONConverter::stringToEnumValue((string)condition["result_type"])) {
						case TypeElemJSONConverter::Integer: {
							return solveMultiplyOperator<int>(config, condition, patternElem);
							break;
						}
						case TypeElemJSONConverter::Float: {
							return solveMultiplyOperator<float>(config, condition, patternElem);
							break;
						}
						default: {
							throwValidatorSchemaJSONException(".type': Default case");
							break;
						}
					}
				}
				catch(const std::out_of_range& e) 
				{
					throwValidatorSchemaJSONException(".result_type': Unknown type '" + string(condition["result_type"]) + "'. Valid values are integer, float");
				}
				break;
			}
			case OperatorsJSONConverter::Module: {
				return solveModuleOperator(config, condition, patternElem);
				break;
			}
			default: {
				throwValidatorSchemaJSONException(".type': Default case");
				break;
			}
		}
	}
	catch(const std::out_of_range& e) 
	{
		throwValidatorSchemaJSONException(".type': Unknown type '" + string(condition["type"]) + "'. Valid values are " + OperatorsJSONConverter::printValues());
	}
}

bool ValidatorJSON::checkCondition(const json& config, const json& condition, const string patternElem) {
	return solveOperator(config, condition, patternElem);
}

vector<string> ValidatorJSON::generateRegexElems(const string& regex) {
	vector<string> regexElems;
	
	char delimiter = '|';
	size_t pos = 0, last_pos = 0;
    while ((pos = regex.find(delimiter, last_pos)) != string::npos) {
        string token = regex.substr(last_pos, pos - last_pos);
		vector<string> tokenElems = generateRegexElems(token);
        regexElems.insert(regexElems.end(), tokenElems.begin(), tokenElems.end());

		last_pos = pos + 1;
    }

	if(last_pos != 0) {
		string token = regex.substr(last_pos);
		vector<string> tokenElems = generateRegexElems(token);
		regexElems.insert(regexElems.end(), tokenElems.begin(), tokenElems.end());

		return regexElems;
	}
	

	std::regex regexNumbers = std::regex("\\[(\\d+)-(\\d+)\\]");

	smatch matchNumbers;
    if(regex_search(regex, matchNumbers, regexNumbers)) {
        int start_num = stoi(matchNumbers[1].str());
        int end_num = stoi(matchNumbers[2].str());

        string prefix = matchNumbers.prefix().str();
        string suffix = matchNumbers.suffix().str();

        for (int i = start_num; i <= end_num; i++) {
			string token = prefix + to_string(i) + suffix;
			vector<string> tokenElems = generateRegexElems(token);
			regexElems.insert(regexElems.end(), tokenElems.begin(), tokenElems.end());
        }

		return regexElems;
    }


	regexElems.push_back(regex);

	return regexElems;
}

void ValidatorJSON::addPatternProperties(json& schema, const json& patternProperties) {
	for(const auto& [key, value] : (map<string,json>)patternProperties) {
		try
		{
			if(!value.is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}

			for(const string& elem : generateRegexElems(key)) {
				schema["properties"][elem] = value;
			}
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage("." + key);
			throw;
		}
	}
}

void ValidatorJSON::removePatternProperties(map<string,json>& properties, const json& patternProperties) {
	ListRegexKey listPatternProperties;
	listPatternProperties = ListRegexKey(&patternProperties);

	for(const auto& [key, value] : properties) {
		try
		{
			listPatternProperties.findRegexKey(key);
			properties.erase(key);
		}
		catch(const std::out_of_range& e) {}
	}
}

void ValidatorJSON::validateObject(const json& config, json& schema) {
	bool definedProperties = false;
	
	try
	{
		if(!schema.at("patternProperties").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		addPatternProperties(schema, schema["patternProperties"]);

		definedProperties = true;
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".patternProperties");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schema.at("conditionals").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		checkConditionals(schema, schema["conditionals"], config);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".conditionals");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schema.at("patternConditionals").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		checkPatternConditionals(schema, schema["patternConditionals"], config);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".patternConditionals");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schema.at("minProperties").is_number_unsigned()) {
			throwValidatorSchemaJSONException("': Not an element of type 'unsigned_integer'");
		}

		if(config.size() < schema["minProperties"]) {
			throwValidatorConfigJSONException("': Number of items less than minimum");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".minProperties");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schema.at("maxProperties").is_number_unsigned()) {
			throwValidatorSchemaJSONException("': Not an element of type 'unsigned_integer'");
		}

		if(config.size() > schema["maxProperties"]) {
			throwValidatorConfigJSONException("': Number of items greater than maximum");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".maxProperties");
		throw;
	}
	catch(const json::out_of_range& e) {}


	map<string, bool> required;

	try
	{
		if(!schema.at("properties").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		definedProperties = true;

		for(const auto& [key, value] : (map<string, json>)schema["properties"]) {
			try
			{
				try
				{
					if(!value.at("required").is_boolean()) {
						throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
					}

					if(value["required"]) {
						required[key] = false;
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".required");
					throw;
				}
				catch(const json::out_of_range& e) 
				{
					throwValidatorSchemaJSONException("': Property 'required' not defined");
				}
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage("." + key);
				throw;
			}
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".properties");
		throw;
	}
	catch(const json::out_of_range& e) {}

	if(!definedProperties) {
		throwValidatorSchemaJSONException("': Not defined properties");
	}

	bool additionalProperties;
	try
	{
		if(!schema.at("additionalProperties").is_boolean()) {
			throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
		}

		additionalProperties = schema["additionalProperties"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".additionalProperties");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		throwValidatorSchemaJSONException("': Property 'additionalProperties' not defined");
	}

	try
	{
		for(const auto& [key, value] : config.items()) {
			try
			{
				validateElem(value, schema["properties"].at(key));
			}
			catch(ValidatorJSONException& e)
			{
				e.addPreMessage("." + key);
				throw;
			}
			catch(const json::out_of_range& e)
			{	
				throwValidatorConfigJSONException("': Item '" + key + "' not defined in schema");
			}

			if(isRequired(required, key)) {
				required[key] = true;
			}
			else{
				if(!additionalProperties) {
					throwValidatorConfigJSONException("': Additional item '" + key + "' not allowed");
				}
			}
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".properties");
		throw;
	}

	for(const auto& [key, value] : required) {
		if(!value) {
			throwValidatorConfigJSONException("': Required item '" + key + "' not found");
		}
	}

	try
	{
		if(!schema.at("requiredConditions").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		for(const json &requiredCondition : schema["requiredConditions"]) {
			if(!checkCondition(config, requiredCondition["condition"])) {
				throwValidatorConfigJSONException("': " + string(requiredCondition["error"]));
			}
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".requiredConditions");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schema.at("warningConditions").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		for(const json &warningCondition : schema["warningConditions"]) {
			if(checkCondition(config, warningCondition["condition"])) {
				cout << "[WARNING] " << (string)warningCondition["warning"] << endl;
				warningsFile << "[WARNING] " << (string)warningCondition["warning"] << endl;
			}
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".warningConditions");
		throw;
	}
	catch(const json::out_of_range& e) {}
}

void ValidatorJSON::validateArray(const json& config, json& schema) {
	try
	{
		if(!schema.at("minItems").is_number_unsigned()) {
			throwValidatorSchemaJSONException("': Not an element of type 'unsigned_integer'");
		}

		if(config.size() < schema["minItems"]) {
			throwValidatorConfigJSONException("': Number of items less than minimum");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".minItems");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schema.at("maxItems").is_number_unsigned()) {
			throwValidatorSchemaJSONException("': Not an element of type 'unsigned_integer'");
		}

		if(config.size() > schema["maxItems"]) {
			throwValidatorConfigJSONException("': Number of items greater than maximum");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".maxItems");
		throw;
	}
	catch(const json::out_of_range& e) {}

	bool definedItems = false;

	size_t numPrefixItems;
	try
	{
		if(!schema.at("prefixItems").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		numPrefixItems = schema["prefixItems"].size();

		definedItems = true;
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".prefixItems");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		numPrefixItems = 0;
	}

	bool additionalItems;
	try
	{
		if(!schema.at("items").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}
		
		additionalItems = true;

		definedItems = true;
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".items");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		additionalItems = false;
	}

	if(!definedItems) {
		throwValidatorSchemaJSONException("': Not defined items");
	}

	bool requiredAll;
	try
	{
		if(!schema.at("requiredAll").is_boolean()) {
			throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
		}

		requiredAll = schema["requiredAll"];
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".requiredAll");
		throw;
	}
	catch(const json::out_of_range& e) 
	{
		requiredAll = false;
	}

	if(!additionalItems && config.size() > numPrefixItems) {
		throwValidatorConfigJSONException("': Too many items in the array");
	}

	if(requiredAll && config.size() < numPrefixItems) {
		throwValidatorConfigJSONException("': Missing required items");
	}

	ItemsOrderConverter::ItemsOrder itemsOrder;
	bool indicatedItemsOrder;
	try
	{
		if(!schema.at("itemsOrder").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}

		itemsOrder = ItemsOrderConverter::stringToEnumValue(string(schema["itemsOrder"]));
		indicatedItemsOrder = true;
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".itemsOrder");
		throw;
	}
	catch(const std::out_of_range& e) 
	{
		throwValidatorSchemaJSONException(".itemsOrder': Unknown type '" + string(schema["itemsOrder"]) + "'. Valid values are " + ItemsOrderConverter::printValues());
	}
	catch(const json::out_of_range& e) 
	{
		indicatedItemsOrder = false;
	}

	for(size_t i = 0; i < config.size(); i++) {
		if(i < numPrefixItems) {
			try
			{
				try
				{
					validateElem(config[i], schema["prefixItems"][i]);
				}
				catch(ValidatorJSONException& e)
				{
					e.addPreMessage(".item_" + to_string(i));
					throw;
				}
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".prefixItems");
				throw;
			}
		}
		else {
			try
			{
				validateElem(config[i], schema["items"]);
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".items");
				throw;
			}
			catch(ValidatorConfigJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}

			if(indicatedItemsOrder && i > numPrefixItems) {
				switch (itemsOrder)
				{
					case ItemsOrderConverter::Increasing: {
						if(config[i-1] > config[i]) {
							throwValidatorConfigJSONException("': Items not arranged in increasing order");
						}
						break;
					}
					case ItemsOrderConverter::Strictly_Increasing: {
						if(config[i-1] >= config[i]) {
							throwValidatorConfigJSONException("': Items not arranged in strictly increasing order");
						}
						break;
					}
					case ItemsOrderConverter::Decreasing: {
						if(config[i-1] < config[i]) {
							throwValidatorConfigJSONException("': Items not arranged in decreasing order");
						}
						break;
					}
					case ItemsOrderConverter::Strictly_Decreasing: {
						if(config[i-1] <= config[i]) {
							throwValidatorConfigJSONException("': Items not arranged in strictly decreasing order");
						}
						break;
					}
					default: {
						break;
					}
				}
			}
		}
	}
}

void ValidatorJSON::validateString(const json& config, json& schema) {
	try
	{
		if(!schema.at("enum").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		if(schema["enum"].size() == 0) {
			throwValidatorSchemaJSONException("': Must be an array with at least one element");
		}

		bool found = false;
		for(size_t i = 0; i < schema["enum"].size() && !found; i++) {
			try
			{
				if(!schema["enum"][i].is_string()) {
					throwValidatorSchemaJSONException("': Not an element of type 'string'");
				}

				if(schema["enum"][i] == config) {
					found = true;
				}
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}

		if(!found) {
			ostringstream enum_values;
			for(size_t i = 0; i < schema["enum"].size(); i++) {
				enum_values << schema["enum"][i];
				if (i != schema["enum"].size() - 1) {
					enum_values << ", ";
				}
			}
			
			throwValidatorConfigJSONException("': Value is not among the allowed values: [" + enum_values.str() + "]");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".enum");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schema.at("minLength").is_number_unsigned()) {
			throwValidatorSchemaJSONException("': Not an element of type 'unsigned_integer'");
		}

		if(((string)config).length() < schema["minLength"]) {
			throwValidatorConfigJSONException("': Value length less than minimum");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".minLength");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schema.at("maxLength").is_number_unsigned()) {
			throwValidatorSchemaJSONException("': Not an element of type 'unsigned_integer'");
		}

		if(((string)config).length() > schema["maxLength"]) {
			throwValidatorConfigJSONException("': Value length greater than maximum");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".maxLength");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schema.at("pattern").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}

		regex pattern((string)schema["pattern"]);
		if(!regex_match((string)config, pattern)) {
			throwValidatorConfigJSONException("': Value does not meet defined pattern");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".pattern");
		throw;
	}
	catch(const json::out_of_range& e) {}
}

void ValidatorJSON::validateIntegerNumber(const json& config, json& schema) {
	try
	{
		try
		{
			if(!schema.at("minimum").is_number_unsigned()) {
				throwValidatorSchemaJSONException("': Not an element of type 'unsigned_integer'");
			}
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".minimum");
			throw;
		}

		bool exclusiveMinimum;
		try
		{
			if(!schema.at("exclusiveMinimum").is_boolean()) {
				throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
			}

			exclusiveMinimum = schema.at("exclusiveMinimum");
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".exclusiveMinimum");
			throw;
		}
		catch(const json::out_of_range& e)
		{
			exclusiveMinimum = false;
		}
		
		if( (exclusiveMinimum && (int)config <= schema["minimum"]) || (!exclusiveMinimum && (int)config < schema["minimum"]) ) {
			throwValidatorConfigJSONException("': Value less than " + string((exclusiveMinimum) ? "or equal " : "") + "minimum");
		}
	}
	catch(const json::out_of_range& e) {}

	try
	{
		try
		{
			if(!schema.at("maximum").is_number_unsigned()) {
				throwValidatorSchemaJSONException("': Not an element of type 'unsigned_integer'");
			}
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".maximum");
			throw;
		}

		bool exclusiveMaximum;
		try
		{
			if(!schema.at("exclusiveMaximum").is_boolean()) {
				throwValidatorSchemaJSONException("': Not an element of type 'boolean'");
			}

			exclusiveMaximum = schema.at("exclusiveMaximum");
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".exclusiveMaximum");
			throw;
		}
		catch(const json::out_of_range& e)
		{
			exclusiveMaximum = false;
		}
		
		if( (exclusiveMaximum && (int)config >= schema["maximum"]) || (!exclusiveMaximum && (int)config > schema["maximum"]) ) {
			throwValidatorConfigJSONException("': Value greater than " + string((exclusiveMaximum) ? "or equal " : "") + "maximum");
		}
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schema.at("multipleOf").is_number_integer()) {
			throwValidatorSchemaJSONException("': Not an element of type 'integer'");
		}

		if(schema["multipleOf"] == 0) {
			throwValidatorSchemaJSONException("': Must have a non-zero integer value");
		}
		
		if((int)config % (int)schema["multipleOf"] != 0) {
			throwValidatorConfigJSONException("': Value is not a multiple of " + to_string(schema["multipleOf"]));
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".multipleOf");
		throw;
	}
	catch(const json::out_of_range& e) {}
}

void ValidatorJSON::validateInteger(const json& config, json& schema) {
	validateIntegerNumber(config, schema);
}

void ValidatorJSON::validateUnsignedInteger(const json& config, json& schema) {
	validateIntegerNumber(config, schema);
}

void ValidatorJSON::validateFloat(const json& config, json& schema) {
	try
	{
		if(!schema.at("minimum").is_number_float()) {
			throwValidatorSchemaJSONException("': Not an element of type 'float'");
		}

		if((float)config < schema["minimum"]) {
			throwValidatorConfigJSONException("': Value less than minimum");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".minimum");
		throw;
	}
	catch(const json::out_of_range& e) {}

	try
	{
		if(!schema.at("maximum").is_number_float()) {
			throwValidatorSchemaJSONException("': Not an element of type 'float'");
		}
		
		if((float)config > schema["maximum"]) {
			throwValidatorConfigJSONException("': Value greater than maximum");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".maximum");
		throw;
	}
	catch(const json::out_of_range& e) {}
}

void ValidatorJSON::validateBoolean(const json& config, json& schema) {
	
}

void ValidatorJSON::validateElem(const json& config, json& schema) {
	try
	{
		if(!schema.at("type").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".type");
		throw;
	}
	catch(const json::out_of_range& e)
	{
		throwValidatorSchemaJSONException("': Property 'type' not defined. Valid values are " + TypeElemJSONConverter::printValues());
	}

	try
	{
		switch (TypeElemJSONConverter::stringToEnumValue(string(schema["type"])))
		{
			case TypeElemJSONConverter::Object: {
				if(!config.is_object()) {
					throwValidatorConfigJSONException("': Not an element of type '" + string(schema["type"]) + "'");
				}

				validateObject(config, schema);
				break;
			}
			case TypeElemJSONConverter::Array: {
				if(!config.is_array()) {
					throwValidatorConfigJSONException("': Not an element of type '" + string(schema["type"]) + "'");
				}

				validateArray(config, schema);
				break;
			}
			case TypeElemJSONConverter::String: {
				if(!config.is_string()) {
					throwValidatorConfigJSONException("': Not an element of type '" + string(schema["type"]) + "'");
				}

				validateString(config, schema);
				break;
			}
			case TypeElemJSONConverter::Integer: {
				if(!config.is_number_integer()) {
					throwValidatorConfigJSONException("': Not an element of type '" + string(schema["type"]) + "'");
				}

				validateInteger(config, schema);
				break;
			}
			case TypeElemJSONConverter::Unsigned_Integer: {
				if(!config.is_number_unsigned()) {
					throwValidatorConfigJSONException("': Not an element of type '" + string(schema["type"]) + "'");
				}

				validateUnsignedInteger(config, schema);
				break;
			}
			case TypeElemJSONConverter::Float: {
				if(!config.is_number_float()) {
					throwValidatorConfigJSONException("': Not an element of type '" + string(schema["type"]) + "'");
				}

				validateFloat(config, schema);
				break;
			}
			case TypeElemJSONConverter::Boolean: {
				if(!config.is_boolean()) {
					throwValidatorConfigJSONException("': Not an element of type '" + string(schema["type"]) + "'");
				}

				validateBoolean(config, schema);
				break;
			}
			default: {
				throwValidatorSchemaJSONException(".type': Default case");
				break;
			}
		}
	}
	catch(const std::out_of_range& e) 
	{
		throwValidatorSchemaJSONException(".type': Unknown type '" + string(schema["type"]) + "'. Valid values are " + TypeElemJSONConverter::printValues());
	}
}

void ValidatorJSON::validateJSON(const json& config, json& schema) {
	try
	{
		validateElem(config, schema);
	}
	catch(ValidatorJSONException& e)
	{
		e.addPreMessage("'root");
		throw;
	}
}
