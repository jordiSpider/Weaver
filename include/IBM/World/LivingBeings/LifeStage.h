#ifndef LIFE_STAGE_H_
#define LIFE_STAGE_H_


#include <ostream>
#include <vector>
#include <magic_enum.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <fstream>
#include <ostream>


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

    static constexpr size_t size() { return magic_enum::enum_count<LifeStageValue>(); }
    static std::string_view to_string(const LifeStageValue& lifeStage) { return magic_enum::enum_name(lifeStage); }
    static const auto& getEnumValues() { return enumValues; }

    LifeStage(const LifeStageValue value);
    LifeStage(const LifeStage &other);

    const LifeStageValue getValue() const { return value; }
    operator int() const { return value; }
    bool operator<(const LifeStage& other) const { return this->value < other.value; }
    bool operator==(const LifeStage& other) const { return this->value == other.value; }
    friend bool operator==(const LifeStage &lhs, const LifeStageValue rhs) { return lhs.value == rhs; }
    friend bool operator==(const LifeStageValue lhs, const LifeStage &rhs) { return lhs == rhs.value; }

    /**
     * @brief Serialize the LifeStage object.
     * @tparam Archive The type of archive (binary_oarchive for saving, binary_iarchive for loading).
     * @param ar The archive to use.
     * @param version The version of the serialization format.
     */
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);

private:
    static const std::vector<LifeStageValue> enumValues;

    static const std::vector<LifeStageValue> createEnumValues();


    LifeStageValue value;

    friend class boost::serialization::access;
};

std::ostream& operator<<(std::ostream& os, const LifeStage lifeStage);


#endif /* LIFE_STAGE_H_ */
