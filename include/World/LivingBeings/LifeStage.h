#ifndef LIFE_STAGE_H_
#define LIFE_STAGE_H_


#include <ostream>
#include <vector>
#include <magic_enum.hpp>


class LifeStage {
public:
    enum LifeStageValue : unsigned int
    {
        UNBORN, 
        ACTIVE, 
        STARVED, 
        PREDATED, 
        REPRODUCING, 
        PUPA, 
        SATIATED, 
        HANDLING, 
        DIAPAUSE, 
        BACKGROUND, 
        SENESCED, 
        SHOCKED
    };

    inline static constexpr size_t size() { return magic_enum::enum_count<LifeStageValue>(); }
    inline static std::string_view to_string(const LifeStageValue& lifeStage) { return magic_enum::enum_name(lifeStage); }
    inline static const auto& getEnumValues() { return enumValues; }

    LifeStage(const LifeStageValue value);
    LifeStage(const LifeStage &other);

    inline const LifeStageValue getValue() const { return value; }
    inline operator int() const { return value; }
    inline bool operator<(const LifeStage& other) const { return this->value < other.value; }
    inline bool operator==(const LifeStage& other) const { return this->value == other.value; }
    friend inline bool operator==(const LifeStage &lhs, const LifeStageValue rhs) { return lhs.value == rhs; }
    friend inline bool operator==(const LifeStageValue lhs, const LifeStage &rhs) { return lhs == rhs.value; }

private:
    static const std::vector<LifeStageValue> enumValues;

    static const std::vector<LifeStageValue> createEnumValues();


    LifeStageValue value;
};

std::ostream& operator<<(std::ostream& os, const LifeStage lifeStage);


#endif /* LIFE_STAGE_H_ */