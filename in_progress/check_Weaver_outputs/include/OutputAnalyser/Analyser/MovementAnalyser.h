#ifndef MOVEMENT_ANALYSER_H_
#define MOVEMENT_ANALYSER_H_


#include <boost/filesystem.hpp>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/screen.hpp>
#include <nana/paint/graphics.hpp>
#include <rapidcsv.h>
#include <vector>
#include <set>
#include <cmath>
#include <unordered_map>

#include "Misc/Utilities.h"



struct Point2D {
    double x;
    double y;
};

struct Step {
    unsigned int timeStep;
    Point2D start;
    Point2D end;
};


class MovementAnalyser {
protected:
    nana::form fm;

    nana::nested_form animalSelector;
    nana::label animalSelectorLabel;
    nana::combox animalSelectorDropdownList;

    nana::nested_form stepSelector;
    nana::label stepSelectorLabel;
    nana::combox stepSelectorDropdownList;

    nana::button previousStepButton;
    nana::button nextStepButton;

    nana::label timeStepInfo;

    nana::label startPointInfo;

    nana::label endPointInfo;

    nana::nested_form movementGraphPanel;
    nana::drawing movementGraphDrawing;
    std::vector<std::vector<nana::color>> gridFillColors;
    std::vector<std::string> resourceSpeciesNames;
    std::vector<nana::color> resourceSpeciesColorPalette;
    std::vector<std::string> resourceSpeciesTexts;
    nana::color arrowColor;

    unsigned int gridSize;
    double cellSize;

    double scale;
    Point2D graphOrigin;


    std::set<unsigned int> animalIds;
    std::unordered_map<unsigned int, std::vector<Step>> animalMovements;


    void readOutputInfo(const boost::filesystem::path& outputPath);

    nana::color HSL_to_RGB(const double& h, const double& s, const double& l) const;
    std::vector<nana::color> generateColorPalette(unsigned int n) const;

    std::string convertNumber2Text(const unsigned int num, const std::vector<std::string>& types) const;
    std::vector<std::string> generateCombinationTexts(const std::vector<std::string>& types) const;

    void calculateMovementGraphScale();

    void refreshSelectedStep();
    void refreshMovementGraph();

    void previousStep();
    void nextStep();

    void drawGrid(nana::paint::graphics& graph);
    void drawStep(nana::paint::graphics& graph);

public:
    MovementAnalyser(nana::form& mainWindow, const boost::filesystem::path& outputPath);
    virtual ~MovementAnalyser();

    void run();
};

#endif // MOVEMENT_ANALYSER_H_