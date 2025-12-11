#ifndef MAIN_VIEW_H_
#define MAIN_VIEW_H_


#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/screen.hpp>

#include "Misc/CustomIndexedVector.h"
#include "Misc/EnumClass.h"
#include "CommonWidgets/InputFieldPath.h"
#include "OutputAnalyser/Analyser/MovementAnalyser.h"



class MainView {
protected:
    enum class Analyser : unsigned int {
        MovementAnalyser
    };

    CustomIndexedVector<Analyser, std::string> analyserTitles;


    nana::form fm;

    InputFieldPath outputField;

    nana::nested_form analyserSelector;
    nana::label analyserSelectorLabel;
    nana::combox analyserSelectorDropdownList;

    nana::button startAnalyserButton;
        

    void startAnalyser();

public:
    MainView();
    virtual ~MainView();

    void run();
};

#endif // MAIN_VIEW_H_