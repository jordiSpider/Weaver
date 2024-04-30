#include "Instar.h"

using namespace std;

Instar::Instar()
{
    this->value = 0;
}

Instar::Instar(unsigned int value)
{
    this->value = value - 1;
}

Instar operator+(const Instar& lhs, const int& rhs)
{
    return Instar(lhs.value + (unsigned int)rhs);
}

Instar operator+(const int& lhs, const Instar& rhs)
{
    return Instar((unsigned int)lhs + rhs.value);
}

bool operator<(const Instar& lhs, const Instar& rhs)
{
    return lhs.value < rhs.value;
}

bool operator<(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) < (unsigned int)rhs;
}

bool operator<(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs < (rhs.value+1);
}

bool operator<=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value <= rhs.value;
}

bool operator<=(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) <= (unsigned int)rhs;
}

bool operator<=(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs <= (rhs.value+1);
}

bool operator>(const Instar& lhs, const Instar& rhs)
{
    return lhs.value > rhs.value;
}

bool operator>(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) > (unsigned int)rhs;
}

bool operator>(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs > (rhs.value+1);
}

bool operator>=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value >= rhs.value;
}

bool operator>=(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) >= (unsigned int)rhs;
}

bool operator>=(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs >= (rhs.value+1);
}

bool operator==(const Instar& lhs, const Instar& rhs)
{
    return lhs.value == rhs.value;
}

bool operator==(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) == (unsigned int)rhs;
}

bool operator==(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs == (rhs.value+1);
}

bool operator!=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value != rhs.value;
}

bool operator!=(const Instar& lhs, const int& rhs)
{
    return (lhs.value+1) != (unsigned int)rhs;
}

bool operator!=(const int& lhs, const Instar& rhs)
{
    return (unsigned int)lhs != (rhs.value+1);
}

ostream& operator<<(ostream& os, Instar const& instar)
{
    return os << (instar.value + 1);
}

/*
// Definición del método para formatear el tipo de dato Instar
template <typename FmtContext>
auto fmt::formatter<Instar>::format(Instar const& instar, FmtContext& ctx) const {
    return fmt::format_to(ctx.out(), "{}", instar.getValue() + 1);
}
*/