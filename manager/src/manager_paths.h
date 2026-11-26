#pragma once

#include <QString>

// Default Manager data folder beneath the current user's Documents directory.
QString managerDocumentsDirectory();
// System-configured Manager data folder, falling back to managerDocumentsDirectory().
QString managerDataDirectory();
// Manager-owned subfolder containing installed Weaver versions.
QString managerInstalledVersionsDirectory();
// Manager-owned subfolder containing application and installer logs.
QString managerLogsDirectory();
// Root of the Manager installation, accounting for an executable under bin/.
QString managerProgramInstallRoot();
// Create the Manager data, installed-version, and log folders.
// On failure, optionally writes a user-facing explanation to errorMessage.
bool initializeManagerDirectories(QString *errorMessage = nullptr);
