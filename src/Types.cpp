
#include "Types.h"


using namespace std;

Instar::Instar()
{
    this->value = 0;
}

Instar::Instar(unsigned int value)
{
    this->value = value - 1;
}

bool operator<(const Instar& lhs, const Instar& rhs)
{
    return lhs.value < rhs.value;
}

bool operator<(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) < rhs;
}

bool operator<(const unsigned int& lhs, const Instar& rhs)
{
    return lhs < (rhs.value+1);
}

bool operator<=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value <= rhs.value;
}

bool operator<=(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) <= rhs;
}

bool operator<=(const unsigned int& lhs, const Instar& rhs)
{
    return lhs <= (rhs.value+1);
}

bool operator>(const Instar& lhs, const Instar& rhs)
{
    return lhs.value > rhs.value;
}

bool operator>(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) > rhs;
}

bool operator>(const unsigned int& lhs, const Instar& rhs)
{
    return lhs > (rhs.value+1);
}

bool operator>=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value >= rhs.value;
}

bool operator>=(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) >= rhs;
}

bool operator>=(const unsigned int& lhs, const Instar& rhs)
{
    return lhs >= (rhs.value+1);
}

bool operator==(const Instar& lhs, const Instar& rhs)
{
    return lhs.value == rhs.value;
}

bool operator==(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) == rhs;
}

bool operator==(const unsigned int& lhs, const Instar& rhs)
{
    return lhs == (rhs.value+1);
}

bool operator!=(const Instar& lhs, const Instar& rhs)
{
    return lhs.value != rhs.value;
}

bool operator!=(const Instar& lhs, const unsigned int& rhs)
{
    return (lhs.value+1) != rhs;
}

bool operator!=(const unsigned int& lhs, const Instar& rhs)
{
    return lhs != (rhs.value+1);
}

ostream& operator<<(ostream& os, const Instar& instar)
{
    os << (instar.value + 1);
    return os;
}
