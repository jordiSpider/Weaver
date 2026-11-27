#include "main_window.h"
#include "manager_logging.h"
#include "manager_paths.h"

#include <QApplication>
#include <QCoreApplication>
#include <QIcon>

// Initialize shared application services before displaying the Manager window.
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(QStringLiteral(":/branding/manager.png")));
    QCoreApplication::setApplicationName(WEAVER_MANAGER_NAME);
    QCoreApplication::setApplicationVersion(WEAVER_MANAGER_VERSION);
    QString directoryError;
    const bool directoriesReady = initializeManagerDirectories(&directoryError);
    initializeManagerLogging();
    if (!directoriesReady) qCritical().noquote() << directoryError;

    MainWindow window;
    window.showMaximized();
    return app.exec();
}
