
#include "App/View/GUI/Widgets/Checkbox.h"


using namespace std;
namespace fs = std::filesystem;




Checkbox::Checkbox(nana::window parent, const string& labelText, const bool defaultValue)
    : nana::panel<true>(parent)
{
    label.caption(labelText);
    label.text_align(nana::align::right, nana::align_v::center);

    checkbox.check(defaultValue);

    layout.div(R"(
        <weight=20% label margin=5>
        <weight=80% vert <><checkbox><>>
    )");
    layout.field("label") << label;
    layout.field("checkbox") << checkbox;
    layout.collocate();
}


Checkbox::~Checkbox()
{

}

bool Checkbox::checked() const
{
    return checkbox.checked();
}
