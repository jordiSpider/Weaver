
#include "App/View/GUI/Widgets/Spinbox.h"


using namespace std;
namespace fs = std::filesystem;



Spinbox::Spinbox(nana::window parent, const std::string& labelText, unsigned int maxValue, unsigned int stepValue)
    : nana::panel<true>(parent), maxValue(maxValue), stepValue(stepValue)
{
    label.caption(labelText);
    label.text_align(nana::align::right, nana::align_v::center);

    // Configurar el Spinbox
    spinbox.range(1, maxValue, stepValue);
    spinbox.value(std::to_string(maxValue)); // Por defecto el máximo
    spinbox.editable(false); // Evita que escriban valores inválidos

    layout.div(R"(
            <weight=20% label margin=5>
            <weight=80% spinbox>
        )");
    layout.field("label") << label;
    layout.field("spinbox") << spinbox;
    layout.collocate();
}

Spinbox::~Spinbox()
{

}

unsigned int Spinbox::value() const {
    return static_cast<unsigned int>(std::stoul(spinbox.value()));
}
