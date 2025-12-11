
#include "Tools/Updater/Versions/Updater_23_12_04.h"


using namespace std;
using json = nlohmann::json;


Updater_23_12_04::Updater_23_12_04()
    : Updater("23.12.04")
{
    
}

Updater_23_12_04::~Updater_23_12_04()
{
    
}

void Updater_23_12_04::update(Config &config)
{
    Updater::update(config);

    //////////////////////////
    //  Version:  23.12.04  //
    //////////////////////////

        /////////////////////////////////////
        //  Rename saveGenetics parameter  //
        /////////////////////////////////////

    config.moveJsonItem(
        {},
        {"simulation", "saveGeneticsSummaries"},
        {"simulation", "saveGenetics"},
        "simulation_params.json"
    );

        ///////////////////////////
        //  Checkpoints changes  //
        ///////////////////////////

            /***************************/
            /*  Add checkpoint section */
            /***************************/

    config.addJsonItem(
        {},
        {"simulation", "checkpoints"},
        {
            {"enabled", false},
            {"recordEach", nullptr},
            {"binaryEnabled", nullptr}
        },
        "simulation_params.json"
    );

        //////////////////////////////////////
        //  Resource growth module changes  //
        //////////////////////////////////////

            /*****************************/
            /*  Move cellMass parameter  */
            /*****************************/
    
    config.moveJsonItemAllFiles(
        {"resource"},
        {"resource", "cellMass"},
        {"resource", "growthModule", "cellMass"}
    );

            /****************************************/
            /*  Move conversionToWetMass parameter  */
            /****************************************/
    
    config.moveJsonItemAllFiles(
        {"resource"},
        {"resource", "conversionToWetMass"},
        {"resource", "growthModule", "conversionToWetMass"}
    );

        ////////////////////////
        //  Resource changes  //
        ////////////////////////

            /**************************************/
            /*  Remove NORMALIZATION_B parameter  */
            /**************************************/

    config.removeJsonItemAllFiles(
        {"resource"},
        {"resource", "NORMALIZATION_B"}
    );

            /****************************************/
            /*  Remove ACTIVATION_ENERGY parameter  */
            /****************************************/

    config.removeJsonItemAllFiles(
        {"resource"},
        {"resource", "ACTIVATION_ENERGY"}
    );

            /*************************************/
            /*  Rename minHR parameter to minRH  */
            /*************************************/
    
    config.moveJsonItemAllFiles(
        {"resource"},
        {"resource", "minHR"},
        {"resource", "minRH"}
    );

            /*************************************/
            /*  Rename maxHR parameter to maxRH  */
            /*************************************/
    
    config.moveJsonItemAllFiles(
        {"resource"},
        {"resource", "maxHR"},
        {"resource", "maxRH"}
    );

            /*********************************************/
            /*  Rename maxRScale parameter to rMaxScale  */
            /*********************************************/
    
    config.moveJsonItemAllFiles(
        {"resource"},
        {"resource", "maxRScale"},
        {"resource", "rMaxScale"}
    );

        ////////////////////////////////////////
        //  minVorExtremeT parameter changes  //
        ////////////////////////////////////////

            /*************************************/
            /*  Remove minVorExtremeT parameter  */
            /*************************************/

    config.removeJsonItemAllFiles(
        {"species"},
        {"animal", "minVorExtremeT"}
    );

        //////////////////////////
        //  New traits section  //
        //////////////////////////

            /****************************/
            /*  Add new traits section  */
            /****************************/

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits"},
        json::object()
    );

        /////////////////////////////////////////////
        //  Individual level traits order section  //
        /////////////////////////////////////////////

            /**********************************************/
            /*  Move individual level traits order value  */
            /**********************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "traits", "variableTraits", "order"},
        {"animal", "genetics", "traits", "individualLevelTraitsOrder"}
    );

    vector<string> speciesFolder = {"species"};

    for(auto& [filename, file] : config.getFolder(speciesFolder, speciesFolder.size())->getJsonFiles()) {
        vector<string> individualLevelTraitsOrderPath = {"animal", "genetics", "traits", "individualLevelTraitsOrder"};

        json* individualLevelTraitsOrder = file.getItem(individualLevelTraitsOrderPath, individualLevelTraitsOrderPath.size());

        for(unsigned int i = 0; i < individualLevelTraitsOrder->size(); i++)
        {
            individualLevelTraitsOrder->at(i) = static_cast<string>(individualLevelTraitsOrder->at(i)) + "$TraitValue";
        }
    }

        ////////////////////////////////
        //  Trait definition section  //
        ////////////////////////////////

            /***********************************/
            /*  Add traits definition section  */
            /***********************************/

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition"},
        json::object()
    );

            /***********************************************************/
            /*  Add species level traits to traits definition section  */
            /***********************************************************/
    
    config.regexAddJsonItemAllFiles(
        {"species"},
        {"^animal$", "^traits$", "^fixedTraits$", ".*"},
        {"animal", "genetics", "traits", "definition", "<match_3_0>"},
        json::object()
    );

            /**************************************************************/
            /*  Add individual level traits to traits definition section  */
            /**************************************************************/
    
    config.regexAddJsonItemAllFiles(
        {"species"},
        {"^animal$", "^traits$", "^variableTraits$", "^minTraitsRanges$", ".*"},
        {"animal", "genetics", "traits", "definition", "<match_4_0>"},
        json::object()
    );

            /*************************************************/
            /*  Remove unnecessary counter 'numberOfTraits'  */
            /*************************************************/
    
    config.removeJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "numberOfTraits"}
    );

        /////////////////////////////////////////
        //  Trait value update method section  //
        /////////////////////////////////////////
    
            /*************************************************************/
            /*  Add value update method section to species level traits  */
            /*************************************************************/
    
    config.regexAddJsonItemAllFiles(
        {"species"},
        {"^animal$", "^traits$", "^fixedTraits$", ".*"},
        {"animal", "genetics", "traits", "definition", "<match_3_0>"},
        {
            {"definitionType", "SpeciesLevel"},
            {
                "speciesLevelParams", 
                {
                    {"value", nullptr}
                }
            },
            {
                "individualLevelParams", 
                {
                    {
                        "limits", 
                        {
                            {"max", nullptr},
                            {"min", nullptr}
                        }
                    },
                    {
                        "ranges",
                        {
                            {"max", nullptr},
                            {"min", nullptr}
                        }
                    },
                    {"restrictValue", nullptr}
                }
            }
        }
    );

            /****************************************************************/
            /*  Add value update method section to individual level traits  */
            /****************************************************************/
    
    config.regexAddJsonItemAllFiles(
        {"species"},
        {"^animal$", "^traits$", "^variableTraits$", "^minTraitsRanges$", ".*"},
        {"animal", "genetics", "traits", "definition", "<match_4_0>"},
        {
            {"definitionType", "IndividualLevel"},
            {
                "speciesLevelParams", 
                {
                    {"value", nullptr}
                }
            },
            {
                "individualLevelParams", 
                {
                    {
                        "limits", 
                        {
                            {"max", nullptr},
                            {"min", nullptr}
                        }
                    },
                    {
                        "ranges",
                        {
                            {"max", nullptr},
                            {"min", nullptr}
                        }
                    },
                    {"restrictValue", nullptr}
                }
            }
        }
    );

            /************************************/
            /*  Add species level traits value  */
            /************************************/
    
    config.regexMoveJsonItemAllFiles(
        {"species"},
        {"^animal$", "^traits$", "^fixedTraits$", ".*"},
        {"animal", "traits", "fixedTraits", "<match_3_0>"},
        {"animal", "genetics", "traits", "definition", "<match_3_0>", "speciesLevelParams", "value"}
    );

            /****************************************/
            /*  Add individual level traits ranges  */
            /****************************************/
    
    config.regexCopyJsonItemAllFiles(
        {"species"},
        {"^animal$", "^traits$", "^variableTraits$", "^(max|min)TraitsRanges$", ".*"},
        {"animal", "traits", "variableTraits", "<match_3_1>TraitsRanges", "<match_4_0>"},
        {"animal", "genetics", "traits", "definition", "<match_4_0>", "individualLevelParams", "ranges", "<match_3_1>"}
    );

            /****************************************/
            /*  Add individual level traits limits  */
            /****************************************/
    
    config.regexCopyJsonItemAllFiles(
        {"species"},
        {"^animal$", "^traits$", "^variableTraits$", "^(max|min)TraitLimits$", ".*"},
        {"animal", "traits", "variableTraits", "<match_3_1>TraitLimits", "<match_4_0>"},
        {"animal", "genetics", "traits", "definition", "<match_4_0>", "individualLevelParams", "limits", "<match_3_1>"}
    );

            /*****************************************/
            /*  Convert value limits to percentages  */
            /*****************************************/

    for(auto& [filename, file] : config.getFolder(speciesFolder, speciesFolder.size())->getJsonFiles()) {
        vector<string> traitDefinitionPath = {"animal", "genetics", "traits", "definition"};

        json* traitDefinition = file.getItem(traitDefinitionPath, traitDefinitionPath.size());

        for(auto& [traitName, traitInfo] : traitDefinition->items()) {
            if(traitInfo["definitionType"] == "IndividualLevel")
            {
                double minTraitLimitValue = traitInfo["individualLevelParams"]["limits"]["min"];
                double maxTraitLimitValue = traitInfo["individualLevelParams"]["limits"]["max"];

                double minTraitRangeValue = traitInfo["individualLevelParams"]["ranges"]["min"];
                double maxTraitRangeValue = traitInfo["individualLevelParams"]["ranges"]["max"];

                double traitRangeValue = maxTraitRangeValue - minTraitRangeValue;

                traitInfo["individualLevelParams"]["limits"]["min"] = ((minTraitRangeValue - minTraitLimitValue) / traitRangeValue) / 100;
                traitInfo["individualLevelParams"]["limits"]["max"] = ((maxTraitLimitValue - maxTraitRangeValue) / traitRangeValue) / 100;
            }
        }
    }

            /******************************************/
            /*  Add individual level traits restrict  */
            /******************************************/
    
    for(auto& [filename, file] : config.getFolder(speciesFolder, speciesFolder.size())->getJsonFiles()) {
        vector<string> individualLevelTraitsOrderPath = {"animal", "genetics", "traits", "individualLevelTraitsOrder"};

        json* individualLevelTraitsOrder = file.getItem(individualLevelTraitsOrderPath, individualLevelTraitsOrderPath.size());

        for(unsigned int i = 0; i < individualLevelTraitsOrder->size(); i++)
        {
            std::string input = individualLevelTraitsOrder->at(i);
            regex pattern("^(\\w+)\\$\\w+$");
            
            smatch matches;
            if(regex_match(input, matches, pattern)) {
                config.copyJsonItem(
                    {"species"},
                    {"animal", "genetics", "restrictPerTrait", "item_" + to_string(i)},
                    {"animal", "genetics", "traits", "definition", static_cast<string>(matches[1]), "individualLevelParams", "restrictValue"},
                    filename
                );
            }
        }
    }

            /*********************************************************/
            /*  Remove old individual level traits restrict section  */
            /*********************************************************/
    
    config.removeJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "restrictPerTrait"}
    );


        //////////////////////
        //  Add new traits  //
        //////////////////////

            /***************************/
            /*  Add traits definition  */
            /***************************/

    for(const string trait : {"devTime", "lengthAtMaturation", "pupaPeriodTime", "longevitySinceMaturation", "dummy"}) {
        config.addJsonItemAllFiles(
            {"species"},
            {"animal", "genetics", "traits", "definition", trait},
            {
                {"definitionType", "SpeciesLevel"},
                {
                    "speciesLevelParams", 
                    {
                        {"value", nullptr}
                    }
                },
                {
                    "individualLevelParams", 
                    {
                        {
                            "limits", 
                            {
                                {"max", nullptr},
                                {"min", nullptr}
                            }
                        },
                        {
                            "ranges",
                            {
                                {"max", nullptr},
                                {"min", nullptr}
                            }
                        },
                        {"restrictValue", nullptr}
                    }
                }
            }
        );
    }

            /*****************************/
            /*  Set devTime trait value  */
            /*****************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "devTimeConstant"},
        {"animal", "genetics", "traits", "definition", "devTime", "speciesLevelParams", "value"}
    );

            /****************************************/
            /*  Set pupa period time trait value  */
            /****************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "pupaPeriodLength"},
        {"animal", "genetics", "traits", "definition", "pupaPeriodTime", "speciesLevelParams", "value"}
    );

            /************************************************/
            /*  Set longevity since maturation trait value  */
            /************************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "longevitySinceMaturation"},
        {"animal", "genetics", "traits", "definition", "longevitySinceMaturation", "speciesLevelParams", "value"}
    );

            /***************************/
            /*  Set dummy trait value  */
            /***************************/

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "dummy", "definitionType"},
        "IndividualLevel"
    );

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "dummy", "individualLevelParams", "limits", "min"},
        0.0
    );

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "dummy", "individualLevelParams", "limits", "max"},
        0.0
    );

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "dummy", "individualLevelParams", "ranges", "min"},
        0.0
    );

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "dummy", "individualLevelParams", "ranges", "max"},
        1.0
    );

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "dummy", "individualLevelParams", "restrictValue"},
        0.0
    );

    for(auto& [filename, file] : config.getFolder(speciesFolder, speciesFolder.size())->getJsonFiles()) {
        vector<string> individualLevelTraitsOrderPath = {"animal", "genetics", "traits", "individualLevelTraitsOrder"};

        json* individualLevelTraitsOrder = file.getItem(individualLevelTraitsOrderPath, individualLevelTraitsOrderPath.size());

        individualLevelTraitsOrder->push_back(string("dummy$TraitValue"));
    }

        ////////////////////////////////////////////
        //  Trait temperature dependency section  //
        ////////////////////////////////////////////

            /***************************************************************************************/
            /*  Add temperature section to all traits except energy tank and length at maturation  */
            /***************************************************************************************/

    config.regexAddJsonItemAllFiles(
        {"species"},
        {"^animal$", "^genetics$", "^traits$", "^definition$", "^(?!energy_tank$|lengthAtMaturation$).*$"},
        {"animal", "genetics", "traits", "definition", "<match_4_0>", "temperature"},
        {
            {"dependent", false},
            {
                "threshold",
                {
                    {
                        "lower", 
                        {
                            {"definitionType", "SpeciesLevel"},
                            {
                                "speciesLevelParams", 
                                {
                                    {"value", nullptr}
                                }
                            },
                            {
                                "individualLevelParams", 
                                {
                                    {
                                        "limits", 
                                        {
                                            {"max", nullptr},
                                            {"min", nullptr}
                                        }
                                    },
                                    {
                                        "ranges",
                                        {
                                            {"max", nullptr},
                                            {"min", nullptr}
                                        }
                                    },
                                    {"restrictValue", nullptr}
                                }
                            }
                        }
                    },
                    {
                        "upper", 
                        {
                            {"definitionType", "SpeciesLevel"},
                            {
                                "speciesLevelParams", 
                                {
                                    {"value", nullptr}
                                }
                            },
                            {
                                "individualLevelParams", 
                                {
                                    {
                                        "limits", 
                                        {
                                            {"max", nullptr},
                                            {"min", nullptr}
                                        }
                                    },
                                    {
                                        "ranges",
                                        {
                                            {"max", nullptr},
                                            {"min", nullptr}
                                        }
                                    },
                                    {"restrictValue", nullptr}
                                }
                            }
                        }
                    }
                }
            },
            {
                "activationEnergy", 
                {
                    {"definitionType", "SpeciesLevel"},
                    {
                        "speciesLevelParams", 
                        {
                            {"value", nullptr}
                        }
                    },
                    {
                        "individualLevelParams", 
                        {
                            {
                                "limits", 
                                {
                                    {"max", nullptr},
                                    {"min", nullptr}
                                }
                            },
                            {
                                "ranges",
                                {
                                    {"max", nullptr},
                                    {"min", nullptr}
                                }
                            },
                            {"restrictValue", nullptr}
                        }
                    }
                }
            },
            {
                "energyDecay",
                {
                    {"definitionType", "SpeciesLevel"},
                    {
                        "speciesLevelParams", 
                        {
                            {"value", nullptr}
                        }
                    },
                    {
                        "individualLevelParams", 
                        {
                            {
                                "limits", 
                                {
                                    {"max", nullptr},
                                    {"min", nullptr}
                                }
                            },
                            {
                                "ranges",
                                {
                                    {"max", nullptr},
                                    {"min", nullptr}
                                }
                            },
                            {"restrictValue", nullptr}
                        }
                    }
                }
            },
            {
                "temperatureOptimal", 
                {
                    {"definitionType", "SpeciesLevel"},
                    {
                        "speciesLevelParams", 
                        {
                            {"value", nullptr}
                        }
                    },
                    {
                        "individualLevelParams", 
                        {
                            {
                                "limits", 
                                {
                                    {"max", nullptr},
                                    {"min", nullptr}
                                }
                            },
                            {
                                "ranges",
                                {
                                    {"max", nullptr},
                                    {"min", nullptr}
                                }
                            },
                            {"restrictValue", nullptr}
                        }
                    }
                }
            },
            {
                "traitAtTempRef", 
                {
                    {"definitionType", "SpeciesLevel"},
                    {
                        "speciesLevelParams", 
                        {
                            {"value", nullptr}
                        }
                    },
                    {
                        "individualLevelParams", 
                        {
                            {
                                "limits", 
                                {
                                    {"max", nullptr},
                                    {"min", nullptr}
                                }
                            },
                            {
                                "ranges",
                                {
                                    {"max", nullptr},
                                    {"min", nullptr}
                                }
                            },
                            {"restrictValue", nullptr}
                        }
                    }
                }
            },
            {
                "temperatureRef", 
                {
                    {"definitionType", "SpeciesLevel"},
                    {
                        "speciesLevelParams", 
                        {
                            {"value", nullptr}
                        }
                    },
                    {
                        "individualLevelParams", 
                        {
                            {
                                "limits", 
                                {
                                    {"max", nullptr},
                                    {"min", nullptr}
                                }
                            },
                            {
                                "ranges",
                                {
                                    {"max", nullptr},
                                    {"min", nullptr}
                                }
                            },
                            {"restrictValue", nullptr}
                        }
                    }
                }
            }
        }
    );

            /*****************************************************************************/
            /*  Add new temperature size rule section to the trait length at maturation  */
            /*****************************************************************************/
    
    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "lengthAtMaturation", "temperature"},
        {
            {"dependent", false},
            {"tempSizeRuleVector", nullptr}
        }
    );

    config.removeJsonItemAllFiles(
        {"species"},
        {"animal", "tempSizeRuleConstant"}
    );



    vector<tuple<string, string, string, string>> traitsThermoDependent = {
        {"voracity", "actE_vor", "EdVoracity", "tempOptVoracity"},
        {"speed", "actE_speed", "EdSpeed", "tempOptSpeed"},
        {"search_area", "actE_search", "EdSearch", "tempOptSearch"}
    };

    for(const auto& [trait, actE_trait, EdTrait, tempOptTrait] : traitsThermoDependent) {
            /*************************************************/
            /*  Activate the thermo-dependency of the trait  */
            /*************************************************/
        
        config.addJsonItemAllFiles(
            {"species"},
            {"animal", "genetics", "traits", "definition", trait, "temperature", "dependent"},
            true
        );

            /********************************************/
            /*  Set temperature threshold of the trait  */
            /********************************************/
        
        config.addJsonItemAllFiles(
            {"species"},
            {"animal", "genetics", "traits", "definition", trait, "temperature", "threshold", "lower", "speciesLevelParams", "value"},
            1.0
        );

        config.addJsonItemAllFiles(
            {"species"},
            {"animal", "genetics", "traits", "definition", trait, "temperature", "threshold", "upper", "speciesLevelParams", "value"},
            35.0
        );

            /****************************************/
            /*  Set activation energy of the trait  */
            /****************************************/
        
        for(auto& [filename, file] : config.getFolder(speciesFolder, speciesFolder.size())->getJsonFiles()) {
            vector<string> definitionTypePath = {"animal", "genetics", "traits", "definition", actE_trait, "definitionType"};
            
            const string definitionType = static_cast<const string>(*file.getItem(definitionTypePath, definitionTypePath.size()));
        
            if(definitionType == "SpeciesLevel")
            {
                vector<string> initial_item_path = {"animal", "genetics", "traits", "definition", actE_trait, "speciesLevelParams", "value"};
                vector<string> final_item_path = {"animal", "genetics", "traits", "definition", trait, "temperature", "activationEnergy", "speciesLevelParams", "value"};

                config.copyJsonItem(
                    speciesFolder,
                    initial_item_path,
                    final_item_path,
                    filename
                );
            }
            else if(definitionType == "IndividualLevel")
            {
                vector<string> traitMinRangePath = {"animal", "genetics", "traits", "definition", actE_trait, "individualLevelParams", "ranges", "min"};
                vector<string> traitMaxRangePath = {"animal", "genetics", "traits", "definition", actE_trait, "individualLevelParams", "ranges", "max"};

                const double minRange = static_cast<const double>(*file.getItem(traitMinRangePath, traitMinRangePath.size()));
                const double maxRange = static_cast<const double>(*file.getItem(traitMaxRangePath, traitMaxRangePath.size()));

                vector<string> item_path = {"animal", "genetics", "traits", "definition", trait, "temperature", "activationEnergy", "speciesLevelParams", "value"};

                config.addJsonItem(
                    speciesFolder,
                    item_path,
                    (maxRange + minRange) / 2.0,
                    filename
                );


                /**************************************************************************/
                /*  Remove activation energy trait from individualLevelTraitsOrder vector */
                /**************************************************************************/

                vector<string> individualLevelTraitsOrderPath = {"animal", "genetics", "traits", "individualLevelTraitsOrder"};

                json* individualLevelTraitsOrder = file.getItem(individualLevelTraitsOrderPath, individualLevelTraitsOrderPath.size());

                unsigned int actE_trait_pos;
                for(unsigned int i = 0; i < individualLevelTraitsOrder->size(); i++)
                {
                    if(individualLevelTraitsOrder->at(i) == (actE_trait + "$TraitValue"))
                    {
                        actE_trait_pos = i;
                    }
                }

                individualLevelTraitsOrder->erase(actE_trait_pos);
            }
        }

            /***********************************/
            /*  Set energy decay of the trait  */
            /***********************************/
        
        config.moveJsonItemAllFiles(
            {"species"},
            {"animal", EdTrait},
            {"animal", "genetics", "traits", "definition", trait, "temperature", "energyDecay", "speciesLevelParams", "value"}
        );

            /******************************************/
            /*  Set temperature optimal of the trait  */
            /******************************************/
        
        config.moveJsonItemAllFiles(
            {"species"},
            {"animal", tempOptTrait},
            {"animal", "genetics", "traits", "definition", trait, "temperature", "temperatureOptimal", "speciesLevelParams", "value"}
        );

            /****************************************/
            /*  Remove old activation energy trait  */
            /****************************************/
        
        config.removeJsonItemAllFiles(
            {"species"},
            {"animal", "genetics", "traits", "definition", actE_trait}
        );
    }


            /********************************************************/
            /*  Activate the thermo-dependency of the growth trait  */
            /********************************************************/
        
    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "growth", "temperature", "dependent"},
        true
    );

            /********************************************/
            /*  Set temperature threshold of the trait  */
            /********************************************/
        
    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "growth", "temperature", "threshold", "lower", "speciesLevelParams", "value"},
        1.0
    );

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "growth", "temperature", "threshold", "upper", "speciesLevelParams", "value"},
        35.0
    );

            /****************************************/
            /*  Set activation energy of the trait  */
            /****************************************/
        
    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "growth", "temperature", "activationEnergy", "speciesLevelParams", "value"},
        0.67
    );

            /***********************************/
            /*  Set energy decay of the trait  */
            /***********************************/
        
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "EdGrowth"},
        {"animal", "genetics", "traits", "definition", "growth", "temperature", "energyDecay", "speciesLevelParams", "value"}
    );

            /******************************************/
            /*  Set temperature optimal of the trait  */
            /******************************************/
        
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "tempOptGrowth"},
        {"animal", "genetics", "traits", "definition", "growth", "temperature", "temperatureOptimal", "speciesLevelParams", "value"}
    );


        //////////////////////////////////////////////
        //  Rename pheno trait to eggDevTime trait  //
        //////////////////////////////////////////////

            /*****************************/
            /*  Rename trait definition  */
            /*****************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traits", "definition", "pheno"},
        {"animal", "genetics", "traits", "definition", "eggDevTime"}
    );

    for(auto& [filename, file] : config.getFolder(speciesFolder, speciesFolder.size())->getJsonFiles()) {
        vector<string> definitionTypePath = {"animal", "genetics", "traits", "definition", "eggDevTime", "definitionType"};
        
        const string definitionType = static_cast<const string>(*file.getItem(definitionTypePath, definitionTypePath.size()));

        if(definitionType == "IndividualLevel")
        {
            /********************************************************/
            /*  Rename trait from individualLevelTraitsOrder vector */
            /********************************************************/

            vector<string> individualLevelTraitsOrderPath = {"animal", "genetics", "traits", "individualLevelTraitsOrder"};

            json* individualLevelTraitsOrder = file.getItem(individualLevelTraitsOrderPath, individualLevelTraitsOrderPath.size());

            unsigned int pheno_trait_pos;
            for(unsigned int i = 0; i < individualLevelTraitsOrder->size(); i++)
            {
                if(individualLevelTraitsOrder->at(i) == "pheno$TraitValue")
                {
                    pheno_trait_pos = i;
                }
            }

            individualLevelTraitsOrder->at(pheno_trait_pos) = "eggDevTime$TraitValue";
        }
    }


        /////////////////////////////////
        //  Remove old traits section  //
        /////////////////////////////////

            /*********************************************/
            /*  Remove old species level traits section  */
            /*********************************************/

    config.removeJsonItemAllFiles(
        {"species"},
        {"animal", "traits"}
    );

        ////////////////////////////////
        //  Genetics modules section  //
        ////////////////////////////////
    
            /*************************/
            /*  Add modules section  */
            /*************************/

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "modules"},
        json::object()
    );

            /**********************************/
            /*  Move traits per module value  */
            /**********************************/
    
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "traitsPerModule"},
        {"animal", "genetics", "modules", "traitsPerModule"}
    );

            /************************************************/
            /*  Move correlation coefficientRho per module  */
            /************************************************/
    
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "genetics", "correlationCoefficientRhoPerModule"},
        {"animal", "genetics", "modules", "correlationCoefficientRhoPerModule"}
    );

        /////////////////////////////
        //  Growth module section  //
        /////////////////////////////

            /***********************************/
            /*  Add new growth module section  */
            /***********************************/
    
    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "growthModule"},
        json::object()
    );

            /*********************************************/
            /*  Remove old growth correlation parameter  */
            /*********************************************/
    
    config.removeJsonItemAllFiles(
        {"species"},
        {"animal", "LinfKcorr"}
    );

            /********************************************/
            /*  Move development time vector parameter  */
            /********************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "devTimeVector"},
        {"animal", "growthModule", "devTimeVector"}
    );

            /************************************/
            /*  Move assignedForMolt parameter  */
            /************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "assignedForMolt"},
        {"animal", "growthModule", "assignedForMolt"}
    );

            /*************************************************/
            /*  Move maxPlasticityKVonBertalanffy parameter  */
            /*************************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "maxPlasticityKVonBertalanffy"},
        {"animal", "growthModule", "maxPlasticityKVonBertalanffy"}
    );

            /*************************************************/
            /*  Move minPlasticityKVonBertalanffy parameter  */
            /*************************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "minPlasticityKVonBertalanffy"},
        {"animal", "growthModule", "minPlasticityKVonBertalanffy"}
    );

            /*************************************************/
            /*  Move coefficientForMassAforMature parameter  */
            /*************************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "coefficientForMassAforMature"},
        {"animal", "growthModule", "coefficientForMassAforMature"}
    );

            /*******************************************/
            /*  Move scaleForMassBforMature parameter  */
            /*******************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "scaleForMassBforMature"},
        {"animal", "growthModule", "scaleForMassBforMature"}
    );

            /****************************************/
            /*  Move coefficientForMassA parameter  */
            /****************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "coefficientForMassA"},
        {"animal", "growthModule", "coefficientForMassA"}
    );

            /**********************************/
            /*  Move scaleForMassB parameter  */
            /**********************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "scaleForMassB"},
        {"animal", "growthModule", "scaleForMassB"}
    );

            /**********************************/
            /*  Move betaScaleTank parameter  */
            /**********************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "betaScaleTank"},
        {"animal", "growthModule", "betaScaleTank"}
    );

            /***************************************/
            /*  Move excessInvestInSize parameter  */
            /***************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "excessInvestInSize"},
        {"animal", "growthModule", "excessInvestInSize"}
    );

            /************************************/
            /*  Move female wet mass parameter  */
            /************************************/
    
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "femaleWetMass"},
        {"animal", "growthModule", "femaleWetMass"}
    );

            /*********************************/
            /*  Move habitatShift parameter  */
            /*********************************/
    
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "habitatShift"},
        {"animal", "growthModule", "habitatShift"}
    );

            /***************************************/
            /*  Move habitatShiftFactor parameter  */
            /***************************************/
    
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "habitatShiftFactor"},
        {"animal", "growthModule", "habitatShiftFactor"}
    );

            /*************************************/
            /*  Add new egg fertility parameter  */
            /*************************************/
    
    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "growthModule", "eggFertility"},
        1.0
    );

            /******************************/
            /*  Add new egg mass section  */
            /******************************/
    
    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "growthModule", "eggDryMass"},
        json::object()
    );

            /****************************************/
            /*  Move eggMassFromEquation parameter  */
            /****************************************/
    
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "eggMassFromEquation"},
        {"animal", "growthModule", "eggDryMass", "eggDryMassFromEquation"}
    );

            /*******************************/
            /*  Move eggDryMass parameter  */
            /*******************************/
    
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "eggDryMass"},
        {"animal", "growthModule", "eggDryMass", "value"}
    );

            /***************************************/
            /*  Add new egg mass equation section  */
            /***************************************/
    
    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "growthModule", "eggDryMass", "equationParameters"},
        json::object()
    );

            /******************************************/
            /*  Move forEggMassCoefficient parameter  */
            /******************************************/
    
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "forEggMassCoefficient"},
        {"animal", "growthModule", "eggDryMass", "equationParameters", "coefficient"}
    );

            /************************************/
            /*  Move forEggMassScale parameter  */
            /************************************/
    
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "forEggMassScale"},
        {"animal", "growthModule", "eggDryMass", "equationParameters", "scale"}
    );

            /**************************************/
            /*  Move growth model type parameter  */
            /**************************************/
    
    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "growthCurve", "curveType"},
        {"animal", "growthModule", "growthModel"}
    );

            /*************************************/
            /*  Remove old growth curve section  */
            /*************************************/
    
    config.removeJsonItemAllFiles(
        {"species"},
        {"animal", "growthCurve"}
    );


    for(auto& [filename, file] : config.getFolder(speciesFolder, speciesFolder.size())->getJsonFiles()) {
            
            /******************************************/
            /*  Add new indeterminate growth section  */
            /******************************************/

        config.addJsonItem(
            {"species"},
            {"animal", "growthModule", "indeterminateGrowth"},
            {
                {"enabled", nullptr},
                {
                    "indeterminateGrowthParams",
                    {
                        {"instarFirstReproduction", nullptr}
                    }
                }
            },
            filename
        );

            /************************************/
            /*  Set indeterminate growth value  */
            /************************************/

        config.moveJsonItem(
            {"species"},
            {"animal", "indeterminateGrowth"},
            {"animal", "growthModule", "indeterminateGrowth", "enabled"},
            filename
        );


        vector<string> indeterminateGrowthPath = {"animal", "growthModule", "indeterminateGrowth", "enabled"};
        
        const bool indeterminateGrowthEnabled = *file.getItem(indeterminateGrowthPath, indeterminateGrowthPath.size());


        if(indeterminateGrowthEnabled)
        {
            /*****************************************/
            /*  Set instar first reproduction value  */
            /*****************************************/

            config.moveJsonItem(
                {"species"},
                {"animal", "instarFirstReproduction"},
                {"animal", "growthModule", "indeterminateGrowth", "indeterminateGrowthParams", "instarFirstReproduction"},
                filename
            );
        }
        else
        {
            config.removeJsonItem(
                {"species"},
                {"animal", "instarFirstReproduction"},
                filename
            );
        }
    }


        ////////////////////////
        //  Capital breeding  //
        ////////////////////////

            /**************************************/
            /*  Add new capital breeding section  */
            /**************************************/
    
    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "growthModule", "capitalBreeding"},
        {
            {"enabled", false},
            {
                "capitalBreedingParams",
                {
                    {"timeOfReproEventDuringCapitalBreeding", nullptr},
                    {"numberOfCapitalBreeds", nullptr}
                }
            }
        }
    );


        //////////////////////
        //  Eggs per batch  //
        //////////////////////

            /************************************/
            /*  Add new eggs per batch section  */
            /************************************/
    
    for(auto& [filename, file] : config.getFolder(speciesFolder, speciesFolder.size())->getJsonFiles()) {
        vector<string> eggsPerBatchPath = {"animal", "eggsPerBatch"};
        
        const unsigned int eggsPerBatchPathValue = *file.getItem(eggsPerBatchPath, eggsPerBatchPath.size());

        config.addJsonItem(
            {"species"},
            {"animal", "growthModule", "eggsPerBatch"},
            {
                {"eggsPerBatchFromEquation", false},
                {"value", eggsPerBatchPathValue},
                {
                    "equationParameters",
                    {
                        {"interceptForEggBatchFromEquation", nullptr},
                        {"slopeForEggBatchFromEquation", nullptr}
                    }
                }
            },
            filename
        );

        config.removeJsonItem(
            {"species"},
            {"animal", "eggsPerBatch"},
            filename
        );
    }


        ////////////////////////////
        //  Target age threshold  //
        ////////////////////////////
    
            /********************************/
            /*  Add moulting age threshold  */
            /********************************/

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "growthModule", "moltingAgeThreshold"},
        nullptr
    );


        /////////////////////////
        //  Yodzis parameters  //
        /////////////////////////
    
            /*****************************/
            /*  Add newAAdult parameter  */
            /*****************************/

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "newAAdult"},
        nullptr
    );

            /************************/
            /*  Add newB parameter  */
            /************************/

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "newB"},
        nullptr
    );


        //////////////////
        //  Enhanced h  //
        //////////////////

            /*********************************/
            /*  Add h_enhancement parameter  */
            /*********************************/

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "h_enhancement"},
        nullptr
    );


        ////////////////
        //  Handling  //
        ////////////////

            /*************************************/
            /*  Add activatedHandling parameter  */
            /*************************************/

    config.addJsonItemAllFiles(
        {"species"},
        {"animal", "activatedHandling"},
        false
    );


        ///////////////////////////////////
        //  Plasticity due to condition  //
        ///////////////////////////////////

    config.removeJsonItemAllFiles(
        {"species"},
        {"animal", "minPlasticityDueToConditionVor"}
    );

    config.removeJsonItemAllFiles(
        {"species"},
        {"animal", "maxPlasticityDueToConditionVor"}
    );


    vector<tuple<string, string, string>> traitsWithPlasticity = {
        {"minPlasticityDueToConditionSearch", "maxPlasticityDueToConditionSearch", "plasticityDueToConditionSearch"},
        {"minPlasticityDueToConditionSpeed", "maxPlasticityDueToConditionSpeed", "plasticityDueToConditionSpeed"}
    };

    for(auto& [filename, file] : config.getFolder(speciesFolder, speciesFolder.size())->getJsonFiles()) {
        for(const auto& [minPlasticityDueToCondition, maxPlasticityDueToCondition, plasticityDueToCondition] : traitsWithPlasticity) {
            vector<string> minPlasticityDueToConditionPath = {"animal", minPlasticityDueToCondition};
            const double minPlasticityValue = *file.getItem(minPlasticityDueToConditionPath, minPlasticityDueToConditionPath.size());

            vector<string> maxPlasticityDueToConditionPath = {"animal", maxPlasticityDueToCondition};
            const double maxPlasticityValue = *file.getItem(maxPlasticityDueToConditionPath, maxPlasticityDueToConditionPath.size());

            double plasticityValue = (maxPlasticityValue + minPlasticityValue) / 2;

            /********************************************/
            /*  Add plasticityDueToCondition parameter  */
            /********************************************/

            config.addJsonItem(
                {"species"},
                {"animal", plasticityDueToCondition},
                plasticityValue,
                filename
            );

            /****************************************************/
            /*  Remove old plasticityDueToCondition parameters  */
            /****************************************************/

            config.removeJsonItem(
                {"species"},
                {"animal", minPlasticityDueToCondition},
                filename
            );

            config.removeJsonItem(
                {"species"},
                {"animal", maxPlasticityDueToCondition},
                filename
            );
        }
    }


        ////////////////////////////////////////////
        //  Decrease on traits due to encounters  //
        ////////////////////////////////////////////

            /************************************************************************************/
            /*  Rename decreaseOnTraitsDueToEncounters parameter to activityUnderPredationRisk  */
            /************************************************************************************/

    config.moveJsonItemAllFiles(
        {"species"},
        {"animal", "decreaseOnTraitsDueToEncounters"},
        {"animal", "activityUnderPredationRisk"}
    );
}
