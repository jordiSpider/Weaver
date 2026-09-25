
#ifndef SPINBOX_H_
#define SPINBOX_H_



#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/spinbox.hpp>
#include <nana/gui/place.hpp>
#include <string>



class Spinbox : public nana::panel<true> {
public:
    Spinbox(nana::window parent, const std::string& labelText, unsigned int maxValue, unsigned int stepValue);

    virtual ~Spinbox();

    // Disable copy constructor and assignment
    Spinbox(const Spinbox& other) = delete;
    Spinbox& operator=(const Spinbox& other) = delete;

    unsigned int value() const;

protected:
    nana::place layout{ *this };
    nana::label label{ *this };
    nana::spinbox spinbox{ *this };

	unsigned int maxValue;  /**< Maximum value for the spinbox */
	unsigned int stepValue; /**< Step value for the spinbox */
};

#endif // SPINBOX_H_