#include "Misc/JsonValidator/Elements/Object.h"


using namespace std;
using json = nlohmann::json;


Object::Object(const json& info)
    : Element(info)
{
    try
	{
		#ifdef DEBUG
		if(!info.at("properties").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}
		#endif

		for(const auto& [key, value] : info.at("properties").items()) {
			try
			{
				properties.insert({key, ElementFactory::createInstance(value)});
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
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'properties' not defined");
	}


    try
	{
		#ifdef DEBUG
		if(!info.at("patternProperties").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}
		#endif

		for(const auto& [key, value] : info.at("patternProperties").items()) {
			try
			{
				#ifdef DEBUG
				try
				{
					if(!value.at("pattern").is_string()) {
						throwValidatorSchemaJSONException("': Not an element of type 'string'");
					}


					try
					{
						value.at("enum");

						throwValidatorSchemaJSONException("': Property 'pattern' and 'enum' are mutually exclusive");
					}
					catch(const json::out_of_range&) {}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".pattern");
					throw;
				}
				catch(const json::out_of_range&) 
				{
					try
					{
						if(!value.at("enum").is_array()) {
							throwValidatorSchemaJSONException("': Not an element of type 'array'");
						}

						for(size_t i = 0; i < value["enum"].size(); i++) {
							try
							{
								if(!value["enum"][i].is_string()) {
									throwValidatorSchemaJSONException("': Not an element of type 'string'");
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
						e.addPreMessage(".enum");
						throw;
					}
					catch(const json::out_of_range&) 
					{
						throwValidatorSchemaJSONException("': Property 'pattern' or 'enum' not defined");
					}
				}
				#endif


				try
				{
					patternProperties.insert({static_cast<string>(value.at("pattern")), ElementFactory::createInstance(value)});
				}
				catch(const json::out_of_range&) 
				{
					for(const string propertyName : value["enum"])
					{
						try
						{
							properties.insert({propertyName, ElementFactory::createInstance(value)});
						}
						catch(ValidatorSchemaJSONException& e)
						{
							e.addPreMessage("." + propertyName);
							throw;
						}
					}
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
		e.addPreMessage(".patternProperties");
		throw;
	}
	catch(const json::out_of_range&) {}


    try
	{
		#ifdef DEBUG
		if(!info.at("conditionals").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}
		
		for(size_t i = 0; i < conditionals.size(); i++) {
			try
			{
				if(!conditionals.at(i).is_object()) {
					throwValidatorSchemaJSONException("': Not an element of type 'object'");
				}

				verifyConditional(conditionals.at(i));
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}
		#endif
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".conditionals");
		throw;
	}
	catch(const json::out_of_range&) {}


	try
	{
		#ifdef DEBUG
		if(!info.at("patternConditionals").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}
		
		for(size_t i = 0; i < patternConditionals.size(); i++) {
			try
			{
				if(!patternConditionals.at(i).is_object()) {
					throwValidatorSchemaJSONException("': Not an element of type 'object'");
				}


				try
				{
					if(!patternConditionals.at(i).at("pattern").is_string()) {
						throwValidatorSchemaJSONException("': Not an element of type 'string'");
					}
				}
				catch(ValidatorSchemaJSONException& e)
				{
					e.addPreMessage(".pattern");
					throw;
				}
				catch(const json::out_of_range&) 
				{
					throwValidatorSchemaJSONException("': Property 'pattern' not defined");
				}


				verifyConditional(patternConditionals.at(i));
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}
		#endif
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".patternConditionals");
		throw;
	}
	catch(const json::out_of_range&) {}


	try
	{
		#ifdef DEBUG
		if(!info.at("requiredConditions").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}
		
		for(size_t i = 0; i < requiredConditions.size(); i++) {
			try
			{
				if(!requiredConditions.at(i).is_object()) {
					throwValidatorSchemaJSONException("': Not an element of type 'object'");
				}

				verifyRequiredCondition(requiredConditions.at(i));
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}
		#endif
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".requiredConditions");
		throw;
	}
	catch(const json::out_of_range&) {}


	try
	{
		#ifdef DEBUG
		if(!info.at("warningConditions").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}
		
		for(size_t i = 0; i < warningConditions.size(); i++) {
			try
			{
				if(!warningConditions.at(i).is_object()) {
					throwValidatorSchemaJSONException("': Not an element of type 'object'");
				}

				verifyWarningCondition(warningConditions.at(i));
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}
		#endif
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".warningConditions");
		throw;
	}
	catch(const json::out_of_range&) {}
}

Object::Object(const Object& other, const bool nullProperty)
	: Element(nullProperty, other.getDescription()), properties(other.getProperties()), patternProperties(other.getPatternProperties()), 
	  conditionals(other.getConditionals()), patternConditionals(other.getPatternConditionals()),
	  requiredConditions(other.getRequiredConditions()), warningConditions(other.getWarningConditions())
{

}

Object::~Object()
{
	if(!isModified())
	{
		for(auto& [key, value] : properties) {
			delete value;
		}

		for(auto& [key, value] : patternProperties) {
			delete value;
		}
	}
}

void Object::verifyPropertyPath(const std::vector<std::string>& path, const unsigned int currentIndex) const
{
	if(currentIndex >= path.size())
	{
		throwValidatorSchemaJSONException("': Incorrect property path");
	}

	properties.at(path[currentIndex])->verifyPropertyPath(path, currentIndex+1);
}

void Object::applyConditionalBlock(const unordered_map<string, json>& conditionalBlock, unordered_map<string, Element*>& modifiedProperties, const bool patternReplacement, const string& replacementValue) const
{
	string elementName;
	const Element* workingPath = this;

	try
	{
		for(const json& pathElem : conditionalBlock.at("workingPath"))
		{
			tie(workingPath, elementName) = workingPath->getProperty(pathElem, patternReplacement, replacementValue);
		}
	}
	catch(const json::out_of_range&) {}


	const Element* property;
	string propertyName;

	try
	{
		for(const string name : conditionalBlock.at("remove").at("nullProperty"))
		{
			tie(property, propertyName) = workingPath->getProperty(name, patternReplacement, replacementValue);

			if(property->isNullProperty())
			{
				modifiedProperties.emplace(propertyName, ElementFactory::createInstance(*property, false));
			}
		}
	}
	catch(const json::out_of_range&) {}


	try
	{
		for(const string name : conditionalBlock.at("add").at("nullProperty"))
		{
			tie(property, propertyName) = workingPath->getProperty(name, patternReplacement, replacementValue);

			if(!property->isNullProperty())
			{
				modifiedProperties.emplace(propertyName, ElementFactory::createInstance(*property, true));
			}
		}
	}
	catch(const json::out_of_range&) {}
}

void Object::applyConditionalBlock(const unordered_map<string, json>& conditionalBlock, unordered_map<string, Element*>& modifiedProperties) const
{
	applyConditionalBlock(conditionalBlock, modifiedProperties, false, "");
}

void Object::validateElement(const json& config) const
{
	if(!config.is_object()) {
        throwValidatorConfigJSONException("': Not an element of type 'object'");
    }

	
	for(const auto& [key, value] : properties) {
        try
        {
            config.at(key);
        }
        catch(const std::out_of_range&)
        {	
            throwValidatorConfigJSONException("': Required item '" + key + "' not found");
        }
    }


	unordered_map<string, Element*> matchedProperties;

	for(const auto& [key, value] : static_cast<const unordered_map<string, json>&>(config)) {
		bool matched = false;
		string matchedPattern;

		for(const auto& [patternStr, patternProperty] : patternProperties) {
			regex pattern(patternStr);

			if(regex_match(key, pattern))
			{
				if(matched)
				{
					throwValidatorConfigJSONException("." + key + "': Conflict between pattern property \"" + matchedPattern + "\" and \"" + patternStr + "\"");
				}

				matched = true;
				matchedPattern = patternStr;
				matchedProperties[key] = patternProperty;
			}
		}

		if(!matched)
		{
			try
			{
				(void)properties.at(key);
			}
			catch(const std::out_of_range&)
			{
				throwValidatorConfigJSONException("': Item '" + key + "' not defined in schema");
			}
		}
	}


	unordered_map<string, Element*> modifiedProperties;

	try
	{
		for(size_t i = 0; i < conditionals.size(); i++) {
			try
			{
				try
				{
					if(static_cast<const bool>(checkOperator(conditionals.at(i)["condition"], config, getProperties())))
					{
						applyConditionalBlock(conditionals.at(i)["then"], modifiedProperties);
					}
					else
					{
						try
						{
							applyConditionalBlock(conditionals.at(i).at("else"), modifiedProperties);
						}
						catch(const json::out_of_range&) {}
					}
				}
				catch(ValidatorConfigJSONException& e)
				{
					e.addPreMessage(".condition");
					throw;
				}
			}
			catch(ValidatorConfigJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}
	}
	catch(ValidatorConfigJSONException& e)
	{
		e.addPreMessage(".conditionals");
		throw;
	}


	for(const auto& [key, value] : static_cast<const unordered_map<string, json>&>(config)) {
		try
		{
			for(size_t i = 0; i < patternConditionals.size(); i++) {
				try
				{
					regex pattern(patternConditionals.at(i)["pattern"]);

					if(regex_match(key, pattern))
					{
						try
						{
							if(static_cast<const bool>(checkOperator(patternConditionals.at(i)["condition"], config, getProperties(), true, key)))
							{
								applyConditionalBlock(patternConditionals.at(i)["then"], modifiedProperties, true, key);
							}
							else
							{
								try
								{
									applyConditionalBlock(patternConditionals.at(i).at("else"), modifiedProperties, true, key);
								}
								catch(const json::out_of_range&) {}
							}
						}
						catch(ValidatorConfigJSONException& e)
						{
							e.addPreMessage(".condition");
							throw;
						}
					}
				}
				catch(ValidatorConfigJSONException& e)
				{
					e.addPreMessage(".item_" + to_string(i));
					throw;
				}
			}
		}
		catch(ValidatorConfigJSONException& e)
		{
			e.addPreMessage(".patternConditionals");
			throw;
		}
	}


	for(const auto& [key, value] : static_cast<const unordered_map<string, json>&>(config)) {
        try
        {
			try
			{
				modifiedProperties.at(key)->validate(value);
			}
			catch(const std::out_of_range&)
			{	
				try
				{
					matchedProperties.at(key)->validate(value);
				}
				catch(const std::out_of_range&)
				{
					properties.at(key)->validate(value);
				}
			}
        }
        catch(ValidatorConfigJSONException& e)
        {
            e.addPreMessage("." + key);
            throw;
        }
    }


	try
	{
		for(size_t i = 0; i < requiredConditions.size(); i++) {
			try
			{
				try
				{
					if(checkOperator(requiredConditions.at(i)["condition"], config, getProperties())) {
						throwValidatorConfigJSONException("': " + string(requiredConditions.at(i)["error"]));
					}
				}
				catch(ValidatorConfigJSONException& e)
				{
					e.addPreMessage(".condition");
					throw;
				}
			}
			catch(ValidatorConfigJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}
	}
	catch(ValidatorConfigJSONException& e)
	{
		e.addPreMessage(".requiredConditions");
		throw;
	}


	try
	{
		for(size_t i = 0; i < warningConditions.size(); i++) {
			try
			{
				try
				{
					if(checkOperator(warningConditions.at(i)["condition"], config, getProperties())) {
						throwValidatorConfigJSONException("': " + string(warningConditions.at(i)["warning"]));
					}
				}
				catch(ValidatorConfigJSONException& e)
				{
					e.addPreMessage(".condition");
					throw;
				}
			}
			catch(ValidatorConfigJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}
	}
	catch(ValidatorConfigJSONException& e)
	{
		e.addPreMessage(".warningConditions");
		throw;
	}


	for(const auto& [key, value] : modifiedProperties) {
		delete value;
	}
}

void Object::verifyConditionalBlockContent(const json& content) const
{
	try
	{
		if(!content.at("nullProperty").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		for(size_t i = 0; i < content["nullProperty"].size(); i++) {
			try
			{
				if(!content["nullProperty"][i].is_string()) {
					throwValidatorSchemaJSONException("': Not an element of type 'string'");
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
		e.addPreMessage(".nullProperty");
		throw;
	}
	catch(const json::out_of_range&) {}
}

void Object::verifyConditionalBlock(const json& conditionalBlock) const
{
	try
	{
		if(!conditionalBlock.at("workingPath").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}


		if(conditionalBlock["workingPath"].empty()) {
			throwValidatorSchemaJSONException("': Path is empty");
		}


		for(size_t i = 0; i < conditionalBlock["workingPath"].size(); i++) {
			try
			{
				if(!conditionalBlock["workingPath"][i].is_string()) {
					throwValidatorSchemaJSONException("': Not an element of type 'string'");
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
		e.addPreMessage(".workingPath");
		throw;
	}
	catch(const json::out_of_range&) {}


	try
	{
		try
		{
			if(!conditionalBlock.at("remove").is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}

			verifyConditionalBlockContent(conditionalBlock["remove"]);
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".remove");
			throw;
		}
	}
	catch(const json::out_of_range&) {}


	try
	{
		try
		{
			if(!conditionalBlock.at("add").is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}

			verifyConditionalBlockContent(conditionalBlock["add"]);
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".add");
			throw;
		}
	}
	catch(const json::out_of_range&) {}
}

void Object::verifyConditional(const json& conditional) const
{
	try
	{
		if(!conditional.at("condition").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		verifyOperator(conditional["condition"], properties);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".condition");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'condition' not defined");
	}


	try
	{
		if(!conditional.at("then").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		for(size_t j = 0; j < conditional["then"].size(); j++) {
			try
			{
				if(!conditional["then"][j].is_object()) {
					throwValidatorSchemaJSONException("': Not an element of type 'object'");
				}

				verifyConditionalBlock(conditional["then"][j]);
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(j));
				throw;
			}
		}


		try
		{
			if(!conditional.at("else").is_array()) {
				throwValidatorSchemaJSONException("': Not an element of type 'array'");
			}

			for(size_t j = 0; j < conditional["else"].size(); j++) {
				try
				{
					if(!conditional["else"][j].is_object()) {
						throwValidatorSchemaJSONException("': Not an element of type 'object'");
					}

					verifyConditionalBlock(conditional["else"][j]);
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
		catch(const json::out_of_range&) {}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".then");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'then' not defined");
	}
}

void Object::verifyRequiredCondition(json& condition) const
{
	try
	{
		if(!condition.at("condition").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		verifyOperator(condition["condition"], properties);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".condition");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'condition' not defined");
	}


	try
	{
		if(!condition.at("error").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".error");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'error' not defined");
	}
}

void Object::verifyWarningCondition(json& condition) const
{
	try
	{
		if(!condition.at("condition").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		verifyOperator(condition["condition"], properties);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".condition");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'condition' not defined");
	}


	try
	{
		if(!condition.at("warning").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".warning");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'warning' not defined");
	}
}

pair<const Element* const, string> Object::getProperty(const string& propertyName, const bool patternReplacement, const string& replacementValue) const
{
	if(patternReplacement)
	{
		if(propertyName == PATTERN_KEY)
		{
			return make_pair<>(properties.at(replacementValue), replacementValue);
		}
		else
		{
			return make_pair<>(properties.at(propertyName), propertyName);
		}
	}
	else
	{
		return make_pair<>(properties.at(propertyName), propertyName);
	}
}

Element::Type Object::getType() const
{
	return Type::object;
}

const unordered_map<string, Element*>& Object::getProperties() const
{
	return properties;
}

const unordered_map<string, Element*>& Object::getPatternProperties() const
{
	return patternProperties;
}

const vector<json>& Object::getConditionals() const
{
	return conditionals;
}

const vector<json>& Object::getPatternConditionals() const
{
	return patternConditionals;
}

const vector<json>& Object::getRequiredConditions() const
{
	return requiredConditions;
}

const vector<json>& Object::getWarningConditions() const
{
	return warningConditions;
}
