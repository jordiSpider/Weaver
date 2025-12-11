/*
 * main.cpp
 *
 *  Created on: Apr 3, 2012
 *      Author: jrbcast
 */
//#define _CRTDBG_MAP_ALLOC



#include <cxxopts.hpp>
#include <nana/gui.hpp>

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>

#include "App/Model/Model.h"
#include "App/View/GUI/GUI.h"


using namespace std;




int main()
{
	cout << setprecision(numeric_limits<double>::max_digits10) << fixed;
	cerr << setprecision(numeric_limits<double>::max_digits10) << fixed;


	GUI view;

	Model* model = Model::createInstance(Model::Type::IBM, view);

	view.setModel(*model);

    unsigned char returnCode;

	try {
        view.run("FromConfig");
        returnCode = 0;
    } catch (const LineInfoException& e) {
        cerr << e.what() << endl;
        returnCode = 1;
    } catch (const std::exception& e) {
        cerr << "Caught std::exception: " << e.what() << endl;
        returnCode = 2;
    } catch (...) {
        cerr << "Caught unknown exception." << endl;
        returnCode = 3;
    }

    delete model;

    return returnCode;
}

