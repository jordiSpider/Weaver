
#include "App/View/GUI/Widgets/OptionSelector.h"


using namespace std;
namespace fs = std::filesystem;



OptionSelector::OptionSelector(nana::window parent, const string& labelText, const vector<string>& options)
    : nana::panel<true>(parent)
{
    label.caption(labelText);
    label.text_align(nana::align::right, nana::align_v::center);

    for(const string& option : options)
    {
        optionsDropdown.push_back(option);
    }

    // Layout: label on left, dropdown on right
    layout.div(R"(
        <weight=20% label margin=5>
        <weight=80% options>
    )");
    layout.field("label") << label;
    layout.field("options") << optionsDropdown;
    layout.collocate();
}

OptionSelector::~OptionSelector()
{

}

void OptionSelector::option(const size_t pos)
{
    optionsDropdown.option(pos);
}

size_t OptionSelector::option() const
{
    return optionsDropdown.option();
}
