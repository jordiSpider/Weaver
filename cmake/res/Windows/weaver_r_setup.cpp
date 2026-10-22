// WeaverRSetup.exe
//
// Silent helper executed by the MSI (deferred custom action, as SYSTEM). It
// finds R, installs missing or outdated dependencies in the installing user's
// standard R library, then installs weaverR there.
//
// It is a GUI-subsystem program and starts R without a console, so no window is
// ever shown. Logs are written outside the installation folder so they survive
// rollback, preferring ProgramData and falling back to the SYSTEM temp folder.
//
// Files expected next to this executable:
//   install_r_env.R, r_dependency_utils.R, cran_repo.txt, weaverR_*.tar.gz
// Exit codes: 0 ok | 2 R not found | 3 missing files | 4 process error | 5 log unavailable | other: exit code of R

#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include "weaver_r_setup_config.h"

#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace {

constexpr int kExitRNotFound     = 2;
constexpr int kExitMissingFiles  = 3;
constexpr int kExitCannotStartR  = 4;
constexpr int kExitLogUnavailable = 5;

HANDLE g_log = INVALID_HANDLE_VALUE;
std::wstring g_log_path;

std::string toUtf8(const std::wstring& text) {
    const int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()),
                                         nullptr, 0, nullptr, nullptr);
    if (size <= 0) return {};
    std::string utf8(static_cast<size_t>(size), '\0');
    if (WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()),
                            utf8.data(), size, nullptr, nullptr) != size) {
        return {};
    }
    return utf8;
}

void writeStandardOutput(const std::string& text) {
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (output == nullptr || output == INVALID_HANDLE_VALUE) return;

    DWORD offset = 0;
    while (offset < text.size()) {
        DWORD written = 0;
        if (!WriteFile(output, text.data() + offset,
                       static_cast<DWORD>(text.size() - offset), &written, nullptr) || written == 0) {
            return;
        }
        offset += written;
    }
}

void logLine(const std::wstring& text) {
    if (g_log == INVALID_HANDLE_VALUE) return;
    const std::string utf8 = toUtf8(text + L"\r\n");
    if (utf8.empty()) return;
    DWORD written = 0;
    WriteFile(g_log, utf8.data(), static_cast<DWORD>(utf8.size()), &written, nullptr);
}

bool ensureDirectory(const std::wstring& path, DWORD& error) {
    const int result = SHCreateDirectoryExW(nullptr, path.c_str(), nullptr);
    if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS && result != ERROR_FILE_EXISTS) {
        error = static_cast<DWORD>(result);
        return false;
    }

    const DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        error = GetLastError();
        return false;
    }
    if (!(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        error = ERROR_DIRECTORY;
        return false;
    }

    error = ERROR_SUCCESS;
    return true;
}

bool openLogAt(const std::wstring& path, DWORD& error) {
    SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};
    g_log = CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
                        &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (g_log == INVALID_HANDLE_VALUE) {
        error = GetLastError();
        return false;
    }

    g_log_path = path;
    error = ERROR_SUCCESS;
    return true;
}

