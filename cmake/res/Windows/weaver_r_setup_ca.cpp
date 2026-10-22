#include <windows.h>
#include <msi.h>
#include <msiquery.h>

#include <cwctype>
#include <string>
#include <vector>

namespace {

std::wstring getMsiProperty(MSIHANDLE install, const wchar_t* name) {
	DWORD length = 0;
	wchar_t empty[1]{};
	const UINT status = MsiGetPropertyW(install, name, empty, &length);
	if (status != ERROR_MORE_DATA && status != ERROR_SUCCESS) return {};
	std::vector<wchar_t> value(length + 1);
	DWORD bufferLength = static_cast<DWORD>(value.size());
	if (MsiGetPropertyW(install, name, value.data(), &bufferLength) != ERROR_SUCCESS) return {};
	return std::wstring(value.data(), bufferLength);
}

std::wstring readFile(HANDLE file) {
	LARGE_INTEGER zero{};
	if (!SetFilePointerEx(file, zero, nullptr, FILE_BEGIN)) return {};

	std::wstring text;
	char buffer[4096];
	DWORD bytesRead = 0;
	while (ReadFile(file, buffer, sizeof(buffer), &bytesRead, nullptr) && bytesRead > 0) {
		const int chars = MultiByteToWideChar(CP_UTF8, 0, buffer, static_cast<int>(bytesRead), nullptr, 0);
		if (chars <= 0) continue;
		const size_t oldSize = text.size();
		text.resize(oldSize + static_cast<size_t>(chars));
		MultiByteToWideChar(CP_UTF8, 0, buffer, static_cast<int>(bytesRead), text.data() + oldSize, chars);
	}
	return text;
}

std::wstring firstError(const std::wstring& output) {
	const size_t marker = output.find(L"ERROR:");
	if (marker == std::wstring::npos) return {};
	size_t end = output.find_first_of(L"\r\n", marker);
	if (end == std::wstring::npos) end = output.size();
	std::wstring message = output.substr(marker + 6, end - marker - 6);
	while (!message.empty() && iswspace(message.front())) message.erase(message.begin());

	size_t context = end;
	while (context < output.size() && message.size() < 700) {
		while (context < output.size() && (output[context] == L'\r' || output[context] == L'\n')) ++context;
		if (context == output.size() || output.compare(context, 7, L"ERROR:") == 0) break;
		size_t contextEnd = output.find_first_of(L"\r\n", context);
		if (contextEnd == std::wstring::npos) contextEnd = output.size();
		if (contextEnd > context) message += L"\r\n" + output.substr(context, contextEnd - context);
		context = contextEnd;
	}
	if (message.size() > 700) message.resize(700);
	return message;
}

std::wstring findLogPath(const std::wstring& output) {
	const size_t marker = output.find(L"Log file: ");
	if (marker == std::wstring::npos) return {};
	const size_t start = marker + 10;
	size_t end = output.find_first_of(L"\r\n", start);
	if (end == std::wstring::npos) end = output.size();
	return output.substr(start, end - start);
}

std::wstring writeDiagnosticLog(const std::wstring& details, const std::wstring& output) {
	wchar_t tempDirectory[MAX_PATH + 1]{};
	const DWORD tempLength = GetTempPathW(MAX_PATH, tempDirectory);
	wchar_t tempFile[MAX_PATH + 1]{};
	if (tempLength == 0 || tempLength > MAX_PATH || GetTempFileNameW(tempDirectory, L"wrs", 0, tempFile) == 0) return {};

	HANDLE file = CreateFileW(tempFile, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL, nullptr);
	if (file == INVALID_HANDLE_VALUE) {
		DeleteFileW(tempFile);
		return {};
	}

	std::wstring content = L"Weaver R package setup failed.\r\n" + details + L"\r\n";
	if (!output.empty()) content += L"\r\nHelper output:\r\n" + output;
	const int byteCount = WideCharToMultiByte(CP_UTF8, 0, content.data(), static_cast<int>(content.size()), nullptr, 0, nullptr, nullptr);
	if (byteCount > 0) {
		std::string utf8(static_cast<size_t>(byteCount), '\0');
		if (WideCharToMultiByte(CP_UTF8, 0, content.data(), static_cast<int>(content.size()), utf8.data(), byteCount, nullptr, nullptr) == byteCount) {
			DWORD written = 0;
			WriteFile(file, utf8.data(), static_cast<DWORD>(utf8.size()), &written, nullptr);
			FlushFileBuffers(file);
		}
	}
	CloseHandle(file);
	return tempFile;
}

void logText(MSIHANDLE install, const std::wstring& text) {
	MSIHANDLE record = MsiCreateRecord(1);
	if (!record) return;
	MsiRecordSetStringW(record, 0, text.c_str());
	MsiProcessMessage(install, INSTALLMESSAGE_INFO, record);
	MsiCloseHandle(record);
}

void displayFailure(MSIHANDLE install, const std::wstring& details, const std::wstring& logPath);

UINT fail(MSIHANDLE install, const std::wstring& details, const std::wstring& output = {}) {
	const std::wstring logPath = writeDiagnosticLog(details, output);
	displayFailure(install, details, logPath);
	return ERROR_INSTALL_FAILURE;
}

void displayFailure(MSIHANDLE install, const std::wstring& details, const std::wstring& logPath) {
	MSIHANDLE record = MsiCreateRecord(3);
	if (!record) return;
	MsiRecordSetInteger(record, 1, 25001);
	MsiRecordSetStringW(record, 2, details.c_str());
	MsiRecordSetStringW(record, 3, logPath.empty() ? L"See the Windows Installer verbose log for details." : logPath.c_str());
	MsiProcessMessage(install, INSTALLMESSAGE_ERROR, record);
	MsiCloseHandle(record);
}

} // namespace

