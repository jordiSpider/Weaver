#ifndef OUTPUT_H
#define OUTPUT_H

#include "LineInfoException.h"
#include <magic_enum.hpp>
#include "Types.h"
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <cstdio>
#include <string>


class OutputStream {
public:
    enum OutputStreamType {
        Terminal,
        Log
    };

    inline static const OutputStreamType stringToEnumValue(const std::string &str) { 
        try
		{
			return stringToEnum.at(str);
		}
		catch(const std::out_of_range& e) 
		{
			throwLineInfoException(fmt::format("Unknown output stream type '{}'. Valid values are {}", str, printAvailableValues()));
		}
    }

private:
    static const std::unordered_map<std::string_view, const OutputStreamType> stringToEnum;
    static const std::string enumValues;

    inline static std::string_view to_string(const OutputStreamType& outputStream) { return magic_enum::enum_name(outputStream); }
	static const std::unordered_map<std::string_view, const OutputStreamType> generateMap();
    inline static constexpr size_t size() { return magic_enum::enum_count<OutputStreamType>(); }
    inline static std::string_view printAvailableValues() { return enumValues; }
    static const std::string generateAvailableValues();
};

class Output {
private:
    static std::FILE* output;
    static std::FILE* errorOutput;

public:
    static void setOutputStream(std::FILE* newOutput);
    static void setOutputStream(const fs::path& outputFolder, const std::string& outputStreamType);
    static void setErrorOutputStream(std::FILE* newErrorOutput);
    template <typename... Args>
    static void cout(const std::string& message, const Args&... args)
    {
        fmt::print(output, message, args...);
    }
    template <typename... Args>
    static void coutFlush(const std::string& message, const Args&... args)
    {
        fmt::print(output, message, args...);
        fflush(output);
    }
    template <typename... Args>
    static void cerr(const std::string& message, const Args&... args)
    {
        fmt::print(errorOutput, message, args...);
    }
};

#endif  // OUTPUT_H