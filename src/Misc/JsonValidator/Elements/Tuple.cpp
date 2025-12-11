#include "Misc/JsonValidator/Elements/Tuple.h"


using namespace std;
using json = nlohmann::json;


Tuple::Tuple(const json& info)
    : Element(info)
{
    try
	{
		if(!info.at("items").is_array()) {
			throwValidatorSchemaJSONException("': Not an element of type 'array'");
		}

		for(const json& item : info["items"])
        {
            items.push_back(ElementFactory::createInstance(item));
        }
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".items");
		throw;
	}
	catch(const json::out_of_range&) 
	{
		throwValidatorSchemaJSONException("': Property 'items' not defined");
	}
}

Tuple::Tuple(const Tuple& other, const bool nullProperty)
	: Element(nullProperty, other.getDescription()), items(other.getItems())
{
	
}

Tuple::~Tuple()
{
	if(!isModified())
	{
		for(Element*& item : items)
		{
			delete item;
		}
	}
}

void Tuple::validateElement(const json& config) const
{
	if(!config.is_array()) {
        throwValidatorConfigJSONException("': Not an element of type 'array'");
    }


	if(config.size() != items.size()) {
		throwValidatorConfigJSONException("': Number of elements is different from number of tuple elements");
	}


    for(size_t i = 0; i < config.size(); i++) {
		try
		{
			items[i]->validate(static_cast<const vector<json>&>(config).at(i));
		}
		catch(ValidatorJSONException& e)
		{
			e.addPreMessage(".item_" + to_string(i));
			throw;
		}
	}
}

Element::Type Tuple::getType() const
{
	return Type::tuple;
}

const vector<Element*>& Tuple::getItems() const
{
	return items;
}
