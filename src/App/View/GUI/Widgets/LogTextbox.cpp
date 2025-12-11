
#include "App/View/GUI/Widgets/LogTextbox.h"


using namespace std;
namespace fs = std::filesystem;



LogTextbox::LogTextbox(nana::window parent)
    : nana::panel<true>(parent)
{
    log.editable(false);           // Make the textbox read-only
    log.line_wrapped(true);        // Enable line wrapping
    log.multi_lines(true);         // Enable multiple lines
    log.typeface(nana::paint::font{"Consolas", 14}); // Set monospaced font

    // Layout with single field "log"
    layout.div("<log>");
    layout.field("log") << log;
    layout.collocate();
}

LogTextbox::~LogTextbox()
{

}

void LogTextbox::append(const std::string& text)
{
    log.append(text, false);
}

void LogTextbox::clean()
{
    log.select(true); // Select all text
    log.del();        // Delete selected text
}