void closeLog() {
    if (g_log == INVALID_HANDLE_VALUE) return;
    FlushFileBuffers(g_log);
    CloseHandle(g_log);
    g_log = INVALID_HANDLE_VALUE;

    HANDLE input = CreateFileW(g_log_path.c_str(), GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (input == INVALID_HANDLE_VALUE) {
        writeStandardOutput("ERROR: could not read the WeaverRSetup log for MSI output.\r\n");
        return;
    }

    char buffer[4096];
    DWORD read = 0;
    while (ReadFile(input, buffer, sizeof(buffer), &read, nullptr) && read > 0) {
        writeStandardOutput(std::string(buffer, read));
    }
    CloseHandle(input);
}

bool openLog() {
    DWORD primaryError = ERROR_SUCCESS;
    PWSTR programData = nullptr;
    const HRESULT result = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, nullptr, &programData);
    if (SUCCEEDED(result)) {
        std::wstring dir = std::wstring(programData) + L"\\" + WEAVER_INSTALLER_LOG_VARIANT + L"\\logs";
        CoTaskMemFree(programData);
        if (ensureDirectory(dir, primaryError) &&
            openLogAt(dir + L"\\install_r_env.log", primaryError)) {
            logLine(L"Log file: " + g_log_path);
            return true;
        }
    } else {
        primaryError = HRESULT_CODE(result);
    }

    std::vector<wchar_t> tempPath(32768);
    const DWORD tempLength = GetTempPathW(static_cast<DWORD>(tempPath.size()), tempPath.data());
    DWORD fallbackError = ERROR_PATH_NOT_FOUND;
    if (tempLength > 0 && tempLength < tempPath.size()) {
        std::wstring tempDir(tempPath.data(), tempLength);
        while (!tempDir.empty() && (tempDir.back() == L'\\' || tempDir.back() == L'/')) tempDir.pop_back();
        tempDir += L"\\" + std::wstring(WEAVER_INSTALLER_LOG_VARIANT) + L"\\logs";
        if (ensureDirectory(tempDir, fallbackError) &&
            openLogAt(tempDir + L"\\install_r_env.log", fallbackError)) {
            logLine(L"WARNING: ProgramData log path unavailable (Windows error " +
                    std::to_wstring(primaryError) + L"). Using fallback log location.");
            logLine(L"Log file: " + g_log_path);
            return true;
        }
    } else if (tempLength > 0) {
        fallbackError = ERROR_INSUFFICIENT_BUFFER;
    } else {
        fallbackError = GetLastError();
    }

    const std::wstring error = L"WeaverRSetup could not create a log file. ProgramData error " +
                               std::to_wstring(primaryError) + L", fallback error " +
                               std::to_wstring(fallbackError) + L".\r\n";
    OutputDebugStringW(error.c_str());
    writeStandardOutput(toUtf8(error));
    return false;
}

std::wstring exeDirectory() {
    wchar_t buf[MAX_PATH * 4];
    const DWORD n = GetModuleFileNameW(nullptr, buf, static_cast<DWORD>(sizeof(buf) / sizeof(buf[0])));
    std::wstring path(buf, n);
    return path.substr(0, path.find_last_of(L"\\/"));
}

