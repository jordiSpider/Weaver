
#include "OutputAnalyser/Analyser/MovementAnalyser.h"


using namespace std;
using json = nlohmann::json;
namespace fs = boost::filesystem;




MovementAnalyser::MovementAnalyser(nana::form& mainWindow, const fs::path& outputPath) 
    : fm(mainWindow, nana::API::make_center(nana::screen().desktop_size().width, nana::screen().desktop_size().height)),
      animalSelector(fm), animalSelectorLabel(animalSelector), animalSelectorDropdownList(animalSelector),
      stepSelector(fm), stepSelectorLabel(stepSelector), stepSelectorDropdownList(stepSelector),
      previousStepButton(fm), nextStepButton(fm),
      timeStepInfo(fm), startPointInfo(fm), endPointInfo(fm),
      movementGraphPanel(fm), movementGraphDrawing(movementGraphPanel)
{
    readOutputInfo(outputPath);



    /*******************/
    /* Animal Selector */
    /*******************/

    animalSelector.div("horizontal <weight=20% label> <dropdownList>");

    animalSelectorLabel.caption("Select Animal ID:");

    animalSelectorDropdownList.editable(false);

    for(const unsigned int id : animalIds)
    {
        animalSelectorDropdownList.push_back(to_string(id));
    }

    animalSelectorDropdownList.events().selected([this] {
        refreshSelectedStep();
    });

    animalSelector["label"] << animalSelectorLabel;
    animalSelector["dropdownList"] << animalSelectorDropdownList;



    /*******************/
    /* Movement Slider */
    /*******************/

    stepSelector.div("horizontal <weight=20% label> <dropdownList>");

    stepSelectorLabel.caption("Select Step:");

    stepSelectorDropdownList.editable(false);

    stepSelectorDropdownList.events().selected([this] {
        refreshMovementGraph();
    });

    stepSelector["label"] << stepSelectorLabel;
    stepSelector["dropdownList"] << stepSelectorDropdownList;



    /******************/
    /* Control Button */
    /******************/
    
    previousStepButton.caption("<< Previous Step");
    previousStepButton.events().click([this] {
        previousStep();
    });

    nextStepButton.caption("Next Step >>");
    nextStepButton.events().click([this] {
        nextStep();
    });



    /****************/
    /* Info section */
    /****************/

    string infoSectionDiv = "<vert <weight=5% animalSelector> <weight=5% stepSelector> <weight=5% <previousButton> <nextButton>> <weight=5%> <weight=5% <> <timeStep> <>> <weight=5% <startPoint> <endPoint>> <>>";



    /*****************/
    /* Graph section */
    /*****************/

    string graphSectionDiv = "<weight=" + to_string(fm.size().height) + " movementGraph>";



    /****************/
    /* Modal window */
    /****************/

    fm.caption("Movement Analyser");

    fm.div("<" + infoSectionDiv + " " + graphSectionDiv + ">");

    fm["animalSelector"] << animalSelector;

    fm["stepSelector"] << stepSelector;

    fm["previousButton"] << previousStepButton;

    fm["nextButton"] << nextStepButton;

    fm["timeStep"] << timeStepInfo;

    fm["startPoint"] << startPointInfo;

    fm["endPoint"] << endPointInfo;

    fm["movementGraph"] << movementGraphPanel;

    animalSelectorDropdownList.option(0); // Opción por defecto
    
    // Actualizar la ventana para reflejar los cambios
    fm.collocate();
}

MovementAnalyser::~MovementAnalyser() 
{

}

void MovementAnalyser::run()
{
    nana::API::modal_window(fm); 
}

