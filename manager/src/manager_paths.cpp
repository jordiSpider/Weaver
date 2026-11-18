#include "manager_paths.h"

#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>

namespace {

QString managerSettingsOrganization() {
	return QStringLiteral("Weaver");
}

QString managerSettingsApplication() {
	return QStringLiteral(WEAVER_MANAGER_NAME);
}

QString managerDataFolderName() {
	return QStringLiteral(WEAVER_MANAGER_DATA_DIRECTORY_NAME);
}

}

QString managerDocumentsDirectory() {
	const auto documentsDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
	if (documentsDirectory.isEmpty()) return {};
	return QDir(documentsDirectory).filePath(managerDataFolderName());
}

QString managerDataDirectory() {
	const auto configuredDirectory = QSettings(QSettings::NativeFormat, QSettings::SystemScope,
												   managerSettingsOrganization(),
												   managerSettingsApplication()).value(
		QStringLiteral("DataDirectory")).toString();
	if (!configuredDirectory.isEmpty()) return QDir::cleanPath(configuredDirectory);
	return managerDocumentsDirectory();
}

QString managerInstalledVersionsDirectory() {
	const auto dataDirectory = managerDataDirectory();
	if (dataDirectory.isEmpty()) return {};
	return QDir(dataDirectory).filePath("installed_versions");
}

QString managerLogsDirectory() {
	const auto dataDirectory = managerDataDirectory();
	if (dataDirectory.isEmpty()) return {};
	return QDir(dataDirectory).filePath("logs");
}

QString managerProgramInstallRoot() {
	QDir applicationDirectory(QCoreApplication::applicationDirPath());
	if (applicationDirectory.dirName().compare("bin", Qt::CaseInsensitive) == 0)
		applicationDirectory.cdUp();
	return QDir::cleanPath(applicationDirectory.absolutePath());
}

bool initializeManagerDirectories(QString *errorMessage) {
	const auto dataDirectory = managerDataDirectory();
	if (dataDirectory.isEmpty()
		|| !QDir().mkpath(dataDirectory)
		|| !QDir().mkpath(managerInstalledVersionsDirectory())
		|| !QDir().mkpath(managerLogsDirectory())) {
		if (errorMessage)
		*errorMessage = "Weaver Manager could not create its application data folders.";
		return false;
	}
	return true;
}
