
#include "App/View/GUI/GUI.h"

#include "App/Manager/TbbManager.h"
#include "App/Manager/LogManager.h"
#include "App/IO/StorageBridge.h"
#include "App/Model/Model.h"

#include <thread>
#include <fmt/compile.h>
#include <fmt/format.h>


using namespace std;
namespace fs = std::filesystem;




GUI::GUI(Model* newModel)
    : View(newModel), layout(fm), runModeSelector(fm, "Select run mode:", getRunModesTitles().getData()),
      inputConfigPathChooser(fm, "Input config:", true), outputFolderPathChooser(fm, "Output folder:", true),
      verboseCheckbox(fm, "Verbose Output:", true), enableDiskOutputMockCheckbox(fm, "Enable Disk Output Mock:", true), 
      threadsSpinbox(fm, "Threads:", std::thread::hardware_concurrency(), 1), startSimulationButton(fm), log(fm)
{
    fm.caption(WEAVER_PROGRAM_NAME);
    fm.zoom(true);

    // Define layout structure
    layout.div(R"(
        vert
            <weight=25% 
                <horizontal 
                    <weight=60% vert params>
                    <start>
                >
            >
            <log>
    )");

    layout.field("params") << runModeSelector << inputConfigPathChooser << outputFolderPathChooser << verboseCheckbox << enableDiskOutputMockCheckbox << threadsSpinbox;

    layout.field("start") << startSimulationButton;

    layout.field("log") << log;


    outputFolderPathChooser.setPath(DEFAULT_OUTPUT_FOLDER.string());

    startSimulationButton.caption("Start Simulation");
    startSimulationButton.events().click([this] {
        startSimulation();
    });


    // Configure log update timer
    logUpdateTimer.elapse([this]{
        std::lock_guard<std::mutex> lock(logMutex);
        while(!pendingLogs.empty())
        {
            log.append(pendingLogs.pop());
        }

        if (!simulationRunning && !startSimulationButton.enabled())
        {
            startSimulationButton.enabled(true);
        }

        nana::API::refresh_window(log);
    });
    logUpdateTimer.interval(chrono::milliseconds(100));
    logUpdateTimer.start();

    
    layout.collocate();
    fm.show();
}

GUI::~GUI() 
{

}

void GUI::run(const string& runMode)
{
    unsigned int selectedRunMode = 0;

    for(unsigned int i = 0; i < getRunModesTitles().size(); i++)
    {
        if(getRunModesTitles()[i] == runMode)
        {
            selectedRunMode = i;
        }
    }

    runModeSelector.option(selectedRunMode);
    
    nana::exec();
}

void GUI::startSimulation()
{
    if (simulationRunning)
    {
        return;
    }

    log.clean();

    log.append("Starting simulation...\n");

    if (!verboseCheckbox.checked())
    {
        log.append("Simulating...\n");
    }

    const bool verboseEnabled = verboseCheckbox.checked();
    const bool diskMockEnabled = enableDiskOutputMockCheckbox.checked();
    const unsigned int threadsRequested = threadsSpinbox.value();
    const RunMode selectedRunMode = static_cast<RunMode>(runModeSelector.option());
    const fs::path inputConfigPath = inputConfigPathChooser.getPath();
    const fs::path outputFolderPath = outputFolderPathChooser.getPath();

    StorageBridge::setDiskOutputMockEnabled(diskMockEnabled);

    LogManager::setHandler([this, verboseEnabled](const LogManager::LogMessage& logItem) {
        if (!verboseEnabled && !logItem.ignore_silent)
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

        lock_guard<mutex> lock(logMutex);
        pendingLogs.push(fmt::format(FMT_COMPILE("{}{}"), prefix, logItem.message));
    });

    TbbManager::configure_pool(threadsRequested, true);
    TbbManager::initialize_and_name_pool();

    simulationRunning = true;
    startSimulationButton.enabled(false);

    std::thread([this, selectedRunMode, inputConfigPath, outputFolderPath]() {
        try {
            TbbManager::simulation_arena->execute([&]() {
                model->run(selectedRunMode, inputConfigPath, outputFolderPath);
                });

            std::string prefix = (LogManager::GREEN + "[SIM][INFO] " + LogManager::RESET);
            std::lock_guard<std::mutex> lock(logMutex);
            pendingLogs.push(fmt::format(FMT_COMPILE("{}Simulation complete.\n"), prefix));
        }
        catch (const std::exception& e) {
            std::string prefix = (LogManager::RED + "[SIM][ERROR] " + LogManager::RESET);
            std::lock_guard<std::mutex> lock(logMutex);
            pendingLogs.push(fmt::format(FMT_COMPILE("{}{}\n"), prefix, e.what()));
        }
        catch (...) {
            std::string prefix = (LogManager::RED + "[SIM][ERROR] " + LogManager::RESET);
            std::lock_guard<std::mutex> lock(logMutex);
            pendingLogs.push(fmt::format(FMT_COMPILE("{}Unknown non-standard exception.\n"), prefix));
        }

        simulationRunning = false;
        }).detach();
}
