#include "App/Model/IBM/Landscape/Map/Patches/Source/ObstacleSource.h"

#include <sstream>
#include <stdexcept>

using namespace std;
using json = nlohmann::json;


ObstacleSource::ObstacleSource()
	: PathSource()
{

}

ObstacleSource::ObstacleSource(const json& porosityConfig)
	: PathSource()
{
	auto readNumber = [&porosityConfig](const char* key, const double defaultValue) {
		auto it = porosityConfig.find(key);
		return it == porosityConfig.end() || it->is_null() ? defaultValue : it->get<double>();
	};

	porosityParameters.fraction = readNumber("fraction", 0.0);
	porosityParameters.minimumPoreWidth = readNumber("minimumPoreWidth", 0.0);
	porosityParameters.featureSize = readNumber("featureSize", 0.0);

	auto octaves = porosityConfig.find("octaves");
	if(octaves != porosityConfig.end() && !octaves->is_null())
	{
		porosityParameters.octaves = octaves->get<size_t>();
	}

	if(porosityParameters.fraction > 0.0 &&
		(porosityParameters.minimumPoreWidth <= 0.0 || porosityParameters.featureSize <= 0.0 || porosityParameters.octaves == 0u))
	{
		throw invalid_argument("Porosity greater than zero requires minimumPoreWidth, featureSize and octaves.");
	}
}

ObstacleSource::~ObstacleSource()
{
    
}

string ObstacleSource::showInfo() const
{
	ostringstream info;
	info << " - Obstacle parameters:\n"
		 << "   - Porosity fraction = " << porosityParameters.fraction << "\n"
		 << "   - Minimum pore width = " << porosityParameters.minimumPoreWidth << "\n"
		 << "   - Feature size = " << porosityParameters.featureSize << "\n"
		 << "   - Noise octaves = " << porosityParameters.octaves;
	return info.str();
}

const ObstacleSource::PorosityParameters& ObstacleSource::getPorosityParameters() const
{
	return porosityParameters;
}

bool ObstacleSource::hasPorosity() const
{
	return porosityParameters.fraction > 0.0;
}

PathSource::Type ObstacleSource::getType() const
{
	return Type::Obstacle;
}
