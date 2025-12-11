#include "App/Model/IBM/Landscape/LivingBeings/Animals/Prey.h"



using namespace std;




Prey::Prey()
	: foodDryMass(DryMass(0.0))
{

}

Prey::Prey(const Species::ID& id, const Instar& instar, const DryMass& dryMass, const Day& handlingTime, const PreciseDouble& timeStepsPerDay)
	: id(id), instar(instar), initialDryMass(dryMass), handlingTime(handlingTime), foodDryMass(getInitialDryMass()),
	  foodDryMassPerTimeStep((timeStepsPerDay / handlingTime.getValue()) * getInitialDryMass().getValue())
{
	
}

Prey::~Prey()
{

}


Prey::Prey(const Prey &other)
{
    id = other.id;
    instar = other.instar;
    initialDryMass = other.initialDryMass;
	handlingTime = other.handlingTime;
	foodDryMass = other.foodDryMass;
}

Prey& Prey::operator=(const Prey& other) {
    if (this != &other) {
        id = other.id;
		instar = other.instar;
		initialDryMass = other.initialDryMass;
		handlingTime = other.handlingTime;
		foodDryMass = other.foodDryMass;
    }
    return *this;
}


const Species::ID& Prey::getId() const
{
	return id;
}

const Instar& Prey::getInstar() const
{
	return instar;
}

const DryMass& Prey::getInitialDryMass() const
{
	return initialDryMass;
}

const Day& Prey::getHandlingTime() const
{
	return handlingTime;
}

const DryMass& Prey::getFoodDryMass() const
{
	return foodDryMass;
}

const DryMass& Prey::getFoodDryMassPerTimeStep() const
{
	return foodDryMassPerTimeStep;
}


void Prey::decreaseFoodMass(const DryMass &foodToEliminate)
{
	foodDryMass = foodDryMass - foodToEliminate;
}


bool Prey::isThereLeftoverFood() const
{
	return getFoodDryMass() > 0.0;
}


template <class Archive>
void Prey::serialize(Archive &ar, const unsigned int) {
	ar & id;
	ar & instar;
	ar & initialDryMass;
	ar & handlingTime;
	ar & foodDryMass;
	ar & foodDryMassPerTimeStep;
}

// Specialisation
template void Prey::serialize<boost::archive::text_iarchive>(boost::archive::text_iarchive&, const unsigned int);
template void Prey::serialize<boost::archive::text_oarchive>(boost::archive::text_oarchive&, const unsigned int);

template void Prey::serialize<boost::archive::binary_iarchive>(boost::archive::binary_iarchive&, const unsigned int);
template void Prey::serialize<boost::archive::binary_oarchive>(boost::archive::binary_oarchive&, const unsigned int);
