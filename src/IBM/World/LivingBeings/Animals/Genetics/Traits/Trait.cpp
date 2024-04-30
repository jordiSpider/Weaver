#include "IBM/World/LivingBeings/Animals/Genetics/Traits/Trait.h"

#include "IBM/World/LivingBeings/Animals/Genetics/Traits/VariableTrait.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/FixedTrait.h"

using namespace std;
using json = nlohmann::json;




unique_ptr<Trait> Trait::createInstance(const string& traitName, const json& traitInfo, const vector<string>& variableTraitsOrder) {
    switch(EnumClass<Trait::ValueUpdateMethod>::stringToEnumValue(traitInfo["valueUpdateMethod"]["type"])) {
        case Trait::ValueUpdateMethod::Fixed: {
            std::vector<std::string>::const_iterator traitPos = find(variableTraitsOrder.cbegin(), variableTraitsOrder.cend(), traitName);

            if(traitPos != variableTraitsOrder.cend())
            {
                throwLineInfoException("Error: Trait '" + traitName + "' is fixed and must not have a defined order.");
            }

            return make_unique<FixedTrait>(traitName, traitInfo);
            break;
        }
        case Trait::ValueUpdateMethod::Variable: {
            std::vector<std::string>::const_iterator traitPos = find(variableTraitsOrder.cbegin(), variableTraitsOrder.cend(), traitName);

            if(traitPos == variableTraitsOrder.cend())
            {
                throwLineInfoException("Error: Trait '" + traitName + "' has no defined order.");
            }

            return make_unique<VariableTrait>(traitName, traitInfo, distance(variableTraitsOrder.cbegin(), traitPos));
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}

unique_ptr<Trait> Trait::createInstance(const Trait::ValueUpdateMethod &traitValueUpdateMethod)
{
	switch(traitValueUpdateMethod) {
        case Trait::ValueUpdateMethod::Fixed: {
            return make_unique<FixedTrait>();
            break;
        }
        case Trait::ValueUpdateMethod::Variable: {
            return make_unique<VariableTrait>();
            break;
        }
        default: {
            throwLineInfoException("Default case");
            break;
        }
    }
}


Trait::Trait()
{

}

Trait::Trait(const string& name, const json& traitInfo)
	: type(EnumClass<Type>::stringToEnumValue(name)), lowerThreshold(traitInfo["temperatureThreshold"]["lower"]), 
	  upperThreshold(traitInfo["temperatureThreshold"]["upper"])
{

}

const Temperature& Trait::getLowerThreshold() const
{
	return lowerThreshold;
}

const Temperature& Trait::getUpperThreshold() const
{
	return upperThreshold;
}

template <class Archive>
void Trait::serialize(Archive &ar, const unsigned int version) {
	ar & type;
	ar & lowerThreshold;
    ar & upperThreshold;
} 

// Specialisation
template void Trait::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Trait::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Trait::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Trait::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);


namespace boost {
    namespace serialization {
        template<class Archive>
        void serialize(Archive &ar, Trait* &traitPtr, const unsigned int version) {
            Trait::ValueUpdateMethod traitValueUpdateMethod;
            
            // For loading
            if(Archive::is_loading::value) 
            {
                ar & traitValueUpdateMethod;
                traitPtr = Trait::createInstance(traitValueUpdateMethod).release();
            }
            else
            {
                traitValueUpdateMethod = traitPtr->getValueUpdateMethodType();
                ar & traitValueUpdateMethod;
            }

            switch(traitValueUpdateMethod) {
				case Trait::ValueUpdateMethod::Fixed: {
					static_cast<FixedTrait*>(traitPtr)->serialize(ar, version);
					break;
				}
				case Trait::ValueUpdateMethod::Variable: {
					static_cast<VariableTrait*>(traitPtr)->serialize(ar, version);
					break;
				}
                default: {
                    throwLineInfoException("Default case");
                    break;
                }
            }
        }

        // Specialisation
        template void serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, Trait*&, const unsigned int);
        template void serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, Trait*&, const unsigned int);

        template void serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, Trait*&, const unsigned int);
        template void serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, Trait*&, const unsigned int);
    }
}
