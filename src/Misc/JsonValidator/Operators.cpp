#include "Misc/JsonValidator/Operators.h"


using namespace std;
using json = nlohmann::json;


void verifyIfElseOperator(const json& condition, const unordered_map<string, Element*>& properties)
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
        if(!condition.at("then").is_object()) {
            throwValidatorSchemaJSONException("': Not an element of type 'object'");
        }

        verifyOperator(condition["then"], properties);


        try
		{
			if(!condition.at("else").is_object()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object'");
			}

            verifyOperator(condition["else"], properties);
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

void verifyComparisonOperator(const json& condition, const unordered_map<string, Element*>& properties)
{
	try
	{
		if(!condition.at("comparisonType").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".comparisonType");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'comparisonType' not defined. Valid values are " + EnumClass<ComparisonType>::printValues());
	}


    try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		verifyOperator(condition["lhs"], properties);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}


	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		verifyOperator(condition["rhs"], properties);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}
}

void verifyLogicalOperator(const json& condition, const unordered_map<string, Element*>& properties)
{
    try
	{
		if(!condition.at("logicalType").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".logicalType");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'logicalType' not defined. Valid values are " + EnumClass<LogicalType>::printValues());
	}


    try
	{
		if(!condition.at("operands").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}


        if(EnumClass<LogicalType>::stringToEnumValue(condition["type"]) == LogicalType::negation)
        {
            if(condition["operands"].size() > 1)
            {
                throwValidatorSchemaJSONException("': Negation requires only one operand");
            }
        }


		for(size_t i = 0; i < condition["operands"].size(); i++) {
			try
			{
				if(!condition["operands"][i].is_object()) {
					throwValidatorSchemaJSONException("': Not an element of type 'object'");
				}

                verifyOperator(condition["operands"][i], properties);
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
		e.addPreMessage(".operands");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'operands' not defined");
	}
}

void verifyPropertyOperator(const json& condition, const unordered_map<string, Element*>& properties)
{
    try
	{
		if(!condition.at("propertyElement").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".propertyElement");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'propertyElement' not defined. Valid values are " + EnumClass<PropertyElement>::printValues());
	}


    try
	{
		if(!condition.at("relativePath").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}


		if(condition["relativePath"].empty()) {
			throwValidatorSchemaJSONException("': Path is empty");
		}


		for(size_t i = 0; i < condition["relativePath"].size(); i++) {
			try
			{
				if(!condition["relativePath"][i].is_string()) {
					throwValidatorSchemaJSONException("': Not an element of type 'string'");
				}
			}
			catch(ValidatorSchemaJSONException& e)
			{
				e.addPreMessage(".item_" + to_string(i));
				throw;
			}
		}


		properties.at(condition["relativePath"][0])->verifyPropertyPath(condition["relativePath"], 1);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".relativePath");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'relativePath' not defined");
	}


	if(EnumClass<PropertyElement>::stringToEnumValue(condition["propertyElement"]) == PropertyElement::item)
	{
		try
		{
			if(!condition.at("selectedItem").is_object() && !condition.at("selectedItem").is_number_unsigned()) {
				throwValidatorSchemaJSONException("': Not an element of type 'object' or 'unsigned_integer'");
			}

			verifyOperator(condition["selectedItem"], properties);
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".selectedItem");
			throw;
		}
		catch(const json::out_of_range&) 
		{
			throwValidatorSchemaJSONException("': Property 'selectedItem' not defined");
		}
	}
}

void verifyConstantOperator(const json& condition)
{
	try
	{
		if(condition.at("value").is_array() || condition.at("value").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'boolean', 'integer', 'floater', 'string' or 'null'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".value");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'value' not defined");
	}
}

