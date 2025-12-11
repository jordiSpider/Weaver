
#include "App/View/GUI/Widgets/PathChooser.h"


using namespace std;
namespace fs = std::filesystem;



PathChooser::PathChooser(nana::window parent, const string& labelText, const bool isDirectory)
    : nana::panel<true>(parent), isDirectory(isDirectory)
{
    label.caption(labelText);
    label.text_align(nana::align::right, nana::align_v::center);

    textbox.editable(false);
    textbox.multi_lines(false);
    textbox.tip_string("Select a path");
    

    button.caption("Browse");

    // Layout: label | textbox | button
    layout.div(R"(
        <weight=20% label margin=5>
        <weight=60% textbox>
        <weight=20% button>
    )");
    layout.field("label") << label;
    layout.field("textbox") << textbox;
    layout.field("button") << button;
    layout.collocate();

    // Configure the button click event
    button.events().click([this] {
        browsePath();
    });
}

PathChooser::~PathChooser()
{

}

fs::path PathChooser::getPath() const
{
    return textbox.caption();
}

void PathChooser::setPath(const fs::path& newPath)
{
    textbox.caption(newPath.string());
}

void PathChooser::browsePath()
{
    if(isDirectory)
    {
        nana::folderbox fb;
        auto result = fb();
        if (!result.empty())
            textbox.caption(result.front());
    }
    else
    {
        nana::filebox fb(nullptr, true);
        auto result = fb.show();
        if (!result.empty())
            textbox.caption(result.front());
    }
}
