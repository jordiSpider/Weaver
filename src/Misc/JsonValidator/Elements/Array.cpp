#include "Misc/JsonValidator/Elements/Array.h"

#include "Misc/JsonValidator/Elements/ElementFactory.h"


using namespace std;
using json = nlohmann::json;


Array::Array(const json& info)
    : Element(info)
{
    try
	{
		minItems = info.value("minItems", 0u);

		if(info.contains("minItems") && !info.at("minItems").is_number_integer()) {
			throwValidatorSchemaJSONException("': Not an element of type 'integer'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".minItems");
		throw;
	}


	try
	{
		maxItems = info.value("maxItems", numeric_limits<size_t>::max());

		if(info.contains("maxItems") && !info.at("maxItems").is_number_integer()) {
			throwValidatorSchemaJSONException("': Not an element of type 'integer'");
		}
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".maxItems");
		throw;
	}


	if(!info.contains("items")) {
		throwValidatorSchemaJSONException("': Property 'items' not defined");
	}

	try
	{
		if(!info.at("items").is_object()) {
			throwValidatorSchemaJSONException("': Not an element of type 'object'");
		}
		
        items = ElementFactory::createInstance(info["items"]);
	}
	catch(ValidatorSchemaJSONException& e)
	{
		e.addPreMessage(".items");
		throw;
	}


	if(!info.contains("itemsOrder")) {
		itemsOrder = make_pair<>(false, ItemsOrder::increasing);
	}
	else {
		try
		{
			if(!info.at("itemsOrder").is_string()) {
				throwValidatorSchemaJSONException("': Not an element of type 'string'");
			}

			itemsOrder = make_pair<>(true, EnumClass<ItemsOrder>::stringToEnumValue(info["itemsOrder"]));
		}
		catch(ValidatorSchemaJSONException& e)
		{
			e.addPreMessage(".itemsOrder");
			throw;
		}
	}
}

Array::Array(const Array& other, const bool nullProperty)
	: Element(nullProperty, other.getDescription()), minItems(other.getMinItems()), maxItems(other.getMaxItems()), 
	  items(other.getItems()), itemsOrder(other.getItemsOrder())
{
	
}

Array::~Array()
{
	if(!isModified())
	{
		delete items;
	}
}

void Array::validateElement(const json& config) const
{
	if(!config.is_array()) {
        throwValidatorConfigJSONException("': Not an element of type 'array'");
    }


	if(static_cast<const vector<json>&>(config).size() < minItems) {
		throwValidatorConfigJSONException("': Number of items less than minimum");
	}
    

    if(static_cast<const vector<json>&>(config).size() > maxItems) {
		throwValidatorConfigJSONException("': Number of items greater than maximum");
	}


    for(size_t i = 0; i < config.size(); i++) {
		try
		{
			items->validate(static_cast<const vector<json>&>(config).at(i));
		}
		catch(ValidatorConfigJSONException& e)
		{
			e.addPreMessage(".item_" + to_string(i));
			throw;
		}


		if(itemsOrder.first && i > 0) {
			switch(itemsOrder.second)
			{
				case ItemsOrder::increasing: {
					if(static_cast<const vector<json>&>(config).at(i-1) > static_cast<const vector<json>&>(config).at(i)) {
						throwValidatorConfigJSONException("': Items not arranged in increasing order");
					}
					break;
				}
				case ItemsOrder::strictly_increasing: {
					if(static_cast<const vector<json>&>(config).at(i-1) >= static_cast<const vector<json>&>(config).at(i)) {
						throwValidatorConfigJSONException("': Items not arranged in strictly increasing order");
					}
					break;
				}
				case ItemsOrder::decreasing: {
					if(static_cast<const vector<json>&>(config).at(i-1) < static_cast<const vector<json>&>(config).at(i)) {
						throwValidatorConfigJSONException("': Items not arranged in decreasing order");
					}
					break;
				}
				case ItemsOrder::strictly_decreasing: {
					if(static_cast<const vector<json>&>(config).at(i-1) <= static_cast<const vector<json>&>(config).at(i)) {
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

Element::Type Array::getType() const
{
	return Type::array;
}

size_t Array::getMinItems() const
{
	return minItems;
}

size_t Array::getMaxItems() const
{
	return maxItems;
}

Element* const& Array::getItems() const
{
	return items;
}

const pair<bool, Array::ItemsOrder>& Array::getItemsOrder() const
{
	return itemsOrder;
}
