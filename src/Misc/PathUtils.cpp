
#include "Misc/PathUtils.h"

#include <stdexcept>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif


using namespace std;
namespace fs = std::filesystem;



fs::path getDefaultOutputFolderPath() {
	#ifdef _WIN32
		char* buffer = nullptr;
		size_t len = 0;
		if (_dupenv_s(&buffer, &len, "USERPROFILE") == 0 && buffer != nullptr) {
			fs::path home(buffer);
			free(buffer);
			return home / "Documents" / PROJECT_NAME / "output";
		}
		throw std::runtime_error("USERPROFILE environment variable not found.");
	#else
		return fs::path(std::getenv("HOME")) / PROJECT_NAME / "output";
	#endif
}

#ifdef _WIN32
string getEnvSafe(const char* varName) {
	char* buffer = nullptr;
	size_t size = 0;
	if (_dupenv_s(&buffer, &size, varName) == 0 && buffer != nullptr) {
		string result(buffer);
		free(buffer);  // liberar memoria asignada por _dupenv_s
		return result;
	}
	return "";
}
#endif