void MovementAnalyser::readOutputInfo(const fs::path& outputPath)
{
    fs::path configPath = fs::directory_iterator(outputPath / "config")->path();

    json worldParams = readConfigFile(configPath / "world_params.json");

    /////////////

    gridSize = worldParams["world"]["mapConfig"]["worldWideParams"]["numberOfCellsPerAxis"];

    /////////////

    cellSize = worldParams["world"]["mapConfig"]["worldWideParams"]["minCellSize"];

    /////////////

    rapidcsv::Document movementsFile((outputPath / "movements.txt").string(), rapidcsv::LabelParams(0, -1), rapidcsv::SeparatorParams('\t'));

    for(size_t i = 0; i < movementsFile.GetRowCount(); ++i) {
        unsigned int id = movementsFile.GetCell<unsigned int>("id", i);

        animalIds.insert(id);

        animalMovements[id].push_back({
            movementsFile.GetCell<unsigned int>("timeStep", i),
            {movementsFile.GetCell<double>("startPointX", i), movementsFile.GetCell<double>("startPointY", i)},
            {movementsFile.GetCell<double>("endPointX", i), movementsFile.GetCell<double>("endPointY", i)}
        });
    }

    //////////////

    for(const auto& entry : fs::directory_iterator(configPath / "resource")) {
        if(fs::is_regular_file(entry.status())) {
            json resourceSpecies = readConfigFile(entry.path());

            resourceSpeciesNames.push_back(resourceSpecies["resource"]["name"]);
        }
    }
    

    gridFillColors.resize(gridSize, vector<nana::color>(gridSize));

    rapidcsv::Document cellsFile((outputPath / "cells_each_day" / "cells_day_0000000000.txt").string(), rapidcsv::LabelParams(0, -1), rapidcsv::SeparatorParams('\t'));

    unsigned int numberOfCombinations = pow(2, resourceSpeciesNames.size());
    resourceSpeciesColorPalette = generateColorPalette(numberOfCombinations);

    arrowColor = HSL_to_RGB(0.0, 1.0, 0.5);

    resourceSpeciesTexts = generateCombinationTexts(resourceSpeciesNames);


    for(size_t row = 0; row < cellsFile.GetRowCount(); row++) {
        unsigned int X = cellsFile.GetCell<unsigned int>("X", row);
        unsigned int Y = cellsFile.GetCell<unsigned int>("Y", row);

        unsigned int number = 0;

        for(unsigned int i = 0; i < resourceSpeciesNames.size(); i++)
        {
            if(cellsFile.GetCell<double>(resourceSpeciesNames[i], row) > 0.0)
            {
                number += (1 << i);
            }
        }

        if(number > 0)
        {
            gridFillColors[X][Y] = resourceSpeciesColorPalette[number-1];
        }
        else
        {
            gridFillColors[X][Y] = nana::colors::white;
        }
    }
}

nana::color MovementAnalyser::HSL_to_RGB(const double& h, const double& s, const double& l) const {
    auto hue2rgb = [](double p, double q, double t) {
        if (t < 0.0) t += 1.0;
        if (t > 1.0) t -= 1.0;
        if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
        if (t < 1.0 / 2.0) return q;
        if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
        return p;
    };

    double r, g, b;
    if (s == 0.0) {
        r = g = b = l;  // achromatic
    } else {
        double q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
        double p = 2.0 * l - q;
        r = hue2rgb(p, q, h + 1.0 / 3.0);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1.0 / 3.0);
    }
    
    return nana::color(static_cast<unsigned char>(r * 255), static_cast<unsigned char>(g * 255), static_cast<unsigned char>(b * 255));
}

// Generar una paleta de 'n' colores en base a HSL
vector<nana::color> MovementAnalyser::generateColorPalette(unsigned int n) const {
    vector<nana::color> palette;

    for(unsigned int i = 0; i < n-1; ++i) {
        double hue = static_cast<double>(i) / n-1;  // Distribuir los colores en el espectro HSL (0 a 1)
        palette.push_back(HSL_to_RGB(hue, 0.2, 0.8));  // Saturación y Luminosidad fijas
    }

    return palette;
}

std::string MovementAnalyser::convertNumber2Text(const unsigned int num, const vector<string>& types) const {
    std::stringstream ss;
    
    if(num & (1 << 0))
    {
        ss << types[0];
    }

    // Recorrer cada bit del número binario
    for(unsigned int i = 1; i < types.size(); i++) {
        if(num & (1 << i)) {  // Si el bit está encendido, incluye el carácter correspondiente
            ss << " + ";
            ss << types[i];
        }
    }
    
    return ss.str();
}

// Función para generar todas las combinaciones posibles de un número binario con 'n' posiciones
vector<string> MovementAnalyser::generateCombinationTexts(const vector<string>& types) const {
    vector<string> combinationTexts;

    // Generar todas las combinaciones posibles (2^n combinaciones)
    for (int i = 1; i < (1 << types.size()); ++i) { // Empieza en 1 para evitar la combinación vacía
        combinationTexts.push_back(convertNumber2Text(i, types));
    }

    return combinationTexts;
}

void MovementAnalyser::calculateMovementGraphScale()
{
    unsigned int graphSize = round(movementGraphPanel.size().height * 0.95);

    scale = static_cast<double>(graphSize) / (gridSize*cellSize);

    graphOrigin = {
        (movementGraphPanel.size().width / 2.0) - (graphSize / 2.0), 
        (movementGraphPanel.size().height / 2.0) - (graphSize / 2.0)
    };
}