bool fileExists(const std::wstring& path) {
    const DWORD attrs = GetFileAttributesW(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

// R for Windows records its install directory under R-core. Check both registry
// views because the installer may have written the key from a 32-bit process.
std::wstring registryInstallPath(HKEY root, REGSAM view) {
    HKEY key = nullptr;
    if (RegOpenKeyExW(root, L"SOFTWARE\\R-core\\R", 0, KEY_READ | view, &key) != ERROR_SUCCESS) {
        return L"";
    }
    wchar_t buf[MAX_PATH * 2];
    DWORD size = sizeof(buf);
    DWORD type = 0;
    const LONG rc = RegQueryValueExW(key, L"InstallPath", nullptr, &type,
                                     reinterpret_cast<LPBYTE>(buf), &size);
    RegCloseKey(key);
    if (rc != ERROR_SUCCESS || type != REG_SZ) return L"";
    size_t len = size / sizeof(wchar_t);
    while (len > 0 && buf[len - 1] == L'\0') --len;
    std::wstring path(buf, len);
    while (!path.empty() && (path.back() == L'\\' || path.back() == L'/')) path.pop_back();
    return path;
}

void addRscriptCandidates(const std::wstring& base, std::vector<std::wstring>& candidates) {
    if (base.empty()) return;
    std::wstring normalized = base;
    while (!normalized.empty() && (normalized.back() == L'\\' || normalized.back() == L'/')) normalized.pop_back();
    candidates.push_back(normalized + L"\\bin\\x64\\Rscript.exe");
    candidates.push_back(normalized + L"\\bin\\Rscript.exe");
}

void addStandardFolderCandidates(const wchar_t* folder, std::vector<std::wstring>& candidates) {
    if (!folder || !*folder) return;
    const std::wstring pattern = std::wstring(folder) + L"\\R\\R-*";
    WIN32_FIND_DATAW data{};
    HANDLE h = FindFirstFileW(pattern.c_str(), &data);
    if (h == INVALID_HANDLE_VALUE) return;
    std::vector<std::wstring> versions;
    do {
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            versions.push_back(std::wstring(folder) + L"\\R\\" + data.cFileName);
        }
    } while (FindNextFileW(h, &data));
    FindClose(h);
    // Newest R versions are typically named R-x.y.z; lexicographic ordering
    // works for current two-digit major/minor version numbers.
    std::sort(versions.begin(), versions.end(), std::greater<std::wstring>());
    for (const auto& version : versions) addRscriptCandidates(version, candidates);
}

std::wstring findRscript() {
    std::vector<std::wstring> candidates;
    // Registry locations first: these are the authoritative locations for
    // standard R installations, including installations outside Program Files.
    for (HKEY root : {HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER}) {
        for (REGSAM view : {KEY_WOW64_64KEY, KEY_WOW64_32KEY})
            addRscriptCandidates(registryInstallPath(root, view), candidates);
    }

    // Also cover normal CRAN installs when registry discovery is unavailable
    // (for example, an elevated MSI running as SYSTEM).
    wchar_t programFiles[MAX_PATH * 4]{};
    if (GetEnvironmentVariableW(L"ProgramW6432", programFiles, static_cast<DWORD>(sizeof(programFiles) / sizeof(programFiles[0]))) > 0)
        addStandardFolderCandidates(programFiles, candidates);
    if (GetEnvironmentVariableW(L"ProgramFiles", programFiles, static_cast<DWORD>(sizeof(programFiles) / sizeof(programFiles[0]))) > 0)
        addStandardFolderCandidates(programFiles, candidates);
    if (GetEnvironmentVariableW(L"ProgramFiles(x86)", programFiles, static_cast<DWORD>(sizeof(programFiles) / sizeof(programFiles[0]))) > 0)
        addStandardFolderCandidates(programFiles, candidates);

    for (const auto& candidate : candidates) {
        if (fileExists(candidate)) return candidate;
    }

    // Finally honor Rscript available on the effective machine PATH.
    wchar_t found[MAX_PATH * 4]{};
    if (SearchPathW(nullptr, L"Rscript.exe", nullptr, static_cast<DWORD>(sizeof(found) / sizeof(found[0])), found, nullptr) > 0)
        return found;
    return L"";
}

std::wstring findTarball(const std::wstring& dir) {
    WIN32_FIND_DATAW data;
    HANDLE h = FindFirstFileW((dir + L"\\weaverR_*.tar.gz").c_str(), &data);
    if (h == INVALID_HANDLE_VALUE) return L"";
    std::wstring best = data.cFileName;
    while (FindNextFileW(h, &data)) {
        if (best < data.cFileName) best = data.cFileName;
    }
    FindClose(h);
    return dir + L"\\" + best;
}

std::wstring quoted(const std::wstring& s) { return L"\"" + s + L"\""; }

}  // namespace

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    if (!openLog()) return kExitLogUnavailable;
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    const std::wstring dir     = exeDirectory();
    const std::wstring script  = dir + L"\\install_r_env.R";
    const std::wstring repo    = dir + L"\\cran_repo.txt";
    const std::wstring tarball = findTarball(dir);

    int argumentCount = 0;
    LPWSTR* arguments = CommandLineToArgvW(GetCommandLineW(), &argumentCount);
    const std::wstring userDocuments = arguments != nullptr && argumentCount > 1 ? arguments[1] : L"";
    if (arguments != nullptr) LocalFree(arguments);
    if (userDocuments.empty()) {
        logLine(L"ERROR: the installing user's documents folder was not provided.");
        closeLog();
        return kExitMissingFiles;
    }

    logLine(L"WeaverRSetup: installation folder " + dir);
    logLine(L"R script: " + script);
    logLine(L"CRAN repository file: " + repo);

    bool missingFiles = false;
    if (tarball.empty()) {
        logLine(L"ERROR: no weaverR_*.tar.gz package tarball was found in " + dir);
        missingFiles = true;
    } else {
        logLine(L"Package tarball: " + tarball);
    }
    if (!fileExists(script)) {
        logLine(L"ERROR: installer script is missing: " + script);
        missingFiles = true;
    }
    if (!fileExists(repo)) {
        logLine(L"ERROR: CRAN repository file is missing: " + repo);
        missingFiles = true;
    }
    const std::wstring dependencyHelper = dir + L"\\r_dependency_utils.R";
    if (!fileExists(dependencyHelper)) {
        logLine(L"ERROR: dependency helper is missing: " + dependencyHelper);
        missingFiles = true;
    }
    if (missingFiles) {
        closeLog();
        return kExitMissingFiles;
    }

    const std::wstring rscript = findRscript();
    if (rscript.empty()) {
        logLine(L"ERROR: Rscript.exe was not found. Checked R-core registry keys in both 32-bit and 64-bit views, standard Program Files\\R\\R-* folders, and the effective PATH.");
        logLine(L"Install R from https://cran.r-project.org, or add its bin folder to the system PATH, then run the installer again.");
        closeLog();
        return kExitRNotFound;
    }
    logLine(L"Using " + rscript);

    // Keep R environment and site-library configuration, but skip the user's
    // R profile, which could preload package namespaces during installation.
    std::wstring command = quoted(rscript) + L" --no-save --no-restore --no-init-file " +
                           quoted(script) + L" user " + quoted(tarball) + L" " + quoted(repo);

    // Handles passed to the child process must be inheritable
    SECURITY_ATTRIBUTES sa{sizeof(sa), nullptr, TRUE};
    HANDLE nul = CreateFileW(L"NUL", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                             &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (nul == INVALID_HANDLE_VALUE) {
        const DWORD error = GetLastError();
        logLine(L"ERROR: could not open NUL for Rscript input (Windows error " + std::to_wstring(error) + L").");
        closeLog();
        return kExitCannotStartR;
    }

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = nul;
    si.hStdOutput = g_log;
    si.hStdError = g_log;

    PROCESS_INFORMATION pi{};
    if (!SetEnvironmentVariableW(L"R_USER", userDocuments.c_str()) ||
        !SetEnvironmentVariableW(L"HOME", userDocuments.c_str())) {
        const DWORD error = GetLastError();
        logLine(L"ERROR: could not set the installing user's R home (Windows error " + std::to_wstring(error) + L").");
        CloseHandle(nul);
        closeLog();
        return kExitCannotStartR;
    }

    // R is started without a console window (CREATE_NO_WINDOW)
    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW,
                        nullptr, dir.c_str(), &si, &pi)) {
        const DWORD error = GetLastError();
        logLine(L"ERROR: could not start Rscript (Windows error " + std::to_wstring(error) + L").");
        CloseHandle(nul);
        closeLog();
        return kExitCannotStartR;
    }

    const DWORD waitResult = WaitForSingleObject(pi.hProcess, INFINITE);
    if (waitResult != WAIT_OBJECT_0) {
        const DWORD error = waitResult == WAIT_FAILED ? GetLastError() : ERROR_GEN_FAILURE;
        logLine(L"ERROR: waiting for Rscript failed (Windows error " + std::to_wstring(error) + L").");
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        CloseHandle(nul);
        closeLog();
        return kExitCannotStartR;
    }

    DWORD exitCode = 1;
    if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
        const DWORD error = GetLastError();
        logLine(L"ERROR: could not read the Rscript exit code (Windows error " + std::to_wstring(error) + L").");
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        CloseHandle(nul);
        closeLog();
        return kExitCannotStartR;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    if (nul != INVALID_HANDLE_VALUE) CloseHandle(nul);

    if (exitCode != 0) {
        logLine(L"ERROR: creating the R environment failed (exit code " + std::to_wstring(exitCode) + L").");
    } else {
        logLine(L"R environment ready.");
    }
    closeLog();
    return static_cast<int>(exitCode);
}