void verifyArithmeticalOperator(const json& condition, const unordered_map<string, Element*>& properties)
{
	try
	{
		if(!condition.at("arithmeticalType").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".arithmeticalType");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'arithmeticalType' not defined. Valid values are " + EnumClass<ArithmeticalType>::printValues());
	}


	try
	{
		if(!condition.at("lhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		verifyOperator(condition["lhs"], properties);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'lhs' not defined");
	}


	try
	{
		if(!condition.at("rhs").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}

		verifyOperator(condition["rhs"], properties);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'rhs' not defined");
	}
}

void verifyOperator(const json& condition, const unordered_map<string, Element*>& properties)
{
    try
	{
		if(!condition.at("operatorType").is_string()) {
			throwValidatorSchemaJSONException("': Not an element of type 'string'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".operatorType");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'operatorType' not defined. Valid values are " + EnumClass<OperatorType>::printValues());
	}
	
	
    switch(EnumClass<OperatorType>::stringToEnumValue(condition["operatorType"]))
	{
		case OperatorType::if_else: {
			verifyIfElseOperator(condition, properties);
			break;
		}
		case OperatorType::comparison: {
			verifyComparisonOperator(condition, properties);
			break;
		}
		case OperatorType::logical: {
			verifyLogicalOperator(condition, properties);
			break;
		}
		case OperatorType::property: {
			verifyPropertyOperator(condition, properties);
			break;
		}
		case OperatorType::constant: {
			verifyConstantOperator(condition);
			break;
		}
		case OperatorType::arithmetical: {
			verifyArithmeticalOperator(condition, properties);
			break;
		}
		default: {
			throwValidatorSchemaJSONException(".operatorType': Default case");
			break;
		}
	}
}

const json getProperty(const json& config, const vector<string>& relativePath, const bool patternReplacement, const string& replacementValue)
{
	json property = config;

	for(const string& pathElem : relativePath)
	{
		try
		{
			if(patternReplacement)
			{
				if(pathElem == PATTERN_KEY)
				{
					property = property.at(replacementValue);
				}
				else
				{
					property = property.at(pathElem);
				}
			}
			else
			{
				property = property.at(pathElem);
			}
		}
		catch(const json::out_of_range&) 
		{
			throwValidatorConfigJSONException("': Incorrect property path");
		}
	}

	return property;
}

const json checkIfElseOperator(const unordered_map<string, json>& condition, const json& config, const unordered_map<string, Element*>& properties, const bool patternReplacement, const string& replacementValue)
{
	json conditionResult;
	try
	{
		conditionResult = checkOperator(condition.at("condition"), config, properties, patternReplacement, replacementValue);
	}
	catch(ValidatorConfigJSONException& e)
	{
		e.addPreMessage(".condition");
		throw;
	}

	if(conditionResult)
	{
		try
		{
			return checkOperator(condition.at("then"), config, properties, patternReplacement, replacementValue);
		}
		catch(ValidatorConfigJSONException& e)
		{
			e.addPreMessage(".then");
			throw;
		}
	}
	else
	{
		try
		{
			return checkOperator(condition.at("else"), config, properties, patternReplacement, replacementValue);
		}
		catch(ValidatorConfigJSONException& e)
		{
			e.addPreMessage(".else");
			throw;
		}
		catch(const std::out_of_range&) {}
	}

	return conditionResult;
}

const json checkComparisonOperator(const unordered_map<string, json>& condition, const json& config, const unordered_map<string, Element*>& properties, const bool patternReplacement, const string& replacementValue)
{
	json lhs;

	try
	{
		lhs = checkOperator(condition.at("lhs"), config, properties, patternReplacement, replacementValue);
	}
	catch(ValidatorConfigJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}

	json rhs;

	try
	{
		rhs = checkOperator(condition.at("rhs"), config, properties, patternReplacement, replacementValue);
	}
	catch(ValidatorConfigJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}

	switch(EnumClass<ComparisonType>::stringToEnumValue(condition.at("comparisonType")))
	{
		case ComparisonType::equal: {
			return lhs == rhs;
			break;
		}
		case ComparisonType::not_equal: {
			return lhs != rhs;
			break;
		}
		case ComparisonType::less_than: {
			return lhs < rhs;
			break;
		}
		case ComparisonType::less_equal_than: {
			return lhs <= rhs;
			break;
		}
		case ComparisonType::greater_than: {
			return lhs > rhs;
			break;
		}
		case ComparisonType::greater_equal_than: {
			return lhs >= rhs;
			break;
		}
		default: {
			throwValidatorSchemaJSONException(".comparisonType': Default case");
			break;
		}
	}
}

const json checkLogicalOperator(const unordered_map<string, json>& condition, const json& config, const unordered_map<string, Element*>& properties, const bool patternReplacement, const string& replacementValue)
{
	bool result;

	try
	{
		switch(EnumClass<LogicalType>::stringToEnumValue(condition.at("logicalType")))
		{
			case LogicalType::negation: {
				try
				{
					result = !checkOperator(condition.at("operands")[0], config, properties, patternReplacement, replacementValue);
				}
				catch(ValidatorConfigJSONException& e)
				{
					e.addPreMessage(".item_0");
					throw;
				}
				break;
			}
			case LogicalType::conjunction: {
				result = true;

				for(size_t i = 0; i < condition.at("operands").size(); i++) {
					try
					{
						result = result && checkOperator(condition.at("operands").at(i), config, properties, patternReplacement, replacementValue);
					}	
					catch(ValidatorConfigJSONException& e)
					{
						e.addPreMessage(".item_" + to_string(i));
						throw;
					}
				}
				break;
			}
			case LogicalType::disjunction: {
				result = false;

				for(size_t i = 0; i < condition.at("operands").size(); i++) {
					try
					{
						result = result || checkOperator(condition.at("operands").at(i), config, properties, patternReplacement, replacementValue);
					}	
					catch(ValidatorConfigJSONException& e)
					{
						e.addPreMessage(".item_" + to_string(i));
						throw;
					}
				}
				break;
			}
			default: {
				throwValidatorSchemaJSONException(".logicalType': Default case");
				break;
			}
		}
	}
	catch(ValidatorConfigJSONException& e)
	{
		e.addPreMessage(".operands");
		throw;
	}

	return result;
}

const json checkPropertyOperator(const unordered_map<string, json>& condition, const json& config, const unordered_map<string, Element*>& properties, const bool patternReplacement, const string& replacementValue)
{
	switch(EnumClass<PropertyElement>::stringToEnumValue(condition.at("propertyElement")))
	{
		case PropertyElement::value: {
			return getProperty(config, condition.at("relativePath"), patternReplacement, replacementValue);
			break;
		}
		case PropertyElement::item: {
			if(condition.at("selectedItem").is_number_unsigned())
			{
				return getProperty(config, condition.at("relativePath"), patternReplacement, replacementValue).at(condition.at("selectedItem"));
			}
			else
			{
				return getProperty(config, condition.at("relativePath"), patternReplacement, replacementValue).at(checkOperator(condition.at("selectedItem"), config, properties, patternReplacement, replacementValue));
			}
			break;
		}
		case PropertyElement::size: {
			return getProperty(config, condition.at("relativePath"), patternReplacement, replacementValue).size();
			break;
		}
		case PropertyElement::nullProperty: {
			const Element* property = properties.at(condition.at("relativePath")[0]);
			string propertyName;

			for(unsigned int i = 1; i < condition.at("relativePath").size(); i++)
			{
				tie(property, propertyName) = property->getProperty(condition.at("relativePath")[i]);
			}

			return property->isNullProperty();
			break;
		}
		default: {
			throwValidatorSchemaJSONException(".propertyElement': Default case");
			break;
		}
	}
}

const json checkConstantOperator(const unordered_map<string, json>& condition)
{
	return condition.at("value");
}

const json checkArithmeticalOperator(const unordered_map<string, json>& condition, const json& config, const unordered_map<string, Element*>& properties, const bool patternReplacement, const string& replacementValue)
{
	json lhs;

	try
	{
		lhs = checkOperator(condition.at("lhs"), config, properties, patternReplacement, replacementValue);
	}
	catch(ValidatorConfigJSONException& e)
	{
		e.addPreMessage(".lhs");
		throw;
	}

	json rhs;

	try
	{
		rhs = checkOperator(condition.at("rhs"), config, properties, patternReplacement, replacementValue);
	}
	catch(ValidatorConfigJSONException& e)
	{
		e.addPreMessage(".rhs");
		throw;
	}

	switch(EnumClass<ArithmeticalType>::stringToEnumValue(condition.at("arithmeticalType")))
	{
		case ArithmeticalType::subtract: {
			if(lhs.is_number_float() || rhs.is_number_float())
			{
				return static_cast<double>(lhs) - static_cast<double>(rhs);
			}
			else
			{
				return static_cast<int>(lhs) - static_cast<int>(rhs);
			}
			break;
		}
		case ArithmeticalType::addition: {
			if(lhs.is_number_float() || rhs.is_number_float())
			{
				return static_cast<double>(lhs) + static_cast<double>(rhs);
			}
			else
			{
				return static_cast<int>(lhs) + static_cast<int>(rhs);
			}
			break;
		}
		case ArithmeticalType::division: {
			if(lhs.is_number_float() || rhs.is_number_float())
			{
				return static_cast<double>(lhs) / static_cast<double>(rhs);
			}
			else
			{
				return static_cast<int>(lhs) / static_cast<int>(rhs);
			}
			break;
		}
		case ArithmeticalType::multiply: {
			if(lhs.is_number_float() || rhs.is_number_float())
			{
				return static_cast<double>(lhs) * static_cast<double>(rhs);
			}
			else
			{
				return static_cast<int>(lhs) * static_cast<int>(rhs);
			}
			break;
		}
		case ArithmeticalType::module: {
			return static_cast<int>(lhs) % static_cast<int>(rhs);
			break;
		}
		default: {
			throwValidatorSchemaJSONException(".arithmeticalType': Default case");
			break;
		}
	}
}

const json checkOperator(const unordered_map<string, json>& condition, const json& config, const unordered_map<string, Element*>& properties, const bool patternReplacement, const string& replacementValue)
{
	switch(EnumClass<OperatorType>::stringToEnumValue(condition.at("operatorType")))
	{
		case OperatorType::if_else: {
			return checkIfElseOperator(condition, config, properties, patternReplacement, replacementValue);
			break;
		}
		case OperatorType::comparison: {
			return checkComparisonOperator(condition, config, properties, patternReplacement, replacementValue);
			break;
		}
		case OperatorType::logical: {
			return checkLogicalOperator(condition, config, properties, patternReplacement, replacementValue);
			break;
		}
		case OperatorType::property: {
			return checkPropertyOperator(condition, config, properties, patternReplacement, replacementValue);
			break;
		}
		case OperatorType::constant: {
			return checkConstantOperator(condition);
			break;
		}
		case OperatorType::arithmetical: {
			return checkArithmeticalOperator(condition, config, properties, patternReplacement, replacementValue);
			break;
		}
		default: {
			throwValidatorSchemaJSONException(".operatorType': Default case");
			break;
		}
	}
}

const json checkOperator(const unordered_map<string, json>& condition, const json& config, const unordered_map<string, Element*>& properties)
{
	return checkOperator(condition, config, properties, false, "");
}
