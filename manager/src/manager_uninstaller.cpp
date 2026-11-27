#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QUuid>

namespace {

// Prefer the system copy of Windows Installer, with PATH lookup as a fallback.
QString findMsiexec() {
	const auto windowsDir = qEnvironmentVariable("WINDIR");
	const auto systemMsiexec = QDir(windowsDir).filePath("System32/msiexec.exe");
	return QFileInfo::exists(systemMsiexec) ? systemMsiexec : QStandardPaths::findExecutable("msiexec.exe");
}

bool removeDirectoryIfEmpty(const QString &path, QString *errorMessage) {
	if (path.isEmpty()) {
		*errorMessage = "The Manager data directory could not be determined.";
		return false;
	}
	QDir directory(path);
	if (!directory.exists()) return true;
	if (!directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System).isEmpty())
		return true;
	if (directory.rmdir(".")) return true;
	*errorMessage = QString("Could not remove empty directory '%1'.").arg(path);
	return false;
}

bool removeDirectory(const QString &path, QString *errorMessage) {
	if (path.isEmpty()) {
		*errorMessage = "The Manager data directory could not be determined.";
		return false;
	}
	if (!QDir(path).exists()) return true;
	if (QDir(path).removeRecursively()) return true;
	*errorMessage = QString("Could not remove directory '%1'.").arg(path);
	return false;
}

// Resolve the same system-configured data root used by the Manager application.
QString managerDataDirectory() {
	const auto configuredDirectory = QSettings(QSettings::NativeFormat, QSettings::SystemScope,
												   QStringLiteral("Weaver"),
												   QStringLiteral(WEAVER_MANAGER_NAME)).value(
		QStringLiteral("DataDirectory")).toString();
	if (!configuredDirectory.isEmpty()) return QDir::cleanPath(configuredDirectory);
	const auto documentsDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
	if (documentsDirectory.isEmpty()) return {};
	return QDir(documentsDirectory).filePath(QStringLiteral(WEAVER_MANAGER_DATA_DIRECTORY_NAME));
}

// Remove one managed MSI product and report any failure to the caller.
bool runUninstaller(const QString &msiexec, const QString &productCode, QString *errorMessage) {
	QProcess process;
	process.setProgram(msiexec);
	process.setArguments({"/x", productCode, "/passive", "/norestart"});
	process.start();
	if (!process.waitForStarted(5000) || !process.waitForFinished(-1)) {
		*errorMessage = process.errorString();
		return false;
	}
	const int exitCode = process.exitCode();
	if (process.exitStatus() != QProcess::NormalExit || exitCode != 0) {
		*errorMessage = QString("Windows Installer returned exit code %1.").arg(exitCode);
		return false;
	}
	return true;
}

int showFailure(const QString &message) {
	QMessageBox::critical(nullptr, "Weaver Manager Uninstall Failed", message);
	return 1;
}

}

// Uninstall managed Weaver products first, then remove only Manager-owned data.
int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon(QStringLiteral(":/branding/uninstall.png")));
	QCoreApplication::setApplicationName(WEAVER_MANAGER_NAME);
	const QStringList arguments = app.arguments();
	if (arguments.size() != 1)
		return showFailure("The uninstaller was started with invalid arguments.");

	const QString msiexec = findMsiexec();
	if (msiexec.isEmpty()) return showFailure("Windows Installer (msiexec.exe) could not be found.");

	QDir applicationDirectory(QCoreApplication::applicationDirPath());
	if (applicationDirectory.dirName().compare("bin", Qt::CaseInsensitive) == 0)
		applicationDirectory.cdUp();
	const QString managerInstallRoot = QDir::cleanPath(applicationDirectory.absolutePath());
	const QString versionsRoot = QDir(managerInstallRoot).filePath("installed_versions");
	const QStringList versions = QDir(versionsRoot).entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
	QStringList products;
	const QRegularExpression versionPattern("^\\d{2}\\.\\d{2}\\.\\d{2}$");
	for (const auto &version : versions) {
		if (!versionPattern.match(version).hasMatch()) continue;
		for (const auto &edition : {QStringLiteral("Desktop"), QStringLiteral("Server")}) {
			const auto installationDirectory = QDir(versionsRoot).filePath(version + "/" + edition);
			if (!QFileInfo(QDir(installationDirectory).filePath("bin/WeaverCLI.exe")).isFile()) continue;
			const auto productName = QString("Weaver-%1-%2-product").arg(version, edition).toUtf8();
			products.append(QUuid::createUuidV5(
				QUuid(QStringLiteral("{6ba7b810-9dad-11d1-80b4-00c04fd430c8}")), productName).toString().toUpper());
		}
	}

	if (!products.isEmpty()) {
		const auto answer = QMessageBox::warning(nullptr, "Uninstall Weaver Manager",
			QString("This will uninstall %1 Weaver version(s) managed by Weaver Manager, then remove Weaver Manager. Continue?")
				.arg(products.size()), QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
		if (answer != QMessageBox::Yes) return 1;
	}

	for (const auto &productCode : products) {
		QString errorMessage;
		if (!runUninstaller(msiexec, productCode, &errorMessage))
			return showFailure("A managed Weaver version could not be uninstalled. Weaver Manager will remain installed.\n\n"
							   + errorMessage);
	}

	const auto dataDirectory = managerDataDirectory();
	if (dataDirectory.isEmpty())
		return showFailure("Weaver Manager could not determine its data directory. Weaver Manager will remain installed.");
	for (const auto &directory : {QDir(managerInstallRoot).filePath("installed_versions"),
									 QDir(dataDirectory).filePath("installed_versions")}) {
		QString errorMessage;
		if (!removeDirectoryIfEmpty(directory, &errorMessage))
			return showFailure("Weaver Manager could not remove its empty version folders. Weaver Manager will remain installed.\n\n"
							   + errorMessage);
	}

	QString errorMessage;
	if (!removeDirectory(QDir(dataDirectory).filePath("logs"), &errorMessage))
			return showFailure("Weaver Manager could not remove its data folders. Weaver Manager will remain installed.\n\n"
							   + errorMessage);
	if (!removeDirectoryIfEmpty(dataDirectory, &errorMessage))
		return showFailure("Weaver Manager could not remove its empty Documents folder. Weaver Manager will remain installed.\n\n"
						   + errorMessage);

	const auto appDataDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	if (!appDataDirectory.isEmpty()) {
		if (!removeDirectory(appDataDirectory, &errorMessage))
			return showFailure("Weaver Manager could not remove its AppData folder. Weaver Manager will remain installed.\n\n"
						   + errorMessage);
	}

	QSettings legacySettings(QSettings::NativeFormat, QSettings::UserScope,
							 QStringLiteral("Weaver"), QStringLiteral(WEAVER_MANAGER_NAME));
	legacySettings.remove(QStringLiteral("updates/seenWeaverVersionEditions"));
	legacySettings.sync();
	return 0;
}
