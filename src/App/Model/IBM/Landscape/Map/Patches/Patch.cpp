#include "App/Model/IBM/Landscape/Map/Patches/Patch.h"

using namespace std;
using json = nlohmann::json;


Patch::Patch(PathSource* const source, const json &patchConfig, const string &filename)
	: priority(patchConfig["priority"].get<size_t>()), source(source), shape(PatchShape::createInstance(patchConfig["shape"])), filename(filename)
{

}

Patch::Patch(PathSource* const source, const size_t priority, PatchShape* shape)
	: priority(priority), source(source), shape(shape), filename("No filename")
{

}

Patch::~Patch()
{
	if(source != nullptr)
	{
		delete source;
	}

	delete shape;
}

Coverage Patch::checkCoverage(const RingModel* const cellBounds) const
{
	return shape->checkCoverage(cellBounds);
}

size_t Patch::getPriority() const
{
	return priority;
}

const PathSource& Patch::getSource() const
{
	return *source;
}

PathSource* Patch::moveSource()
{
	PathSource* sourceToMove = source;
	source = nullptr;

	return sourceToMove;
}

const string Patch::getDescription() const
{
	std::ostringstream description;

	description << "Initializing " << EnumClass<PatchShape::Type>::to_string(shape->getType()) << " " << EnumClass<PathSource::Type>::to_string(source->getType()) << " patch \"" << filename << "\":\n";

	description << " - Priority = " << getPriority() << "\n";

	description << shape->getDescription() << "\n";

	description << source->showInfo();

	return description.str();
}

bool Patch::operator<(const Patch& other) const {
	return getPriority() > other.getPriority();
}
