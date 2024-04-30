#include "Output.h"
#include "GlobalVariable.h"


using namespace std;

const unordered_map<string_view, const OutputStream::OutputStreamType> OutputStream::generateMap() 
{
	unordered_map<string_view, const OutputStream::OutputStreamType> enumMap;

	for(size_t i = 0; i < size(); i++) {
		const OutputStream::OutputStreamType outputStreamType = static_cast<const OutputStream::OutputStreamType>(i);
		enumMap.insert({to_string(outputStreamType), outputStreamType});
	}

	return enumMap;
}

const unordered_map<string_view, const OutputStream::OutputStreamType> OutputStream::stringToEnum = OutputStream::generateMap();

const string OutputStream::generateAvailableValues()
{
	constexpr auto typeNames = magic_enum::enum_names<OutputStreamType>();

	auto values = fmt::format("{}", typeNames[0]);
	for(size_t i = 1; i < typeNames.size(); i++) {
		values += fmt::format(", {}", typeNames[i]);
	}

	return values;
}

const string OutputStream::enumValues = OutputStream::generateAvailableValues();


std::FILE* Output::output = stdout;
std::FILE* Output::errorOutput = stderr;

void Output::setOutputStream(std::FILE* newOutput)
{
    output = newOutput;
}

void Output::setOutputStream(const fs::path& outputFolder, const std::string& outputStreamType)
{
    switch (OutputStream::stringToEnumValue(outputStreamType))
	{
		case OutputStream::Log: {
			FILE* file = std::fopen((outputFolder / fs::path(string(LOG_FILENAME) + "." + string(LOG_EXTENSION))).string().c_str(), "w");
			setOutputStream(file);
			break;
		}
		case OutputStream::Terminal: {
            setOutputStream(stdout);
			break;
		}
		default: {
			throwLineInfoException("Default case");
			break;
		}
	}
}

void Output::setErrorOutputStream(std::FILE* newErrorOutput)
{
    errorOutput = newErrorOutput;
}
