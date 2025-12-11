
#include "App/View/Console.h"

#include "App/Model/Model.h"


using namespace std;
namespace fs = std::filesystem;



Console::Console()
    : View()
{

}

Console::~Console()
{

}

bool Console::isSilentMode() const
{
    return silentMode;
}

RunMode Console::selectRunMode() const
{
    cout << "Select Mode:" << endl;
    
    for(const RunMode runMode : EnumClass<RunMode>::getEnumValues())
    {
        cout << "\t" << static_cast<int>(runMode) << ") " << getRunModesTitles().at(runMode) << endl;
    }

    cout << endl;

    cout << "Enter mode number: ";
    unsigned int selectedRunMode;
    cin >> selectedRunMode;

    cout << endl;
    cout << "---------------------------------------------" << endl;

    return static_cast<RunMode>(selectedRunMode);
}

bool Console::isValidFolderPath(const fs::path& folderPath) const
{
    return fs::exists(folderPath) && fs::is_directory(folderPath);
}

fs::path Console::requestInputConfig() const
{
    cout << "Please specify config directory." << endl;
    cout << endl;

    fs::path inputConfigPath;
    
    do
    {
        cout << "Input config: ";
        cin >> inputConfigPath;

        inputConfigPath = resolvePath(WORKING_DIRECTORY, inputConfigPath);
    } 
    while(!isValidFolderPath(inputConfigPath));

    cout << endl;
    cout << "---------------------------------------------" << endl;

    return inputConfigPath;
}

fs::path Console::requestOutputFolder() const
{
    cout << "Please specify output directory." << endl;
    cout << endl;

    fs::path outputFolderPath;

    do
    {
        // Limpiar cualquier carácter sobrante en el búfer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Output folder [default: " << DEFAULT_OUTPUT_FOLDER.string() << "] (press ENTER to default): ";

        string outputFolderPathStr;
        getline(cin, outputFolderPathStr);
        outputFolderPath = fs::path(outputFolderPathStr);

        if(outputFolderPath.empty())
        {
            outputFolderPath = DEFAULT_OUTPUT_FOLDER;
        }

        outputFolderPath = resolvePath(WORKING_DIRECTORY, outputFolderPath);
    } 
    while(!isValidFolderPath(outputFolderPath));

    cout << endl;
    cout << "---------------------------------------------" << endl;

    return outputFolderPath;
}

bool Console::requestShowOutput() const
{
    cout << "Please specify if you want to display the simulation output." << endl;
    cout << endl;
    
    string silentModeValue;
    do
    {
        cout << "Do you want to show output [Y/n]: ";
        cin >> silentModeValue;
    } 
    while(silentModeValue != "Y" && silentModeValue != "y" && silentModeValue != "N" && silentModeValue != "n");
    
    cout << endl;
    cout << "---------------------------------------------" << endl;

    return silentModeValue == "Y" || silentModeValue == "y";
}

void Console::run(const string& runMode, const string& inputConfig, const string& outputFolder, const bool silent)
{
    RunMode selectedRunMode;

    if(runMode.empty())
    {
        selectedRunMode = selectRunMode();
    }
    else
    {
        selectedRunMode = EnumClass<RunMode>::stringToEnumValue(runMode);
    }
    


    fs::path inputConfigPath;
    
    if(inputConfig.empty())
    {
        inputConfigPath = requestInputConfig();
    }
    else
    {
        inputConfigPath = resolvePath(WORKING_DIRECTORY, fs::path(inputConfig));

        if(!isValidFolderPath(inputConfigPath))
        {
            throwLineInfoException("Error: The input config directory does not exist or is not a valid directory.");
        }
    }


    fs::path outputFolderPath;

    if(outputFolder.empty())
    {
        outputFolderPath = requestOutputFolder();
    }
    else
    {
        outputFolderPath = resolvePath(WORKING_DIRECTORY, fs::path(outputFolder));

        if(!isValidFolderPath(outputFolderPath))
        {
            throwLineInfoException("Error: The output folder directory does not exist or is not a valid directory.");
        }
    }
    

    if(silent)
    {
        silentMode = silent;
    }
    else
    {
        silentMode = !requestShowOutput();
    }


    cout << "Starting simulation..." << endl;

    if(isSilentMode())
    {
        cout << "Simulating..." << endl;
    }

    model->run(selectedRunMode, inputConfigPath, outputFolderPath);

    cout << "Simulation complete." << endl;
}

void Console::updateLog(const string& message, bool ignoreSilentMode)
{
    if(!isSilentMode() || ignoreSilentMode)
    {
        cout << message;
    }
}

void Console::updateLogError(const string& message, bool ignoreSilentMode)
{
    if(!isSilentMode() || ignoreSilentMode)
    {
        cerr << message;
    }
}
