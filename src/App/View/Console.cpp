
#include "App/View/Console.h"

#include <thread>

#include "App/Manager/TbbManager.h"
#include "App/Manager/LogManager.h"
#include "App/IO/StorageBridge.h"
#include "App/Model/Model.h"

#ifdef _WIN32
#include <windows.h>
#endif


using namespace std;
namespace fs = std::filesystem;



Console::Console(Model* newModel)
    : View(newModel)
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
        cout << "\t" << static_cast<int>(runMode) << ") " << getRunModesTitles()[runMode] << endl;
    }

    cout << endl;

    cout << "Enter mode number: ";
    unsigned int selectedRunMode;

    if (!(cin >> selectedRunMode)) {
        cin.clear();
        selectedRunMode = 0;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

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
    string inputStr;
    
    do
    {
        cout << "Input config: ";
        std::getline(cin, inputStr);

        inputConfigPath = fs::path(inputStr);

        inputConfigPath = std::filesystem::weakly_canonical(inputConfigPath);
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
    string inputStr;

    do
    {
        cout << "Output folder [default: " << DEFAULT_OUTPUT_FOLDER.string() << "] (press ENTER to default): ";

        std::getline(cin, inputStr);

        if (inputStr.empty())
        {
            outputFolderPath = DEFAULT_OUTPUT_FOLDER;
        }
        else
        {
            outputFolderPath = fs::path(inputStr);
        }

        outputFolderPath = std::filesystem::weakly_canonical(outputFolderPath);
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

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } 
    while(silentModeValue != "Y" && silentModeValue != "y" && silentModeValue != "N" && silentModeValue != "n");
    
    cout << endl;
    cout << "---------------------------------------------" << endl;

    return silentModeValue == "Y" || silentModeValue == "y";
}

unsigned int Console::requestThreads() const
{
    cout << "Please specify the number of threads for parallel execution." << endl;
    cout << "Default value: " << std::thread::hardware_concurrency() << " (press ENTER to default)" << endl;
    cout << endl;

    unsigned int threads = 0u;
    string inputStr;

    do
    {
        cout << "Threads: ";
        std::getline(cin, inputStr);

        if (inputStr.empty())
        {
            threads = std::thread::hardware_concurrency();
            break;
        }

        try {
            threads = std::stoul(inputStr);
        }
        catch (...) {
            threads = 0u;
        }

        if (threads == 0u) {
            cout << "Invalid number of threads. Please enter a positive integer." << endl;
        }
        else if(threads > std::thread::hardware_concurrency()) {
            cout << "Warning: The specified number of threads exceeds the hardware concurrency (" << std::thread::hardware_concurrency() << ")." << endl;
            threads = 0u;
        }
    } while (threads == 0u);

    cout << endl;
    cout << "---------------------------------------------" << endl;

    return threads;
}

void Console::run(const string& runMode, const string& inputConfig, const string& outputFolder, bool silent, bool verbose, bool enableDiskOutputMock, unsigned int requestedThreads)
{
	// Enable ANSI escape codes for colored output in Windows console
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif


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
		inputConfigPath = std::filesystem::weakly_canonical(fs::path(inputConfig));

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
        outputFolderPath = std::filesystem::weakly_canonical(fs::path(outputFolder));

        if(!isValidFolderPath(outputFolderPath))
        {
            throwLineInfoException("Error: The output folder directory does not exist or is not a valid directory.");
        }
    }
    

    if(silent || verbose)
    {
        if(silent)
        {
            silentMode = silent;
        }
        
        if(verbose)
        {
            silentMode = !verbose;
        }
    }
    else
    {
        silentMode = !requestShowOutput();
    }


    if (requestedThreads == 0u)
    {
        requestedThreads = requestThreads();
    }


    cout << "Starting simulation..." << endl;

    if(isSilentMode())
    {
        cout << "Simulating..." << endl;
    }

    StorageBridge::setDiskOutputMockEnabled(enableDiskOutputMock);

    LogManager::setHandler([this](const LogManager::LogMessage& logItem) {
        if (isSilentMode() && !logItem.ignore_silent)
        {
            return;
        }

        std::string prefix;
        if (logItem.subsystem == LogManager::Subsystem::IO) {
            prefix = logItem.is_error ? (LogManager::RED + "[IO ][ERROR] " + LogManager::RESET)
                : (LogManager::CYAN + "[IO ][INFO] " + LogManager::RESET);
        }
        else {
            prefix = logItem.is_error ? (LogManager::RED + "[SIM][ERROR] " + LogManager::RESET)
                : (LogManager::GREEN + "[SIM][INFO] " + LogManager::RESET);
        }

        if (logItem.is_error) {
            std::cerr << prefix << logItem.message;
        }
        else {
            std::cout << prefix << logItem.message;
        }
    });

    // Configuración e inicialización del pool de TBB
    TbbManager::configure_pool(requestedThreads, false);
    TbbManager::initialize_and_name_pool();

    try {
        TbbManager::simulation_arena->execute([&]() {
            model->run(selectedRunMode, inputConfigPath, outputFolderPath);
            });
    }
    catch (...) {
        throw;
    }

    cout << "Simulation complete." << endl;
}