void MovementAnalyser::refreshSelectedStep()
{
    stepSelectorDropdownList.clear();

    unsigned int animalId = stoi(animalSelectorDropdownList.text(animalSelectorDropdownList.option()));

    for(unsigned int step = 1; step <= animalMovements[animalId].size(); step++)
    {
        stepSelectorDropdownList.push_back(to_string(step));
    }
    stepSelectorDropdownList.option(0); // Opción por defecto
}

void MovementAnalyser::refreshMovementGraph()
{
    movementGraphDrawing.clear();

    movementGraphDrawing.draw([this](nana::paint::graphics& graph){
        calculateMovementGraphScale();
        drawGrid(graph);
        drawStep(graph);
    });

    movementGraphDrawing.update();
    
    fm.collocate();  
}

void MovementAnalyser::previousStep()
{
    size_t step = max(static_cast<size_t>(0), stepSelectorDropdownList.option()-1);
    stepSelectorDropdownList.option(step);
}

void MovementAnalyser::nextStep()
{
    size_t step = min(animalMovements[animalSelectorDropdownList.option()].size()-1, stepSelectorDropdownList.option()+1);
    stepSelectorDropdownList.option(step);
}

void MovementAnalyser::drawGrid(nana::paint::graphics& graph) {
    unsigned int cellSizePx = cellSize * scale;
    
    for(unsigned int row = 0; row < gridSize; row++) {
        for(unsigned int col = 0; col < gridSize; col++) {
            int x = graphOrigin.x + col * cellSizePx;
            int y = graphOrigin.y + row * cellSizePx;

            graph.rectangle({x, y, cellSizePx, cellSizePx}, true, gridFillColors[row][col]);

            if(row == 0 && col == 0)
            {
                graph.rectangle({x, y, cellSizePx, cellSizePx}, false, nana::colors::red);
            }
            else
            {
                graph.rectangle({x, y, cellSizePx, cellSizePx}, false, nana::colors::black);
            }
        }
    }
}

void MovementAnalyser::drawStep(nana::paint::graphics& graph) {
    unsigned int animalId = stoi(animalSelectorDropdownList.text(animalSelectorDropdownList.option()));

    Point2D startPoint = animalMovements[animalId][stepSelectorDropdownList.option()].start;
    Point2D endPoint = animalMovements[animalId][stepSelectorDropdownList.option()].end;
    
    
    timeStepInfo.caption("TimeStep: " + to_string(animalMovements[animalId][stepSelectorDropdownList.option()].timeStep));
    startPointInfo.caption("Start Point: [" + to_string(startPoint.x) + " , " + to_string(startPoint.y) + "]");
    endPointInfo.caption("Start Point: [" + to_string(endPoint.x) + " , " + to_string(endPoint.y) + "]");


    // Escalar las coordenadas para ajustarlas a la cuadrícula
    int x1 = graphOrigin.x + startPoint.x * scale;
    int y1 = graphOrigin.y + startPoint.y * scale;

    int x2 = graphOrigin.x + endPoint.x * scale;
    int y2 = graphOrigin.y + endPoint.y * scale;

    // Dibujar la línea (paso) entre los dos puntos
    graph.line({x1, y1}, {x2, y2}, arrowColor);

    // Opcional: Dibujar la flecha al final de la línea
    double angle = std::atan2(y2 - y1, x2 - x1);  // Calcular el ángulo de la línea

    // Parámetros de la flecha
    int arrow_size = 10;  // Tamaño de la flecha
    double arrow_angle = 3.14159 / 6;  // Ángulo de apertura de la flecha (30 grados)

    // Calcular los puntos de la flecha
    int arrow_x1 = static_cast<int>(x2 - arrow_size * std::cos(angle - arrow_angle));
    int arrow_y1 = static_cast<int>(y2 - arrow_size * std::sin(angle - arrow_angle));
    int arrow_x2 = static_cast<int>(x2 - arrow_size * std::cos(angle + arrow_angle));
    int arrow_y2 = static_cast<int>(y2 - arrow_size * std::sin(angle + arrow_angle));

    // Dibujar los lados de la flecha
    graph.line({x2, y2}, {arrow_x1, arrow_y1}, arrowColor);
    graph.line({x2, y2}, {arrow_x2, arrow_y2}, arrowColor);
}
