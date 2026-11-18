
#include "App/View/View.h"


using namespace std;
namespace fs = std::filesystem;



const fs::path View::DEFAULT_OUTPUT_FOLDER = getDefaultOutputFolderPath();



View::View(Model* newModel)
    : model(newModel), runModesTitles(EnumClass<RunMode>::size())
{
    runModesTitles[RunMode::FromConfig] = "Config";
    runModesTitles[RunMode::FromTextCheckpoint] = "Text Checkpoint";
    runModesTitles[RunMode::FromBinaryCheckpoint] = "Binary Checkpoint";
}

View::~View()
{

}

const CustomIndexedVector<RunMode, std::string>& View::getRunModesTitles() const
{
    return runModesTitles;
}
