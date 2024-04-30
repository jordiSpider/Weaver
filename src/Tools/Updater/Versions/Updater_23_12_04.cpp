
#include "Tools/Updater/Versions/Updater_23_12_04.h"


using namespace std;
using json = nlohmann::json;


Updater_23_12_04::Updater_23_12_04()
    : Updater("23.12.04")
{
    //////////////////////////
    //  Version:  23.12.04  //
    //////////////////////////

        ////////////////////////////////////////
        //  minVorExtremeT parameter changes  //
        ////////////////////////////////////////

    addChanges(
        {
            /*************************************/
            /*  Remove minVorExtremeT parameter  */
            /*************************************/
            new RemoveJsonItemAllFilesChange(
                {"species"},
                {"animal", "minVorExtremeT"}
            )
        }
    );

        //////////////////////////
        //  New traits section  //
        //////////////////////////

    addChanges(
        {
            /****************************/
            /*  Add new traits section  */
            /****************************/
            new AddJsonItemAllFilesChange(
                {"species"},
                {"animal", "genetics", "traits"},
                json::object()
            )
        }
    );

        /////////////////////////////////////
        //  Variable traits order section  //
        /////////////////////////////////////

    addChanges(
        {
            /**************************************/
            /*  Move variable traits order value  */
            /**************************************/
            new MoveJsonItemAllFilesChange(
                {"species"},
                {"animal", "traits", "variableTraits", "order"},
                {"animal", "genetics", "traits", "variableTraitsOrder"}
            )
        }
    );

        ////////////////////////////////
        //  Trait definition section  //
        ////////////////////////////////

    addChanges(
        {
            /***********************************/
            /*  Add traits definition section  */
            /***********************************/
            new AddJsonItemAllFilesChange(
                {"species"},
                {"animal", "genetics", "traits", "definition"},
                json::object()
            ),
            /***************************************************/
            /*  Add fixed traits to traits definition section  */
            /***************************************************/
            new RegexAddJsonItemAllFilesChange(
                {"species"},
                {"^animal$", "^traits$", "^fixedTraits$", ".*"},
                {"animal", "genetics", "traits", "definition", "<match_3_0>"},
                json::object()
            ),
            /******************************************************/
            /*  Add variable traits to traits definition section  */
            /******************************************************/
            new RegexAddJsonItemAllFilesChange(
                {"species"},
                {"^animal$", "^traits$", "^variableTraits$", "^minTraitsRanges$", ".*"},
                {"animal", "genetics", "traits", "definition", "<match_4_0>"},
                json::object()
            ),
            /*************************************************/
            /*  Remove unnecessary counter 'numberOfTraits'  */
            /*************************************************/
            new RemoveJsonItemAllFilesChange(
                {"species"},
                {"animal", "genetics", "numberOfTraits"}
            )
        }
    );

        /////////////////////////////////////////
        //  Trait value update method section  //
        /////////////////////////////////////////
    
    addChanges(
        {
            /*****************************************************/
            /*  Add value update method section to fixed traits  */
            /*****************************************************/
            new RegexAddJsonItemAllFilesChange(
                {"species"},
                {"^animal$", "^traits$", "^fixedTraits$", ".*"},
                {"animal", "genetics", "traits", "definition", "<match_3_0>", "valueUpdateMethod"},
                {
                    {"type", nullptr},
                    {
                        "fixedParams", 
                        {
                            {"value", nullptr}
                        }
                    },
                    {
                        "variableParams", 
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
            ),
            /********************************************************/
            /*  Add value update method section to variable traits  */
            /********************************************************/
            new RegexAddJsonItemAllFilesChange(
                {"species"},
                {"^animal$", "^traits$", "^variableTraits$", "^minTraitsRanges$", ".*"},
                {"animal", "genetics", "traits", "definition", "<match_4_0>", "valueUpdateMethod"},
                {
                    {"type", nullptr},
                    {
                        "fixedParams", 
                        {
                            {"value", nullptr}
                        }
                    },
                    {
                        "variableParams", 
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
            ),
            /********************/
            /*  Set fixed type  */
            /********************/
            new RegexAddJsonItemAllFilesChange(
                {"species"},
                {"^animal$", "^traits$", "^fixedTraits$", ".*"},
                {"animal", "genetics", "traits", "definition", "<match_3_0>", "valueUpdateMethod", "type"},
                "Fixed"
            ),
            /****************************/
            /*  Add fixed traits value  */
            /****************************/
            new RegexMoveJsonItemAllFilesChange(
                {"species"},
                {"^animal$", "^traits$", "^fixedTraits$", ".*"},
                {"animal", "traits", "fixedTraits", "<match_3_0>"},
                {"animal", "genetics", "traits", "definition", "<match_3_0>", "valueUpdateMethod", "fixedParams", "value"}
            ),
            /***********************/
            /*  Set variable type  */
            /***********************/
            new RegexAddJsonItemAllFilesChange(
                {"species"},
                {"^animal$", "^traits$", "^variableTraits$", "^minTraitsRanges$", ".*"},
                {"animal", "genetics", "traits", "definition", "<match_4_0>", "valueUpdateMethod", "type"},
                "Variable"
            ),
            /********************************/
            /*  Add variable traits limits  */
            /********************************/
            new RegexCopyJsonItemAllFilesChange(
                {"species"},
                {"^animal$", "^traits$", "^variableTraits$", "^(max|min)TraitLimits$", ".*"},
                {"animal", "traits", "variableTraits", "<match_3_1>TraitLimits", "<match_4_0>"},
                {"animal", "genetics", "traits", "definition", "<match_4_0>", "valueUpdateMethod", "variableParams", "limits", "<match_3_1>"}
            ),
            /********************************/
            /*  Add variable traits ranges  */
            /********************************/
            new RegexCopyJsonItemAllFilesChange(
                {"species"},
                {"^animal$", "^traits$", "^variableTraits$", "^(max|min)TraitsRanges$", ".*"},
                {"animal", "traits", "variableTraits", "<match_3_1>TraitsRanges", "<match_4_0>"},
                {"animal", "genetics", "traits", "definition", "<match_4_0>", "valueUpdateMethod", "variableParams", "ranges", "<match_3_1>"}
            ),
            /**********************************/
            /*  Add variable traits restrict  */
            /**********************************/
            new RegexCopyJsonItemAllFilesChange(
                {"species"},
                {"^animal$", "genetics", "^traits$", "^variableTraitsOrder$", "^[0-9]+$"},
                {"animal", "genetics", "restrictPerTrait", "item_<match_4_0>"},
                {"animal", "genetics", "traits", "definition", "<regex_content>", "valueUpdateMethod", "variableParams", "restrictValue"}
            ),
            /*************************************************/
            /*  Remove old variable traits restrict section  */
            /*************************************************/
            new RemoveJsonItemAllFilesChange(
                {"species"},
                {"animal", "genetics", "restrictPerTrait"}
            )
        }
    );

        ///////////////////////////////////////////
        //  Trait temperature threshold section  //
        ///////////////////////////////////////////

    addChanges(
        {
            /*************************************************/
            /*  Add temperature threshold section to traits  */
            /*************************************************/
            new RegexAddJsonItemAllFilesChange(
                {"species"},
                {"^animal$", "^genetics$", "^traits$", "^definition$", ".*"},
                {"animal", "genetics", "traits", "definition", "<match_4_0>", "temperatureThreshold"},
                {
                    {"lower", 0.0},
                    {"upper", 35.0}
                }
            )
        }
    );

        /////////////////////////////////
        //  Remove old traits section  //
        /////////////////////////////////

    addChanges(
        {
            /*************************************/
            /*  Remove old fixed traits section  */
            /*************************************/
            new RemoveJsonItemAllFilesChange(
                {"species"},
                {"animal", "traits"}
            )
        }
    );

        ////////////////////////////////
        //  Genetics modules section  //
        ////////////////////////////////
    
    addChanges(
        {
            /*************************/
            /*  Add modules section  */
            /*************************/
            new AddJsonItemAllFilesChange(
                {"species"},
                {"animal", "genetics", "modules"},
                json::object()
            ),
            /**********************************/
            /*  Move traits per module value  */
            /**********************************/
            new MoveJsonItemAllFilesChange(
                {"species"},
                {"animal", "genetics", "traitsPerModule"},
                {"animal", "genetics", "modules", "traitsPerModule"}
            ),
            /************************************************/
            /*  Move correlation coefficientRho per module  */
            /************************************************/
            new MoveJsonItemAllFilesChange(
                {"species"},
                {"animal", "genetics", "correlationCoefficientRhoPerModule"},
                {"animal", "genetics", "modules", "correlationCoefficientRhoPerModule"}
            )
        }
    );
}

Updater_23_12_04::~Updater_23_12_04()
{
    
}