extern "C" __declspec(dllexport) UINT __stdcall InstallREnvironment(MSIHANDLE install) {
	const std::wstring customActionData = getMsiProperty(install, L"CustomActionData");
	const size_t separator = customActionData.find(L'|');
	if (separator == std::wstring::npos) {
		return fail(install, L"The R setup helper path or user documents folder was not provided.");
	}
	std::wstring executable = customActionData.substr(0, separator);
	const std::wstring userDocuments = customActionData.substr(separator + 1);
	if (executable.size() >= 2 && executable.front() == L'"' && executable.back() == L'"') {
		executable = executable.substr(1, executable.size() - 2);
	}
	if (executable.empty() || userDocuments.empty()) {
		return fail(install, L"The R setup helper path or user documents folder was empty.");
	}

	wchar_t tempDirectory[MAX_PATH + 1]{};
	const DWORD tempLength = GetTempPathW(MAX_PATH, tempDirectory);
	wchar_t tempFile[MAX_PATH + 1]{};
	if (tempLength == 0 || tempLength > MAX_PATH || GetTempFileNameW(tempDirectory, L"wrs", 0, tempFile) == 0) {
		return fail(install, L"A temporary file could not be created to capture the R setup output.");
	}

	SECURITY_ATTRIBUTES security{sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE};
	HANDLE outputFile = CreateFileW(tempFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
									&security, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, nullptr);
	if (outputFile == INVALID_HANDLE_VALUE) {
		DeleteFileW(tempFile);
		return fail(install, L"A temporary file could not be opened to capture the R setup output.");
	}

	STARTUPINFOW startup{};
	startup.cb = sizeof(startup);
	startup.dwFlags = STARTF_USESTDHANDLES;
	startup.hStdOutput = outputFile;
	startup.hStdError = outputFile;
	startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	PROCESS_INFORMATION process{};
	std::wstring command = L"\"" + executable + L"\" \"" + userDocuments + L"\"";
	const BOOL started = CreateProcessW(executable.c_str(), command.data(), nullptr, nullptr, TRUE,
										CREATE_NO_WINDOW, nullptr, nullptr, &startup, &process);
	if (!started) {
		const DWORD error = GetLastError();
		CloseHandle(outputFile);
		DeleteFileW(tempFile);
		return fail(install, L"The R setup helper could not be started (Windows error " + std::to_wstring(error) + L").");
	}

	CloseHandle(process.hThread);
	WaitForSingleObject(process.hProcess, INFINITE);
	DWORD exitCode = ERROR_INSTALL_FAILURE;
	GetExitCodeProcess(process.hProcess, &exitCode);
	CloseHandle(process.hProcess);

	const std::wstring output = readFile(outputFile);
	CloseHandle(outputFile);
	DeleteFileW(tempFile);

	if (exitCode == 0) return ERROR_SUCCESS;

	size_t lineStart = 0;
	while (lineStart < output.size()) {
		size_t lineEnd = output.find_first_of(L"\r\n", lineStart);
		if (lineEnd == std::wstring::npos) lineEnd = output.size();
		if (lineEnd > lineStart) logText(install, output.substr(lineStart, lineEnd - lineStart));
		lineStart = lineEnd + 1;
	}

	std::wstring details = firstError(output);
	if (details.empty()) details = L"The R setup helper exited with code " + std::to_wstring(exitCode) + L".";
	const std::wstring helperLog = findLogPath(output);
	const std::wstring logPath = helperLog.empty() ? writeDiagnosticLog(details, output) : helperLog;
	displayFailure(install, details, logPath);
	return ERROR_INSTALL_FAILURE;
}