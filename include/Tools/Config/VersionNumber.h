#ifndef VERSION_NUMBER_H_
#define VERSION_NUMBER_H_

#include <string>
#include <sstream>
#include <regex>
#include <vector>


class VersionNumber
{
private:
    int major;
    int minor;
    int patch;

public:
    VersionNumber(std::string version_number_str = "");
    virtual ~VersionNumber();

    std::string toString();

    bool operator>=(const VersionNumber& other) const;
    bool operator>(const VersionNumber& other) const;
    bool operator<=(const VersionNumber& other) const;
    bool operator<(const VersionNumber& other) const;
    bool operator==(const VersionNumber& other) const;
    bool operator!=(const VersionNumber& other) const;
};



#endif /* VERSION_NUMBER_H_ */