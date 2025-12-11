
#include "OutputAnalyser/MainView.h"


using namespace std;
namespace fs = boost::filesystem;




MainView::MainView() 
    : analyserTitles(EnumClass<Analyser>::size()),
      fm(nana::API::make_center(800, 200)),
      outputField(fm, "Output Path:", true),
      analyserSelector(fm), analyserSelectorLabel(analyserSelector), analyserSelectorDropdownList(analyserSelector), 
      startAnalyserButton(fm)
{
    analyserTitles[Analyser::MovementAnalyser] = "Movement Analyser";


    /*********************/
    /* Analyser Selector */
    /*********************/

    analyserSelector.div("horizontal <weight=20% label> <dropdownList>");

    analyserSelectorLabel.caption("Select Analyser:");

    // Añadir el desplegable (combox)
    analyserSelectorDropdownList.editable(false);

    for(const Analyser type : EnumClass<Analyser>::getEnumValues())
    {
        analyserSelectorDropdownList.push_back(analyserTitles[type]);
    }
    analyserSelectorDropdownList.option(0); // Opción por defecto

    analyserSelector["label"] << analyserSelectorLabel;
    analyserSelector["dropdownList"] << analyserSelectorDropdownList;


    /****************/
    /* Start Button */
    /****************/
    
    startAnalyserButton.caption("Start Analyser");
    startAnalyserButton.events().click([this] {
        startAnalyser();
    });

    
    /***************/
    /* Main window */
    /***************/
    
    fm.caption("Output Analyser");

    // Configurar el layout inicial de la ventana
    fm.div("vert <outputField> <analyserSelector> <horizontal <> <weight=20% startButton> <>>");

    outputField.placeIn(fm.get_place(), "outputField");

    fm["analyserSelector"] << analyserSelector;

    fm["startButton"] << startAnalyserButton;

    // Actualizar la ventana para reflejar los cambios
    fm.collocate();

    // Mostrar la ventana
    fm.show();
}

MainView::~MainView() 
{

}

void MainView::run()
{
    nana::exec();
}

// Método para manejar el evento del botón submit
void MainView::startAnalyser()
{
    switch(static_cast<Analyser>(analyserSelectorDropdownList.option()))
	{
		case Analyser::MovementAnalyser: {
            MovementAnalyser movementAnalyser(fm, outputField.getPath());
            movementAnalyser.run();
			break;
		}
		default: {
			break;
		}
	}
}
