#ifndef TRAIT_FACTORY_H_
#define TRAIT_FACTORY_H_


#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include <algorithm>

#include "IBM/World/LivingBeings/Animals/Genetics/Traits/Trait.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/FixedTrait.h"
#include "IBM/World/LivingBeings/Animals/Genetics/Traits/VariableTrait.h"
#include "Exceptions/LineInfoException.h"


class TraitFactory
{
public:
    static std::unique_ptr<Trait> createInstance(const std::string& traitName, const nlohmann::json& traitInfo, const std::vector<std::string>& variableTraitsOrder);
};


#endif // TRAIT_FACTORY_H_
