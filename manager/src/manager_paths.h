#pragma once

#include <QString>

QString managerDocumentsDirectory();
QString managerDataDirectory();
QString managerInstalledVersionsDirectory();
QString managerLogsDirectory();
QString managerProgramInstallRoot();
bool initializeManagerDirectories(QString *errorMessage = nullptr);
