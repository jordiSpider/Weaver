#include "App/Model/IBM/Landscape/Map/Patches/Source/ObstacleSource.h"


using namespace std;
using json = nlohmann::json;


ObstacleSource::ObstacleSource()
	: PathSource()
{

}

ObstacleSource::~ObstacleSource()
{
    
}

string ObstacleSource::showInfo() const
{
	return string(" - Obstacle parameters:");
}

PathSource::Type ObstacleSource::getType() const
{
	return Type::Obstacle;
}
