
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/TraitFactory.h"


using namespace std;
using json = nlohmann::json;


unique_ptr<Trait> TraitFactory::createInstance(const string& traitName, const json& traitInfo, const vector<string>& variableTraitsOrder) {
    switch(Trait::ValueUpdateMethod::stringToEnumValue(traitInfo["valueUpdateMethod"]["type"])) {
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

