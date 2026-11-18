#include "manager_logging.h"
#include "manager_paths.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QTextStream>
#include <cstdlib>

namespace {
QFile logFile;
QMutex logMutex;
QString logDirectory;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message) {
	const char *level = "debug";
	switch (type) {
	case QtDebugMsg: level = "debug"; break;
	case QtInfoMsg: level = "info"; break;
	case QtWarningMsg: level = "warning"; break;
	case QtCriticalMsg: level = "critical"; break;
	case QtFatalMsg: level = "fatal"; break;
	}

	QMutexLocker lock(&logMutex);
	if (!logFile.isOpen()) return;
	QTextStream stream(&logFile);
	stream << QDateTime::currentDateTime().toString(Qt::ISODateWithMs) << " [" << level << "] "
		   << message;
	if (context.file) stream << " (" << context.file << ':' << context.line << ')';
	stream << Qt::endl;
	logFile.flush();
	if (type == QtFatalMsg) std::abort();
}
}

QString managerLogDirectory() {
	return managerLogsDirectory();
}

void initializeManagerLogging() {
	logDirectory = managerLogsDirectory();
	if (logDirectory.isEmpty() || !QDir().mkpath(logDirectory)) {
		logDirectory.clear();
		qWarning() << "Could not create the Weaver Manager log directory.";
		return;
	}

	logFile.setFileName(QDir(logDirectory).filePath("weaver-manager.log"));
	if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
		logDirectory.clear();
		qWarning() << "Could not open the Weaver Manager log file:" << logFile.fileName() << logFile.errorString();
		return;
	}
	qInstallMessageHandler(messageHandler);
	qInfo().noquote() << "Weaver Manager started; log file:" << logFile.fileName();
}
