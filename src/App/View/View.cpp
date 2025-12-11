
#include "App/View/View.h"


using namespace std;
namespace fs = std::filesystem;



const fs::path View::DEFAULT_OUTPUT_FOLDER = getDefaultOutputFolderPath();



View::View()
    : runModesTitles(EnumClass<RunMode>::size())
{
    runModesTitles[RunMode::FromConfig] = "Config";
    runModesTitles[RunMode::FromTextCheckpoint] = "Text Checkpoint";
    runModesTitles[RunMode::FromBinaryCheckpoint] = "Binary Checkpoint";
}

View::~View()
{

}

void View::setModel(Model& newModel)
{
    model = &newModel;
}

const CustomIndexedVector<RunMode, std::string>& View::getRunModesTitles() const
{
    return runModesTitles;
}

void View::updateLog(initializer_list<string> messages, bool ignoreSilentMode)
{
    for(const string& message : messages)
    {
        updateLog(message, ignoreSilentMode);
    }
}

void View::updateLogError(initializer_list<string> messages, bool ignoreSilentMode)
{
    for(const string& message : messages)
    {
        updateLogError(message, ignoreSilentMode);
    }
}
