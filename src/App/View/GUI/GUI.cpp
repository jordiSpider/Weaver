
#include "App/View/GUI/GUI.h"

#include "App/Model/Model.h"


using namespace std;




GUI::GUI() 
    : View(), layout(fm), runModeSelector(fm, "Select run mode:", getRunModesTitles().getData()),
      inputConfigPathChooser(fm, "Input config:", true), outputFolderPathChooser(fm, "Output folder:", true),
       showOutputCheckbox(fm, "Show Output:", true), startSimulationButton(fm), log(fm)
{
    fm.caption(PROGRAM_NAME);
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

    layout.field("params") << runModeSelector << inputConfigPathChooser << outputFolderPathChooser << showOutputCheckbox;

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
            log.append(pendingLogs.front());
            pendingLogs.pop();
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
        if(getRunModesTitles().at(static_cast<RunMode>(i)) == runMode)
        {
            selectedRunMode = i;
        }
    }

    runModeSelector.option(selectedRunMode);
    
    nana::exec();
}

void GUI::startSimulation()
{
    log.clean();

    std::thread([this] {
        try {
            log.append("Starting simulation...\n");

            if(!showOutputCheckbox.checked())
            {
                log.append("Simulating...\n");
            }

            model->run(
                static_cast<RunMode>(runModeSelector.option()),
                inputConfigPathChooser.getPath(),
                outputFolderPathChooser.getPath()
            );
            
            log.append("Simulation complete.\n");
        }
        catch(const std::exception& e) {
            std::lock_guard<std::mutex> lock(logMutex);
            pendingLogs.push(std::string("Error: ") + e.what() + "\n");
        }
    }).detach();
}

void GUI::updateLog(const string& message, bool ignoreSilentMode)
{
    if(showOutputCheckbox.checked() || ignoreSilentMode)
    {
        lock_guard<mutex> lock(logMutex);
        pendingLogs.push(message);
    }
}

void GUI::updateLogError(const string& message, bool ignoreSilentMode)
{
    if(showOutputCheckbox.checked() || ignoreSilentMode)
    {
        lock_guard<mutex> lock(logMutex);
        pendingLogs.push(message);
    }
}
