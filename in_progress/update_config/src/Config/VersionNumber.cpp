
#include "Tools/Config/VersionNumber.h"


using namespace std;


VersionNumber::VersionNumber(string version_number_str)
{
    regex pattern("(\\d+)\\.(\\d+)\\.(\\d+)");

    smatch matches;

    if(regex_search(version_number_str, matches, pattern))
    {
        major = stoi(matches[1].str());
        minor = stoi(matches[2].str());
        patch = stoi(matches[3].str());
    } 
    else
    {
        major = 0;
        minor = 0;
        patch = 0;
    }
}

VersionNumber::~VersionNumber()
{

}

std::string VersionNumber::toString()
{
    std::ostringstream ss;

    ss << major;
    if(minor >= 0)
    {
        ss << ".";
        if(minor < 10)
        {
            ss << "0" << minor;
        }
        else
        {
            ss << minor;
        }

        if(patch >= 0)
        {
            ss << ".";
            if(patch < 10)
            {
                ss << "0" << patch;
            }
            else
            {
                ss << patch;
            }
        }
    }

    return ss.str();
}

bool VersionNumber::operator>=(const VersionNumber& other) const
{
    if(this->major > other.major)
    {
        return true;
    }
    else if(this->major == other.major)
    {
        if(this->minor > other.minor)
        {
            return true;
        }
        else if(this->minor == other.minor)
        {
            if(this->patch >= other.patch)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool VersionNumber::operator>(const VersionNumber& other) const
{
    if(this->major > other.major)
    {
        return true;
    }
    else if(this->major == other.major)
    {
        if(this->minor > other.minor)
        {
            return true;
        }
        else if(this->minor == other.minor)
        {
            if(this->patch > other.patch)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool VersionNumber::operator<=(const VersionNumber& other) const
{
    if(this->major < other.major)
    {
        return true;
    }
    else if(this->major == other.major)
    {
        if(this->minor < other.minor)
        {
            return true;
        }
        else if(this->minor == other.minor)
        {
            if(this->patch <= other.patch)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool VersionNumber::operator<(const VersionNumber& other) const
{
    if(this->major < other.major)
    {
        return true;
    }
    else if(this->major == other.major)
    {
        if(this->minor < other.minor)
        {
            return true;
        }
        else if(this->minor == other.minor)
        {
            if(this->patch < other.patch)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool VersionNumber::operator==(const VersionNumber& other) const
{
    if(this->major == other.major)
    {
        if(this->minor == other.minor)
        {
            if(this->patch == other.patch)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool VersionNumber::operator!=(const VersionNumber& other) const
{
    return !(*this == other);
}
