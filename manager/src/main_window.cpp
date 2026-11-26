#include "main_window.h"
#include "manager_logging.h"
#include "manager_paths.h"
#include "release_trust_config.h"

#include <QDir>
#include <QColor>
#include <QApplication>
#include <QActionGroup>
#include <QCryptographicHash>
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopServices>
#if ENABLE_TEST
#include <QDialog>
#include <QDialogButtonBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <functional>
#endif
#include <QEvent>
#include <QFile>
#if ENABLE_TEST
#include <QFileDialog>
#include <QLineEdit>
#endif
#include <QFileInfo>
#include <QFrame>
#include <QFont>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#if ENABLE_TEST
#include <QMimeData>
#endif
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QPushButton>
#include <QSaveFile>
#include <QStandardPaths>
#include <QSettings>
#include <QStatusBar>
#include <QStyle>
#include <QStyleHints>
#include <QSysInfo>
#include <QTabWidget>
#include <QScrollArea>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>
#include <QUuid>
#include <QUrlQuery>
#include <QVBoxLayout>
#include <QVersionNumber>
#include <QWidget>
#include <QUrl>
#include <QUrlQuery>
#if defined(Q_OS_WIN)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#include <limits>
#include <openssl/evp.h>

namespace {
constexpr int LatestReleaseRequest = std::numeric_limits<int>::min();
constexpr int DownloadRequest = std::numeric_limits<int>::max();
constexpr int ManagerReleasePageRequest = std::numeric_limits<int>::min() + 1;
constexpr int ReleaseSearchGenerationAttribute = static_cast<int>(QNetworkRequest::User) + 1;
constexpr int ReleaseValidationEntryAttribute = static_cast<int>(QNetworkRequest::User) + 2;
constexpr int ReleaseRequestTimeoutMs = 30000;
constexpr int MaxReleasePageRetries = 2;
constexpr int ReleasePageRetryDelayMs = 1000;

enum class ReleaseStatus { Stable, Latest, Prerelease, Test };

struct ReleaseColors {
    QColor installedBackground;
    QColor newBackground;
    QColor rowText;
    QColor installedText;
    QColor newText;
    QColor stableBackground;
    QColor stableText;
    QColor prereleaseBackground;
    QColor prereleaseText;
    QColor testBackground;
    QColor testText;
};

ReleaseColors releaseColors() {
    if (QApplication::palette().color(QPalette::Base).lightness() < 128) {
        return {QColor("#1d392c"), QColor("#453a1f"), QColor("#f5f2e8"),
                QColor("#a5d6a7"), QColor("#e1bee7"), QColor("#1b344c"),
                QColor("#90caf9"), QColor("#493420"), QColor("#ffcc80"),
                QColor("#3d2945"), QColor("#e1bee7")};
    }
    return {QColor("#e8f5e9"), QColor("#fff2cc"), QColor("#202020"),
            QColor("#1b5e20"), QColor("#6a1b9a"), QColor("#e3f2fd"),
            QColor("#0d47a1"), QColor("#fff3e0"), QColor("#8a4b08"),
            QColor("#f3e5f5"), QColor("#6a1b9a")};
}

QPalette managerPalette(bool dark) {
    QPalette palette;
    const QColor window(dark ? "#252b33" : "#f4f6f8");
    const QColor base(dark ? "#1c2229" : "#ffffff");
    const QColor text(dark ? "#edf1f5" : "#20252b");
    const QColor button(dark ? "#343e49" : "#e7edf3");
    const QColor disabled(dark ? "#9ba6b2" : "#626d78");
    palette.setColor(QPalette::Window, window);
    palette.setColor(QPalette::WindowText, text);
    palette.setColor(QPalette::Base, base);
    palette.setColor(QPalette::AlternateBase, window);
    palette.setColor(QPalette::Text, text);
    palette.setColor(QPalette::Button, button);
    palette.setColor(QPalette::ButtonText, text);
    palette.setColor(QPalette::ToolTipBase, dark ? QColor("#343e49") : QColor("#ffffdc"));
    palette.setColor(QPalette::ToolTipText, text);
    palette.setColor(QPalette::Highlight, dark ? QColor("#90caf9") : QColor("#1976d2"));
    palette.setColor(QPalette::HighlightedText, dark ? QColor("#102334") : QColor("#ffffff"));
    palette.setColor(QPalette::Link, dark ? QColor("#90caf9") : QColor("#0d47a1"));
    palette.setColor(QPalette::Disabled, QPalette::WindowText, disabled);
    palette.setColor(QPalette::Disabled, QPalette::Text, disabled);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled);
    return palette;
}

int compareManagerRelease(const QVersionNumber &leftVersion, int leftPrerelease,
                          const QVersionNumber &rightVersion, int rightPrerelease) {
    const int baseComparison = QVersionNumber::compare(leftVersion, rightVersion);
    if (baseComparison != 0) return baseComparison;
    if (leftPrerelease == rightPrerelease) return 0;
    if (leftPrerelease < 0) return 1;
    if (rightPrerelease < 0) return -1;
    return leftPrerelease < rightPrerelease ? -1 : 1;
}

struct ManagerReleaseVersion {
    QVersionNumber version;
    int prereleaseNumber{-1};
};

bool parseManagerReleaseTag(const QString &tag, ManagerReleaseVersion *parsed) {
    static const QRegularExpression pattern(
        R"(^v(\d+(?:\.\d+)+)-manager(?:-rc\.(\d+))?$)", QRegularExpression::CaseInsensitiveOption);
    const auto match = pattern.match(tag.trimmed());
    if (!match.hasMatch()) return false;
    parsed->version = QVersionNumber::fromString(match.captured(1));
    parsed->prereleaseNumber = match.captured(2).isEmpty() ? -1 : match.captured(2).toInt();
    return !parsed->version.isNull();
}

ManagerReleaseVersion currentManagerReleaseVersion() {
    static const QRegularExpression pattern(R"(^(.+?)(?:-rc\.(\d+))?$)", QRegularExpression::CaseInsensitiveOption);
    const auto match = pattern.match(QStringLiteral(WEAVER_MANAGER_DISPLAY_VERSION));
    ManagerReleaseVersion current;
    current.version = QVersionNumber::fromString(match.captured(1));
    current.prereleaseNumber = match.captured(2).isEmpty() ? -1 : match.captured(2).toInt();
    return current;
}

#if ENABLE_TEST
// Event filter installed on every widget of the dialog so a dropped file is
// accepted wherever it is released (child widgets would otherwise reject it).
class InstallerDropFilter final : public QObject {
public:
    InstallerDropFilter(QLineEdit *pathEdit, std::function<bool(const QString &)> isValid, QObject *parent)
        : QObject(parent), pathEdit_(pathEdit), isValid_(std::move(isValid)) {}

    bool eventFilter(QObject *watched, QEvent *event) override {
        switch (event->type()) {
        case QEvent::DragEnter:
        case QEvent::DragMove: {
            auto *drag = static_cast<QDropEvent *>(event);
            if (!localFile(drag->mimeData()).isEmpty()) {
                drag->setDropAction(Qt::CopyAction);
                drag->accept();
                return true;
            }
            break;
        }
        case QEvent::Drop: {
            auto *drop = static_cast<QDropEvent *>(event);
            const auto path = localFile(drop->mimeData());
            if (!path.isEmpty()) {
                pathEdit_->setText(path);
                drop->setDropAction(Qt::CopyAction);
                drop->accept();
                return true;
            }
            break;
        }
        default:
            break;
        }
        return QObject::eventFilter(watched, event);
    }

private:
    QString localFile(const QMimeData *mime) const {
        if (!mime || !mime->hasUrls()) return {};
        const auto urls = mime->urls();
        if (urls.size() != 1 || !urls.first().isLocalFile()) return {};
        const auto path = urls.first().toLocalFile();
        return isValid_ && !isValid_(path) ? QString() : path;
    }

    QLineEdit *pathEdit_{};
    std::function<bool(const QString &)> isValid_;
};
#endif

QString findRscriptExecutable() {
    auto executable = QStandardPaths::findExecutable("Rscript.exe");
    if (!executable.isEmpty()) return executable;

    QSettings rSettings(QSettings::NativeFormat, QSettings::SystemScope,
                        QStringLiteral("R-core"), QStringLiteral("R"));
    const auto installPath = rSettings.value(QStringLiteral("InstallPath")).toString();
    if (!installPath.isEmpty()) {
        const QDir rDirectory(installPath);
        for (const auto &candidate : {rDirectory.filePath("bin/x64/Rscript.exe"),
                                      rDirectory.filePath("bin/Rscript.exe")}) {
            if (QFileInfo(candidate).isFile()) return candidate;
        }
    }

    const auto rExecutable = QStandardPaths::findExecutable("R.exe");
    if (!rExecutable.isEmpty()) {
        const QDir rBin(QFileInfo(rExecutable).dir());
        for (const auto &candidate : {rBin.filePath("Rscript.exe"),
                                      rBin.filePath("x64/Rscript.exe")}) {
            if (QFileInfo(candidate).isFile()) return candidate;
        }
    }
    return {};
}

#if defined(Q_OS_WIN)
QString findRguiExecutable() {
    auto executable = QStandardPaths::findExecutable("Rgui.exe");
    if (!executable.isEmpty()) return executable;

    for (const auto scope : {QSettings::SystemScope, QSettings::UserScope}) {
        QSettings rSettings(QSettings::NativeFormat, scope,
                            QStringLiteral("R-core"), QStringLiteral("R"));
        const auto installPath = rSettings.value(QStringLiteral("InstallPath")).toString();
        if (installPath.isEmpty()) continue;

        const QDir rDirectory(installPath);
        for (const auto &candidate : {rDirectory.filePath("bin/x64/Rgui.exe"),
                                      rDirectory.filePath("bin/i386/Rgui.exe"),
                                      rDirectory.filePath("bin/Rgui.exe")}) {
            if (QFileInfo(candidate).isFile()) return candidate;
        }
    }

    const auto rExecutable = QStandardPaths::findExecutable("R.exe");
    if (!rExecutable.isEmpty()) {
        const QDir rBin(QFileInfo(rExecutable).dir());
        for (const auto &candidate : {rBin.filePath("Rgui.exe"),
                                      rBin.filePath("x64/Rgui.exe"),
                                      rBin.filePath("i386/Rgui.exe")}) {
            if (QFileInfo(candidate).isFile()) return candidate;
        }
    }
    return {};
}
#endif

class ReleaseTableWidgetItem final : public QTableWidgetItem {
public:
    using QTableWidgetItem::QTableWidgetItem;

    bool operator<(const QTableWidgetItem &other) const override {
        if (column() == 2) {
            const auto version = QVersionNumber::fromString(text());
            const auto otherVersion = QVersionNumber::fromString(other.text());
            if (!version.isNull() && !otherVersion.isNull()) {
                const int comparison = QVersionNumber::compare(version, otherVersion);
                if (comparison != 0) return comparison < 0;
            }
        }
        return QString::localeAwareCompare(text(), other.text()) < 0;
    }
};

class ReleaseTableHeader final : public QHeaderView {
public:
    explicit ReleaseTableHeader(Qt::Orientation orientation, QWidget *parent = nullptr)
        : QHeaderView(orientation, parent) {
        updateColors();
        connect(qApp, &QApplication::paletteChanged, this, [this] { updateColors(); });
    }

private:
    void updateColors() {
        const bool dark = QApplication::palette().color(QPalette::Base).lightness() < 128;
        const QString background = dark ? QStringLiteral("#303943") : QStringLiteral("#edf1f5");
        const QString foreground = dark ? QStringLiteral("#edf1f5") : QStringLiteral("#20252b");
        const QString border = dark ? QStringLiteral("#45515e") : QStringLiteral("#d3dae2");
        const QString hover = dark ? QStringLiteral("#3a4652") : QStringLiteral("#e1e8ef");
        setStyleSheet(QStringLiteral(
            "QHeaderView { background-color: %1; }"
            "QHeaderView::section { background-color: %1; color: %2;"
            " border: 0; border-right: 1px solid %3; border-bottom: 1px solid %3;"
            " padding-top: 6px; padding-bottom: 6px; }"
            "QHeaderView::section:hover { background-color: %4; }"
            "QHeaderView::section:pressed { background-color: %4; }")
                          .arg(background, foreground, border, hover));
        if (orientation() == Qt::Horizontal && parentWidget()) {
            parentWidget()->setStyleSheet(QStringLiteral(
                "QTableCornerButton::section { background-color: %1; border: 0;"
                " border-right: 1px solid %2; border-bottom: 1px solid %2; }")
                                              .arg(background, border));
        }
    }
};

// Authenticate the exact manifest bytes before any artifact metadata is trusted.
bool verifyEd25519Manifest(const QByteArray &manifest, const QByteArray &base64Signature) {
    const QByteArray publicKey = QByteArray::fromBase64(QByteArray(WEAVER_RELEASE_PUBLIC_KEY_BASE64),
                                                        QByteArray::AbortOnBase64DecodingErrors);
    const QByteArray signature = QByteArray::fromBase64(base64Signature.trimmed(),
                                                        QByteArray::AbortOnBase64DecodingErrors);
    if (publicKey.size() != 32 || signature.size() != 64) return false;

    EVP_PKEY *key = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr,
                                                reinterpret_cast<const unsigned char *>(publicKey.constData()),
                                                static_cast<size_t>(publicKey.size()));
    if (!key) return false;
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    if (!context) {
        EVP_PKEY_free(key);
        return false;
    }
    const bool valid = EVP_DigestVerifyInit(context, nullptr, nullptr, nullptr, key) == 1
        && EVP_DigestVerify(context,
                            reinterpret_cast<const unsigned char *>(signature.constData()),
                            static_cast<size_t>(signature.size()),
                            reinterpret_cast<const unsigned char *>(manifest.constData()),
                            static_cast<size_t>(manifest.size())) == 1;
    EVP_MD_CTX_free(context);
    EVP_PKEY_free(key);
    return valid;
}

QString platformLabel() {
#ifdef Q_OS_WIN
    return "Windows";
#elif defined(Q_OS_MACOS)
    return "macOS";
#else
    return "Linux";
#endif
}

QString currentPlatformId() {
    const auto architecture = QSysInfo::currentCpuArchitecture().toLower();
    const QString arch = (architecture == "x86_64" || architecture == "amd64" || architecture == "x64")
        ? "x64" : (architecture == "aarch64" || architecture == "arm64") ? "arm64" : architecture;
#if defined(Q_OS_WIN)
    return "windows-" + arch;
#elif defined(Q_OS_MACOS)
    return "macos-" + arch;
#else
    return "linux-" + arch;
#endif
}

QString weaverCliExecutableName() {
#if defined(Q_OS_WIN)
    return QStringLiteral("WeaverCLI.exe");
#else
    return QStringLiteral("WeaverCLI");
#endif
}

QStringList weaverInstallationRoots() {
    QStringList roots{QDir(managerProgramInstallRoot()).filePath("installed_versions")};
#if defined(Q_OS_MACOS)
    roots.append(QDir(QDir::homePath()).filePath("Applications/Weaver"));
    roots.append(QStringLiteral("/Applications/Weaver"));
    roots.append(QStringLiteral("/usr/local/Weaver"));
    roots.append(QStringLiteral("/opt/Weaver"));
#elif defined(Q_OS_LINUX)
    roots.append(QStringLiteral("/opt/Weaver"));
#endif
    roots.removeDuplicates();
    return roots;
}

QString findWeaverInstallationRoot(const QString &version, const QString &edition) {
    for (const auto &versionsRoot : weaverInstallationRoots()) {
        const auto root = QDir(versionsRoot).filePath(QString("%1/%2").arg(version, edition));
        if (QFileInfo(QDir(root).filePath("bin/" + weaverCliExecutableName())).isFile()) return root;
    }
    return {};
}

QString shellQuoted(const QString &value) {
    QString quoted = value;
    quoted.replace(QLatin1Char('\''), QStringLiteral("'\\''"));
    return QLatin1Char('\'') + quoted + QLatin1Char('\'');
}

bool parseInstallerName(const QString &assetName, QString *edition, QString *version, QString *platform) {
    static const QRegularExpression installerNamePattern(
        R"(^Weaver-(Desktop|Server)-(\d{2}\.\d{2}\.\d{2})-(Windows|Linux|macOS)-(x64|arm64)\.(msi|deb|rpm|pkg|dmg)$)",
        QRegularExpression::CaseInsensitiveOption);
    const auto match = installerNamePattern.match(QFileInfo(assetName).fileName());
    if (!match.hasMatch()) return false;

    const auto os = match.captured(3).toLower();
    *edition = match.captured(1);
    *version = match.captured(2);
    *platform = (os == "macos" ? QStringLiteral("macos") : os) + "-" + match.captured(4).toLower();
    return true;
}

QString seenVersionsCacheFilePath() {
    const auto appDataDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (appDataDirectory.isEmpty() || !QDir().mkpath(appDataDirectory)) return {};
    return QDir(appDataDirectory).filePath("release-cache.ini");
}

QString releaseEntryKey(const QString &tag, const QString &version, const QString &edition) {
    return tag + QLatin1Char('|') + version + QLatin1Char('|') + edition;
}

void migrateSeenVersionsCache(QSettings &settings) {
    const QString key = QStringLiteral("updates/seenWeaverVersionEditions");
    if (settings.contains(key)) return;

    QSettings legacySettings(QSettings::NativeFormat, QSettings::UserScope,
                             QStringLiteral("Weaver"), QString::fromLatin1(WEAVER_MANAGER_NAME));
    if (!legacySettings.contains(key)) return;
    settings.setValue(key, legacySettings.value(key));
    settings.sync();
}

}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    const auto windowTitle = QStringLiteral(WEAVER_MANAGER_NAME)
        + QStringLiteral(" %1").arg(QStringLiteral(WEAVER_MANAGER_DISPLAY_VERSION))
        + (QStringLiteral(WEAVER_MANAGER_DISPLAY_VERSION) == QStringLiteral(WEAVER_MANAGER_VERSION)
               ? QString() : QStringLiteral(" (Pre-Release)"));
    setWindowTitle(windowTitle);
    resize(960, 620);

    systemDarkFallback_ = QApplication::palette().color(QPalette::Window).lightness() < 128;
    const auto themeSettingsPath = seenVersionsCacheFilePath();
    if (!themeSettingsPath.isEmpty()) {
        QSettings settings(themeSettingsPath, QSettings::IniFormat);
        theme_ = settings.value(QStringLiteral("appearance/theme"), QStringLiteral("system")).toString();
        if (theme_ != "system" && theme_ != "light" && theme_ != "dark") {
            qWarning() << "Invalid saved theme:" << theme_;
            theme_ = QStringLiteral("system");
        }
    } else {
        qWarning() << "Could not access the settings directory for the theme preference";
    }
    applyTheme();

    auto *notificationsMenu = menuBar()->addMenu("Notifications");
    auto *managerNotificationsMenu = notificationsMenu->addMenu("Weaver Manager");
    updateNotificationsAction_ = managerNotificationsMenu->addAction("Show update notifications");
    updateNotificationsAction_->setCheckable(true);
    updateNotificationsAction_->setChecked(true);
    const auto notificationSettingsPath = seenVersionsCacheFilePath();
    if (!notificationSettingsPath.isEmpty()) {
        QSettings settings(notificationSettingsPath, QSettings::IniFormat);
        updateNotificationsAction_->setChecked(settings.value(QStringLiteral("updates/notifyManagerUpdates"), true).toBool());
    }
    connect(updateNotificationsAction_, &QAction::toggled, this, [this](bool enabled) {
        const auto cacheFilePath = seenVersionsCacheFilePath();
        if (cacheFilePath.isEmpty()) return;
        QSettings settings(cacheFilePath, QSettings::IniFormat);
        settings.setValue(QStringLiteral("updates/notifyManagerUpdates"), enabled);
        if (enabled) {
            settings.remove(QStringLiteral("updates/dismissedManagerVersion"));
            settings.remove(QStringLiteral("updates/dismissedManagerStableTag"));
            settings.remove(QStringLiteral("updates/dismissedManagerPrereleaseTag"));
        }
        settings.sync();
        if (enabled && managerReleasesLoaded_) showManagerUpdateNotification();
    });
    auto *notifyPrereleases = managerNotificationsMenu->addAction("Notify about pre-release versions");
    notifyPrereleases->setCheckable(true);
    if (!notificationSettingsPath.isEmpty()) {
        QSettings settings(notificationSettingsPath, QSettings::IniFormat);
        notifyPrereleases->setChecked(settings.value(QStringLiteral("updates/notifyManagerPrereleases"), false).toBool());
    }
    connect(notifyPrereleases, &QAction::toggled, this, [this](bool enabled) {
        const auto cacheFilePath = seenVersionsCacheFilePath();
        if (cacheFilePath.isEmpty()) return;
        QSettings settings(cacheFilePath, QSettings::IniFormat);
        settings.setValue(QStringLiteral("updates/notifyManagerPrereleases"), enabled);
        settings.sync();
        if (managerReleasesLoaded_) showManagerUpdateNotification();
    });
    auto *weaverNotificationsMenu = notificationsMenu->addMenu("Weaver releases");
    weaverNotificationsAction_ = weaverNotificationsMenu->addAction("Show new release notifications");
    weaverNotificationsAction_->setCheckable(true);
    weaverNotificationsAction_->setChecked(true);
    weaverPrereleaseNotificationsAction_ = weaverNotificationsMenu->addAction("Notify about pre-release versions");
    weaverPrereleaseNotificationsAction_->setCheckable(true);
    if (!notificationSettingsPath.isEmpty()) {
        QSettings settings(notificationSettingsPath, QSettings::IniFormat);
        weaverNotificationsAction_->setChecked(settings.value(QStringLiteral("updates/notifyWeaverReleases"), true).toBool());
        weaverPrereleaseNotificationsAction_->setChecked(settings.value(QStringLiteral("updates/notifyWeaverPrereleases"), false).toBool());
    } else {
        qWarning() << "Could not access the settings directory for notification preferences";
    }
    connect(weaverNotificationsAction_, &QAction::toggled, this, [this](bool enabled) {
        savePreference(QStringLiteral("updates/notifyWeaverReleases"), enabled);
    });
    connect(weaverPrereleaseNotificationsAction_, &QAction::toggled, this, [this](bool enabled) {
        savePreference(QStringLiteral("updates/notifyWeaverPrereleases"), enabled);
    });
    auto *themeMenu = menuBar()->addMenu("Theme");
    auto *themeActions = new QActionGroup(this);
    themeActions->setExclusive(true);
    for (const auto &option : {qMakePair(QStringLiteral("System"), QStringLiteral("system")),
                               qMakePair(QStringLiteral("Light"), QStringLiteral("light")),
                               qMakePair(QStringLiteral("Dark"), QStringLiteral("dark"))}) {
        auto *action = themeMenu->addAction(option.first);
        action->setCheckable(true);
        action->setData(option.second);
        action->setChecked(theme_ == option.second);
        themeActions->addAction(action);
    }
    connect(themeActions, &QActionGroup::triggered, this, [this](QAction *action) {
        theme_ = action->data().toString();
        applyTheme();
        const auto path = seenVersionsCacheFilePath();
        if (!path.isEmpty()) {
            QSettings settings(path, QSettings::IniFormat);
            settings.setValue(QStringLiteral("appearance/theme"), theme_);
            settings.sync();
            if (settings.status() == QSettings::NoError) return;
        }
        qWarning() << "Could not save the theme preference";
        QMessageBox::warning(this, "Theme Preference Not Saved",
                             "The theme was applied, but the preference could not be saved.");
    });
    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged, this, [this] {
        if (theme_ == "system") applyTheme();
    });
    connect(qApp, &QApplication::paletteChanged, this, [this] {
        if (releaseTable_) refreshReleaseCacheStatuses();
    });

    auto *helpMenu = menuBar()->addMenu("Help");
    auto *managerDocumentation = helpMenu->addAction("Weaver Manager Documentation");
    connect(managerDocumentation, &QAction::triggered, this, [] {
        QDesktopServices::openUrl(QUrl(QStringLiteral("https://jordiSpider.github.io/Weaver/weaver-manager/%1/")
                                           .arg(QStringLiteral(WEAVER_MANAGER_DOCUMENTATION_VERSION))));
    });

    auto *tabs = new QTabWidget(this);
    auto *installedPage = new QWidget(tabs);
    auto *installedLayout = new QVBoxLayout(installedPage);
    auto *installedScrollArea = new QScrollArea(installedPage);
    installedScrollArea->setWidgetResizable(true);
    installedListContent_ = new QWidget(installedScrollArea);
    installedListLayout_ = new QVBoxLayout(installedListContent_);
    installedListLayout_->setAlignment(Qt::AlignTop);
    installedScrollArea->setWidget(installedListContent_);
    installedLayout->addWidget(installedScrollArea);
    auto *installedButtons = new QHBoxLayout;
    auto *refreshInstalledButton = new QPushButton("Refresh Installed Versions", installedPage);
    installedButtons->addWidget(refreshInstalledButton);
    installedButtons->addStretch();
    installedLayout->addLayout(installedButtons);
    connect(refreshInstalledButton, &QPushButton::clicked, this, &MainWindow::refreshInstalledVersions);
    tabs->addTab(installedPage, "Installed Versions");

    auto *releasePage = new QWidget(tabs);
    auto *releaseLayout = new QVBoxLayout(releasePage);
    releaseTable_ = new QTableWidget(0, 8, releasePage);
    releaseTable_->setHorizontalHeader(new ReleaseTableHeader(Qt::Horizontal, releaseTable_));
    releaseTable_->setVerticalHeader(new ReleaseTableHeader(Qt::Vertical, releaseTable_));
    releaseTable_->setHorizontalHeaderLabels({"", "Cache status", "Version", "Edition", "Published", "Installer", "Release status", "GitHub tag"});
    auto headerFont = releaseTable_->horizontalHeader()->font();
    headerFont.setBold(true);
    releaseTable_->horizontalHeader()->setFont(headerFont);
    QPushButton installButtonWidthProbe("Install");
    releaseTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    releaseTable_->setColumnWidth(0, installButtonWidthProbe.sizeHint().width() + 16);
    releaseTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    releaseTable_->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    releaseTable_->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    releaseTable_->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    releaseTable_->horizontalHeader()->setSortIndicator(2, Qt::DescendingOrder);
    releaseTable_->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    releaseTable_->horizontalHeader()->setSectionsClickable(true);
    releaseTable_->horizontalHeader()->setSortIndicatorShown(true);
    connect(releaseTable_->horizontalHeader(), &QHeaderView::sectionClicked, this,
            [this, sortColumn = 2, sortOrder = Qt::DescendingOrder](int section) mutable {
        auto *header = releaseTable_->horizontalHeader();
        if (section == sortColumn) {
            sortOrder = sortOrder == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder;
        } else {
            sortColumn = section;
            sortOrder = Qt::AscendingOrder;
        }
        header->setSortIndicator(sortColumn, sortOrder);
        releaseTable_->sortItems(sortColumn, sortOrder);
        applyReleaseFilter();
    });
    releaseTable_->horizontalHeader()->setToolTip("Click a column heading to sort the releases.");
    releaseTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    releaseLayout->addWidget(releaseTable_);
    auto *releaseButtons = new QHBoxLayout;
    auto *refreshButton = new QPushButton("Refresh GitHub Releases", releasePage);
    releaseButtons->addWidget(refreshButton);
    showPrereleasesCheckBox_ = new QCheckBox("Show pre-releases", releasePage);
    showPrereleasesCheckBox_->setChecked(true);
    if (!notificationSettingsPath.isEmpty()) {
        QSettings settings(notificationSettingsPath, QSettings::IniFormat);
        showPrereleasesCheckBox_->setChecked(settings.value(QStringLiteral("releases/showPrereleases"), true).toBool());
    }
    connect(showPrereleasesCheckBox_, &QCheckBox::toggled, this, [this](bool enabled) {
        savePreference(QStringLiteral("releases/showPrereleases"), enabled);
        applyReleaseFilter();
    });
    releaseButtons->addWidget(showPrereleasesCheckBox_);
#if ENABLE_TEST
    auto *addTestInstallerButton = new QPushButton("Add Test Installer", releasePage);
    releaseButtons->addWidget(addTestInstallerButton);
    connect(addTestInstallerButton, &QPushButton::clicked, this, &MainWindow::addLocalTestInstaller);
#endif
    releaseButtons->addStretch();
    releaseLayout->addLayout(releaseButtons);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshReleases);
    connect(&network_, &QNetworkAccessManager::finished, this, &MainWindow::onReleasesReceived);
    connect(&managerUpdateNetwork_, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        handleManagerReleasePage(reply, reply->readAll());
    });
    connect(&releaseValidationNetwork_, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        const auto data = reply->readAll();
        const auto entry = reply->request()
            .attribute(static_cast<QNetworkRequest::Attribute>(ReleaseValidationEntryAttribute)).toJsonObject();
        const auto requestedReleaseId = entry.value("release_id").toVariant().toLongLong();
        const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 404) {
            reply->deleteLater();
            refreshReleases();
            QMessageBox::warning(this, "Weaver Release Removed",
                QString("Weaver %1 (%2) was removed from GitHub Releases while Weaver Manager was running. The release list will now be refreshed.")
                    .arg(entry.value("version").toString(), entry.value("edition").toString()));
            return;
        }
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "Could Not Verify Weaver Release",
                "Weaver Manager could not verify that this release still exists on GitHub. The installation was cancelled.");
            qWarning() << "Could not verify GitHub release" << requestedReleaseId << reply->errorString();
            reply->deleteLater();
            return;
        }

        QJsonParseError parseError;
        const auto document = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error != QJsonParseError::NoError || !document.isObject()
            || document.object().value("id").toVariant().toLongLong() != requestedReleaseId) {
            QMessageBox::warning(this, "Could Not Verify Weaver Release",
                "GitHub did not return the expected release. The installation was cancelled.");
            reply->deleteLater();
            return;
        }
        reply->deleteLater();
        continueVerifiedInstallerInstall(entry);
    });
    connect(&installerProcess_, &QProcess::readyReadStandardOutput, this, [this] {
        const auto output = QString::fromLocal8Bit(installerProcess_.readAllStandardOutput()).trimmed();
        if (!output.isEmpty()) qInfo().noquote() << "Installer stdout:" << output;
    });
    connect(&installerProcess_, &QProcess::readyReadStandardError, this, [this] {
        const auto output = QString::fromLocal8Bit(installerProcess_.readAllStandardError()).trimmed();
        if (!output.isEmpty()) qWarning().noquote() << "Installer stderr:" << output;
    });
    connect(&installerProcess_, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this,
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
        qInfo() << "Installer process finished; exit code:" << exitCode << "exit status:" << exitStatus;
        releaseTable_->setEnabled(true);
        loadInstallations();
        refreshReleaseCacheStatuses();
        const bool succeeded = exitStatus == QProcess::NormalExit && exitCode == 0;
        auto status = succeeded
            ? QStringLiteral("Installer process completed successfully.")
            : QString("Installer process finished with exit code %1.").arg(exitCode);
        if (!succeeded && !installerLogPath_.isEmpty()) {
            status += " MSI log: " + installerLogPath_;
            qWarning().noquote() << "MSI diagnostic log:" << installerLogPath_;
        }
        installerLogPath_.clear();
        setStatus(status);
        if (closeWhenInstallerFinishes_) close();
    });
    connect(&installerProcess_, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        qWarning() << "Installer process error:" << error << installerProcess_.errorString();
        if (error == QProcess::FailedToStart) {
            releaseTable_->setEnabled(true);
            setStatus("The verified installer could not be started: " + installerProcess_.errorString());
            if (closeWhenInstallerFinishes_) close();
        }
    });
    tabs->addTab(releasePage, "Available Releases");

    status_ = new QLabel("Ready", this);
    statusBar()->addWidget(status_);
    setCentralWidget(tabs);
    loadInstallations();
    requestManagerReleasePage(1);
    refreshReleases();
}

void MainWindow::savePreference(const QString &key, bool enabled) {
    const auto path = seenVersionsCacheFilePath();
    if (!path.isEmpty()) {
        QSettings settings(path, QSettings::IniFormat);
        settings.setValue(key, enabled);
        settings.sync();
        if (settings.status() == QSettings::NoError) return;
    }
    qWarning() << "Could not save preference" << key;
    QMessageBox::warning(this, "Preference Not Saved",
                         "The preference was applied for this session, but could not be saved.");
}

void MainWindow::applyTheme() {
    bool dark = theme_ == "dark";
    if (theme_ == "system") {
        const auto scheme = qApp->styleHints()->colorScheme();
        if (scheme == Qt::ColorScheme::Unknown) {
            qWarning() << "System color scheme is unavailable; using the initial system palette";
            dark = systemDarkFallback_;
        } else {
            dark = scheme == Qt::ColorScheme::Dark;
        }
    }
    if (!QApplication::setStyle(QStringLiteral("Fusion")))
        qWarning() << "Could not apply the Fusion style";
    QApplication::setPalette(managerPalette(dark));
    if (releaseTable_) refreshReleaseCacheStatuses();
}

void MainWindow::requestManagerReleasePage(int page) {
    if (page == 1) {
        latestManagerStableTag_.clear();
        latestManagerStableUrl_.clear();
        latestManagerStableVersion_.clear();
        latestManagerPrereleaseTag_.clear();
        latestManagerPrereleaseUrl_.clear();
        latestManagerPrereleaseVersion_.clear();
        latestManagerPrereleaseNumber_ = 0;
        managerReleasesLoaded_ = false;
    }
    QNetworkRequest request(QUrl(QString("https://api.github.com/repos/%1/releases?per_page=100&page=%2")
                                 .arg(WEAVER_REPOSITORY).arg(page)));
    request.setTransferTimeout(ReleaseRequestTimeoutMs);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
    request.setRawHeader("Cache-Control", "no-cache, no-store");
    request.setRawHeader("Pragma", "no-cache");
    auto url = request.url();
    QUrlQuery query(url);
    query.addQueryItem("_fresh", QUuid::createUuid().toString(QUuid::WithoutBraces));
    url.setQuery(query);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "WeaverManager/" WEAVER_MANAGER_VERSION);
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setAttribute(QNetworkRequest::User, ManagerReleasePageRequest);
    request.setAttribute(static_cast<QNetworkRequest::Attribute>(ReleaseSearchGenerationAttribute), page);
    managerUpdateNetwork_.get(request);
}

void MainWindow::handleManagerReleasePage(QNetworkReply *reply, const QByteArray &data) {
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Could not check for Weaver Manager updates:" << reply->errorString();
        latestManagerStableTag_.clear();
        latestManagerStableUrl_.clear();
        latestManagerStableVersion_.clear();
        latestManagerPrereleaseTag_.clear();
        latestManagerPrereleaseUrl_.clear();
        latestManagerPrereleaseVersion_.clear();
        latestManagerPrereleaseNumber_ = 0;
        managerReleasesLoaded_ = false;
        reply->deleteLater();
        return;
    }

    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isArray()) {
        qWarning() << "GitHub returned an invalid Weaver Manager release list.";
        latestManagerStableTag_.clear();
        latestManagerStableUrl_.clear();
        latestManagerStableVersion_.clear();
        latestManagerPrereleaseTag_.clear();
        latestManagerPrereleaseUrl_.clear();
        latestManagerPrereleaseVersion_.clear();
        latestManagerPrereleaseNumber_ = 0;
        managerReleasesLoaded_ = false;
        reply->deleteLater();
        return;
    }

    const int page = reply->request()
        .attribute(static_cast<QNetworkRequest::Attribute>(ReleaseSearchGenerationAttribute)).toInt();
    const auto releases = document.array();
    for (const auto &releaseValue : releases) {
        const auto release = releaseValue.toObject();
        if (release.value("draft").toBool()) continue;
        ManagerReleaseVersion candidate;
        const auto tag = release.value("tag_name").toString().trimmed();
        if (!parseManagerReleaseTag(tag, &candidate)) continue;
        const auto url = release.value("html_url").toString();
        if (candidate.prereleaseNumber < 0) {
            if (latestManagerStableVersion_.isEmpty()
                || compareManagerRelease(candidate.version, -1,
                                         QVersionNumber::fromString(latestManagerStableVersion_), -1) > 0) {
                latestManagerStableTag_ = tag;
                latestManagerStableUrl_ = url;
                latestManagerStableVersion_ = candidate.version.toString();
            }
        } else if (latestManagerPrereleaseVersion_.isEmpty()
                   || compareManagerRelease(candidate.version, candidate.prereleaseNumber,
                                            QVersionNumber::fromString(latestManagerPrereleaseVersion_),
                                            latestManagerPrereleaseNumber_) > 0) {
            latestManagerPrereleaseTag_ = tag;
            latestManagerPrereleaseUrl_ = url;
            latestManagerPrereleaseVersion_ = candidate.version.toString();
            latestManagerPrereleaseNumber_ = candidate.prereleaseNumber;
        }
    }

    reply->deleteLater();
    if (releases.size() == 100) {
        requestManagerReleasePage(page + 1);
        return;
    }
    managerReleasesLoaded_ = true;
    showManagerUpdateNotification();
}

void MainWindow::showManagerUpdateNotification() {
    if (!managerReleasesLoaded_) return;
    const auto cacheFilePath = seenVersionsCacheFilePath();
    if (cacheFilePath.isEmpty()) return;
    QSettings settings(cacheFilePath, QSettings::IniFormat);
    if (!settings.value(QStringLiteral("updates/notifyManagerUpdates"), true).toBool()) return;
    const auto current = currentManagerReleaseVersion();
    const bool notifyPrereleases = settings.value(QStringLiteral("updates/notifyManagerPrereleases"), false).toBool();
    const bool hasNewStable = !latestManagerStableVersion_.isEmpty()
        && compareManagerRelease(QVersionNumber::fromString(latestManagerStableVersion_), -1,
                                 current.version, current.prereleaseNumber) > 0;
    const bool hasNewPrerelease = notifyPrereleases && !latestManagerPrereleaseVersion_.isEmpty()
        && compareManagerRelease(QVersionNumber::fromString(latestManagerPrereleaseVersion_),
                                 latestManagerPrereleaseNumber_, current.version,
                                 current.prereleaseNumber) > 0;
    const bool stableWasDismissed = settings.value(QStringLiteral("updates/dismissedManagerStableTag")).toString()
            == latestManagerStableTag_
        || settings.value(QStringLiteral("updates/dismissedManagerVersion")).toString()
            == latestManagerStableVersion_;
    const bool prereleaseWasDismissed = settings.value(QStringLiteral("updates/dismissedManagerPrereleaseTag"))
            .toString() == latestManagerPrereleaseTag_;
    const bool showStable = hasNewStable && !stableWasDismissed;
    const bool showPrerelease = hasNewPrerelease && !prereleaseWasDismissed;
    const bool prereleaseIsNewest = showPrerelease
        && (!showStable
            || compareManagerRelease(QVersionNumber::fromString(latestManagerPrereleaseVersion_),
                                    latestManagerPrereleaseNumber_,
                                    QVersionNumber::fromString(latestManagerStableVersion_), -1) > 0);

    QStringList notificationLines;
    QStringList links;
    QStringList candidateTags;
    if (showStable) {
        const auto stableMessage = current.prereleaseNumber >= 0
                && QVersionNumber::compare(QVersionNumber::fromString(latestManagerStableVersion_), current.version) == 0
            ? QString("The stable Weaver Manager %1 release is available. You are currently using the pre-release test version %2 of this same version.")
                  .arg(latestManagerStableVersion_, QStringLiteral(WEAVER_MANAGER_DISPLAY_VERSION))
            : QString("Weaver Manager %1 is available. You are using version %2.")
                  .arg(latestManagerStableVersion_, QStringLiteral(WEAVER_MANAGER_DISPLAY_VERSION));
        notificationLines.append(stableMessage);
        links.append(QString("Stable release: <a href=\"%1\">%2</a>")
                         .arg(latestManagerStableUrl_, latestManagerStableTag_));
        candidateTags.append(latestManagerStableTag_);
    }
    if (showPrerelease) {
        const bool newerRcOfCurrentBase = current.prereleaseNumber >= 0
            && QVersionNumber::compare(QVersionNumber::fromString(latestManagerPrereleaseVersion_), current.version) == 0;
        const auto prereleaseMessage = newerRcOfCurrentBase
            ? QString("A more advanced test version, %1, is available for version %2. You are currently using %3.")
                  .arg(QString("%1-rc.%2").arg(latestManagerPrereleaseVersion_).arg(latestManagerPrereleaseNumber_),
                       current.version.toString(), QStringLiteral(WEAVER_MANAGER_DISPLAY_VERSION))
            : QString("A newer Weaver Manager pre-release %1 is available. You are using version %2. This is a test version and may be unstable.")
                  .arg(QString("%1-rc.%2").arg(latestManagerPrereleaseVersion_).arg(latestManagerPrereleaseNumber_),
                       QStringLiteral(WEAVER_MANAGER_DISPLAY_VERSION));
        notificationLines.append(prereleaseMessage);
        links.append(QString("Pre-release: <a href=\"%1\">%2</a>")
                         .arg(latestManagerPrereleaseUrl_, latestManagerPrereleaseTag_));
        candidateTags.append(latestManagerPrereleaseTag_);
    }
    if (notificationLines.isEmpty()) return;

    QMessageBox messageBox(prereleaseIsNewest ? QMessageBox::Warning : QMessageBox::Information,
                           "Weaver Manager Update Available",
                           QString(), QMessageBox::Ok, this);
    QStringList richNotificationLines;
    for (const auto &line : notificationLines)
        richNotificationLines.append(QStringLiteral("<p>%1</p>").arg(line.toHtmlEscaped()));
    messageBox.setText(richNotificationLines.join(QString()));
    auto *dismissCheckBox = new QCheckBox("Do not show this notification again for this version", &messageBox);
    messageBox.setCheckBox(dismissCheckBox);
    messageBox.setInformativeText(links.join("<br>"));
    messageBox.setTextFormat(Qt::RichText);
    messageBox.exec();
    if (dismissCheckBox->isChecked()) {
        for (const auto &tag : candidateTags) {
            if (tag == latestManagerStableTag_)
                settings.setValue(QStringLiteral("updates/dismissedManagerStableTag"), tag);
            if (tag == latestManagerPrereleaseTag_)
                settings.setValue(QStringLiteral("updates/dismissedManagerPrereleaseTag"), tag);
        }
        settings.setValue(QStringLiteral("updates/notifyManagerUpdates"), false);
        settings.sync();
        updateNotificationsAction_->setChecked(false);
    }
}

void MainWindow::refreshReleases() {
    ++releaseSearchGeneration_;
    releasePageRetryCount_ = 0;
    seenReleaseEntriesAtSearchStart_.clear();
    seenCacheFormat_ = SeenCacheFormat::Tag;
    const auto cacheFilePath = seenVersionsCacheFilePath();
    if (!cacheFilePath.isEmpty()) {
        QSettings settings(cacheFilePath, QSettings::IniFormat);
        migrateSeenVersionsCache(settings);
        QString snapshotKey = QStringLiteral("updates/seenWeaverReleaseTags");
        if (!settings.contains(snapshotKey)) {
            snapshotKey = QStringLiteral("updates/seenWeaverReleases");
            seenCacheFormat_ = SeenCacheFormat::ReleaseId;
            if (!settings.contains(snapshotKey)) {
                snapshotKey = QStringLiteral("updates/seenWeaverVersionEditions");
                seenCacheFormat_ = SeenCacheFormat::VersionEdition;
            }
        }
        for (const auto &seenEntry : settings.value(snapshotKey).toStringList())
            seenReleaseEntriesAtSearchStart_.insert(seenEntry);
    } else {
        qWarning() << "Could not access the settings directory for the seen Weaver releases cache";
    }

    releaseItems_ = {};
    releaseManifests_ = {};
    catalogEntries_ = {};
    newReleaseEntries_.clear();
    releaseTable_->setRowCount(0);
    pendingManifests_ = 0;
    latestReleaseId_ = 0;
    downloadedManifests_.clear();
    downloadedSignatures_.clear();
    setStatus("Resolving the latest stable release…");
    QNetworkRequest latestRequest(QUrl("https://api.github.com/repos/" WEAVER_REPOSITORY "/releases/latest"));
    latestRequest.setTransferTimeout(ReleaseRequestTimeoutMs);
    latestRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    latestRequest.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
    latestRequest.setRawHeader("Cache-Control", "no-cache, no-store");
    latestRequest.setRawHeader("Pragma", "no-cache");
    auto latestUrl = latestRequest.url();
    QUrlQuery latestQuery(latestUrl);
    latestQuery.addQueryItem("_fresh", QUuid::createUuid().toString(QUuid::WithoutBraces));
    latestUrl.setQuery(latestQuery);
    latestRequest.setUrl(latestUrl);
    latestRequest.setHeader(QNetworkRequest::UserAgentHeader, "WeaverManager/" WEAVER_MANAGER_VERSION);
    latestRequest.setRawHeader("Accept", "application/vnd.github+json");
    latestRequest.setAttribute(QNetworkRequest::User, LatestReleaseRequest);
    latestRequest.setAttribute(static_cast<QNetworkRequest::Attribute>(ReleaseSearchGenerationAttribute),
                               QVariant::fromValue<qulonglong>(releaseSearchGeneration_));
    network_.get(latestRequest);
}

void MainWindow::requestReleasePage(int page) {
    QNetworkRequest request(QUrl(QString("https://api.github.com/repos/%1/releases?per_page=100&page=%2")
                                 .arg(WEAVER_REPOSITORY).arg(page)));
    request.setTransferTimeout(ReleaseRequestTimeoutMs);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
    request.setRawHeader("Cache-Control", "no-cache, no-store");
    request.setRawHeader("Pragma", "no-cache");
    auto url = request.url();
    QUrlQuery query(url);
    query.addQueryItem("_fresh", QUuid::createUuid().toString(QUuid::WithoutBraces));
    url.setQuery(query);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "WeaverManager/" WEAVER_MANAGER_VERSION);
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setAttribute(QNetworkRequest::User, page);
    request.setAttribute(static_cast<QNetworkRequest::Attribute>(ReleaseSearchGenerationAttribute),
                         QVariant::fromValue<qulonglong>(releaseSearchGeneration_));
    network_.get(request);
}

void MainWindow::retryReleasePage(int page, const QString &errorMessage) {
    if (releasePageRetryCount_ < MaxReleasePageRetries) {
        ++releasePageRetryCount_;
        qWarning() << "GitHub release catalog request failed; retrying page" << page
                   << "attempt" << releasePageRetryCount_ << "of" << MaxReleasePageRetries
                   << errorMessage;
        const auto generation = releaseSearchGeneration_;
        QTimer::singleShot(ReleasePageRetryDelayMs, this, [this, page, generation] {
            if (generation != releaseSearchGeneration_) return;
            requestReleasePage(page);
        });
        return;
    }
    releasePageRetryCount_ = 0;
    setStatus("Could not load GitHub releases: " + errorMessage);
}

// Route asynchronous responses and enforce size/hash checks before installer launch.
void MainWindow::onReleasesReceived(QNetworkReply *reply) {
    const auto requestPage = reply->request().attribute(QNetworkRequest::User).toInt();
    const auto data = reply->readAll();
    if (requestPage == ManagerReleasePageRequest) {
        handleManagerReleasePage(reply, data);
        return;
    }
    if (requestPage == DownloadRequest) {
        if (reply->error() != QNetworkReply::NoError) {
            setStatus("Installer download failed: " + reply->errorString());
            releaseTable_->setEnabled(true);
            reply->deleteLater();
            return;
        }
        const auto actualHash = QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
        if (data.size() != pendingDownloadSize_ || actualHash.compare(pendingDownloadHash_, Qt::CaseInsensitive) != 0) {
            setStatus("Integrity check failed. The downloaded installer was discarded.");
            QMessageBox::critical(this, "Installer Verification Failed",
                                  "The installer SHA-256 does not match the signed release manifest.");
            releaseTable_->setEnabled(true);
            reply->deleteLater();
            return;
        }
        QSaveFile file(pendingDownloadPath_);
        if (!file.open(QIODevice::WriteOnly) || file.write(data) != data.size() || !file.commit()) {
            setStatus("Could not save the verified installer.");
            QMessageBox::warning(this, "Cache Write Failed", "The verified installer could not be saved in the Weaver Manager cache.");
            releaseTable_->setEnabled(true);
            reply->deleteLater();
            return;
        }
        QFile cachedInstaller(pendingDownloadPath_);
        if (!cachedInstaller.open(QIODevice::ReadOnly)
            || cachedInstaller.size() != pendingDownloadSize_
            || QCryptographicHash::hash(cachedInstaller.readAll(), QCryptographicHash::Sha256)
                   .toHex().compare(pendingDownloadHash_, Qt::CaseInsensitive) != 0) {
            setStatus("Cached installer verification failed; installer was not launched.");
            QMessageBox::critical(this, "Cached Installer Verification Failed",
                                  "The file saved in the Weaver Manager cache does not match the signed release manifest. It was not launched.");
            releaseTable_->setEnabled(true);
            reply->deleteLater();
            return;
        }
        QString launchError;
        if (launchInstaller(pendingDownloadPath_, &launchError)) {
            setStatus("Installer verified and launched from " + pendingDownloadPath_);
        } else {
            setStatus("Installer verified, but could not be launched.");
            QMessageBox::warning(this, "Could Not Start Installer", launchError);
            releaseTable_->setEnabled(true);
        }
        reply->deleteLater();
        return;
    }
    const auto requestGeneration = reply->request()
        .attribute(static_cast<QNetworkRequest::Attribute>(ReleaseSearchGenerationAttribute)).toULongLong();
    if (requestGeneration != releaseSearchGeneration_) {
        reply->deleteLater();
        return;
    }
    if (requestPage == LatestReleaseRequest) {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonParseError latestError;
            const auto latest = QJsonDocument::fromJson(data, &latestError);
            if (latestError.error == QJsonParseError::NoError && latest.isObject()) {
                const auto release = latest.object();
                latestReleaseId_ = release.value("id").toVariant().toLongLong();
            }
        }
        reply->deleteLater();
        requestReleasePage(1);
        return;
    }
    if (reply->error() != QNetworkReply::NoError) {
        if (requestPage < 0) {
            --pendingManifests_;
            setStatus("A signed release metadata file could not be downloaded: " + reply->errorString());
            reply->deleteLater();
            if (pendingManifests_ == 0) {
                for (int index = 0; index < releaseManifests_.size(); ++index) processReleaseManifest(index);
                finishReleaseSearch();
            }
            return;
        }
        retryReleasePage(requestPage, reply->errorString());
        reply->deleteLater();
        return;
    }
    if (requestPage < 0) {
        const int encoded = -requestPage - 1;
        const int releaseIndex = encoded / 2;
        const bool isSignature = (encoded % 2) != 0;
        if (isSignature) downloadedSignatures_.insert(releaseIndex, data);
        else downloadedManifests_.insert(releaseIndex, data);
        --pendingManifests_;
        reply->deleteLater();
        if (pendingManifests_ == 0) {
            for (int index = 0; index < releaseManifests_.size(); ++index) processReleaseManifest(index);
            finishReleaseSearch();
        }
        return;
    }

    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isArray()) {
        setStatus("GitHub returned an invalid release catalog.");
        reply->deleteLater();
        return;
    }
    const auto pageItems = document.array();
    releasePageRetryCount_ = 0;
    for (const auto &item : pageItems) releaseItems_.append(item);
    if (pageItems.size() == 100) {
        reply->deleteLater();
        requestReleasePage(requestPage + 1);
        return;
    }
    pruneReleaseCache();
    fetchReleaseManifests();
    reply->deleteLater();
}

void MainWindow::fetchReleaseManifests() {
    if (QByteArray(WEAVER_RELEASE_PUBLIC_KEY_BASE64).isEmpty()) {
        setStatus("Release verification key is not configured. Set WEAVER_RELEASE_PUBLIC_KEY_BASE64 and rebuild.");
        return;
    }
    for (const auto releaseValue : releaseItems_) {
        const auto release = releaseValue.toObject();
        if (release.value("draft").toBool() || !release.value("immutable").toBool()) continue;
        QString manifestUrl;
        QString signatureUrl;
        for (const auto assetValue : release.value("assets").toArray()) {
            const auto asset = assetValue.toObject();
            const auto name = asset.value("name").toString();
            if (name == "weaver-manager-manifest.json") manifestUrl = asset.value("browser_download_url").toString();
            else if (name == "weaver-manager-manifest.json.sig") signatureUrl = asset.value("browser_download_url").toString();
        }
        if (manifestUrl.isEmpty() || signatureUrl.isEmpty()) continue;

        const int index = releaseManifests_.size();
        releaseManifests_.append(release);
        for (int kind = 0; kind < 2; ++kind) {
            QNetworkRequest request(QUrl(kind == 0 ? manifestUrl : signatureUrl));
            request.setTransferTimeout(ReleaseRequestTimeoutMs);
            request.setHeader(QNetworkRequest::UserAgentHeader, "WeaverManager/" WEAVER_MANAGER_VERSION);
            request.setAttribute(QNetworkRequest::User, -(index * 2 + kind + 1));
            request.setAttribute(static_cast<QNetworkRequest::Attribute>(ReleaseSearchGenerationAttribute),
                                 QVariant::fromValue<qulonglong>(releaseSearchGeneration_));
            network_.get(request);
            ++pendingManifests_;
        }
    }
    if (pendingManifests_ == 0) {
        finishReleaseSearch();
        return;
    }
    setStatus(QString("Reading signed manifests from %1 releases…").arg(pendingManifests_ / 2));
}

// Add only platform-compatible assets whose release manifest signature is valid.
void MainWindow::processReleaseManifest(int index) {
    if (!downloadedManifests_.contains(index) || !downloadedSignatures_.contains(index)) return;
    const auto manifestBytes = downloadedManifests_.value(index);
    if (!verifyEd25519Manifest(manifestBytes, downloadedSignatures_.value(index))) return;

    QJsonParseError manifestError;
    const auto manifest = QJsonDocument::fromJson(manifestBytes, &manifestError);
    if (manifestError.error != QJsonParseError::NoError || !manifest.isObject()) return;
    const auto release = releaseManifests_.at(index).toObject();
    if (!release.value("immutable").toBool()) return;
    if (release.value("tag_name").toString().isEmpty()) {
        qWarning() << "GitHub release is missing its tag:" << release.value("id");
        return;
    }
    const auto root = manifest.object();
    if (root.value("schema_version").toInt() != 1 || !root.value("artifacts").isArray()) return;

    const auto assets = release.value("assets").toArray();
    for (const auto artifactValue : root.value("artifacts").toArray()) {
        const auto artifact = artifactValue.toObject();
        const auto assetName = artifact.value("asset").toString();
        QString edition;
        QString version;
        QString platform;
        auto sha256 = artifact.value("sha256").toString().trimmed();
        if (sha256.startsWith("sha256:", Qt::CaseInsensitive)) sha256.remove(0, 7);
        if (!parseInstallerName(assetName, &edition, &version, &platform)
            || platform != currentPlatformId()
            || !QRegularExpression("^[A-Fa-f0-9]{64}$").match(sha256).hasMatch()) continue;
        for (const auto assetValue : assets) {
            const auto asset = assetValue.toObject();
            if (asset.value("name").toString() != assetName) continue;
            const auto assetSize = asset.value("size").toVariant().toLongLong();
            if (assetSize <= 0) continue;
            catalogEntries_.append(QJsonObject{
                {"version", version}, {"edition", edition},
                {"release_id", release.value("id")},
                {"release_tag", release.value("tag_name").toString()},
                {"prerelease", release.value("prerelease").toBool()},
                {"published", release.value("published_at").toString().left(10)},
                {"asset", assetName},
                {"url", asset.value("browser_download_url").toString()},
                {"size", assetSize},
                {"sha256", sha256}
            });
            break;
        }
    }
}

void MainWindow::pruneReleaseCache() {
    QSet<QString> activeReleaseEntries;
    QSet<QString> migratedEntries;
    QSet<QString> activeInstallerAssets;
    for (const auto &releaseValue : releaseItems_) {
        const auto release = releaseValue.toObject();
        const auto tag = release.value("tag_name").toString();
        for (const auto &assetValue : release.value("assets").toArray()) {
            const auto assetName = assetValue.toObject().value("name").toString();
            QString edition;
            QString version;
            QString platform;
            if (!parseInstallerName(assetName, &edition, &version, &platform)
                || platform != currentPlatformId()) continue;
            activeInstallerAssets.insert(assetName);
            if (tag.isEmpty()) {
                qWarning() << "GitHub release is missing its tag:" << release.value("id");
                continue;
            }
            const auto key = releaseEntryKey(tag, version, edition);
            activeReleaseEntries.insert(key);
            if (seenCacheFormat_ != SeenCacheFormat::Tag) {
                const auto oldKey = seenCacheFormat_ == SeenCacheFormat::ReleaseId
                    ? releaseEntryKey(QString::number(release.value("id").toVariant().toLongLong()), version, edition)
                    : version + QLatin1Char('|') + edition;
                if (seenReleaseEntriesAtSearchStart_.contains(oldKey)) migratedEntries.insert(key);
            }
        }
    }
    if (seenCacheFormat_ != SeenCacheFormat::Tag) seenReleaseEntriesAtSearchStart_ = migratedEntries;

    const auto cacheFilePath = seenVersionsCacheFilePath();
    if (!cacheFilePath.isEmpty()) {
        QSettings settings(cacheFilePath, QSettings::IniFormat);
        const auto seenEntriesKey = QStringLiteral("updates/seenWeaverReleaseTags");
        if (settings.contains(seenEntriesKey)) {
            QStringList retainedEntries;
            for (const auto &entry : settings.value(seenEntriesKey).toStringList()) {
                if (activeReleaseEntries.contains(entry)) retainedEntries.append(entry);
            }
            settings.setValue(seenEntriesKey, retainedEntries);
            settings.sync();
            if (settings.status() != QSettings::NoError)
                qWarning() << "Could not prune the seen Weaver releases cache";
        }
    }

    const auto cacheRoot = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (cacheRoot.isEmpty()) return;
    QDir installerCache(QDir(cacheRoot).filePath("verified-installers"));
    if (!installerCache.exists()) return;
    const auto cachedFiles = installerCache.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const auto &cachedFile : cachedFiles) {
        if (!activeInstallerAssets.contains(cachedFile.fileName()) && !QFile::remove(cachedFile.filePath()))
            qWarning() << "Could not remove cached installer for a deleted/unavailable release:" << cachedFile.filePath();
    }
}

void MainWindow::finishReleaseSearch() {
    const QString seenEntriesKey = QStringLiteral("updates/seenWeaverReleaseTags");
    QSet<QString> seenEntries = seenCacheFormat_ == SeenCacheFormat::Tag
        ? QSet<QString>() : seenReleaseEntriesAtSearchStart_;

    newReleaseEntries_.clear();
    QStringList newlyAvailable;
    QSet<QString> notifiedEntries;
    for (const auto &entryValue : catalogEntries_) {
        const auto entry = entryValue.toObject();
        const QString version = entry.value("version").toString();
        const QString edition = entry.value("edition").toString();
        const QString key = releaseEntryKey(entry.value("release_tag").toString(), version, edition);
        if (!seenReleaseEntriesAtSearchStart_.contains(key)) {
            newReleaseEntries_.insert(key);
            if ((!entry.value("prerelease").toBool() || weaverPrereleaseNotificationsAction_->isChecked())
                && !notifiedEntries.contains(key)) {
                const auto tag = entry.value("release_tag").toString();
                newlyAvailable.append(QStringLiteral("Weaver %1 (%2)%3%4")
                                          .arg(version, edition, entry.value("prerelease").toBool()
                                               ? QStringLiteral(" - Pre-Release") : QString(),
                                               tag.isEmpty() ? QString() : QStringLiteral(" [%1]").arg(tag)));
                notifiedEntries.insert(key);
            }
        }
        seenEntries.insert(key);
    }
    const auto cacheFilePath = seenVersionsCacheFilePath();
    if (!cacheFilePath.isEmpty()) {
        QSettings settings(cacheFilePath, QSettings::IniFormat);
        migrateSeenVersionsCache(settings);
        for (const auto &seenEntry : settings.value(seenEntriesKey).toStringList())
            seenEntries.insert(seenEntry);
        QStringList savedEntries = seenEntries.values();
        savedEntries.sort();
        settings.setValue(seenEntriesKey, savedEntries);
        settings.sync();
        if (settings.status() != QSettings::NoError)
            qWarning() << "Could not save the seen Weaver releases cache";
    } else {
        qWarning() << "Could not access the settings directory for the seen Weaver releases cache";
    }

    populateReleaseTable();
    if (weaverNotificationsAction_->isChecked() && !newlyAvailable.isEmpty()) {
        QMessageBox messageBox(QMessageBox::Information, QStringLiteral("New Weaver versions available"),
                               QStringLiteral("New versions were found:\n\n%1")
                                   .arg(newlyAvailable.join(QLatin1Char('\n'))),
                               QMessageBox::Ok, this);
        messageBox.setTextFormat(Qt::PlainText);
        auto *disableCheckBox = new QCheckBox("Do not show new Weaver release notifications again", &messageBox);
        messageBox.setCheckBox(disableCheckBox);
        messageBox.exec();
        if (disableCheckBox->isChecked()) weaverNotificationsAction_->setChecked(false);
    }
}

void MainWindow::populateReleaseTable() {
    releaseTable_->setRowCount(0);
    for (const auto entryValue : catalogEntries_) {
        const auto entry = entryValue.toObject();
        const int row = releaseTable_->rowCount();
        releaseTable_->insertRow(row);
        const QStringList columns{entry.value("version").toString(), entry.value("edition").toString(),
                                  entry.value("published").toString(), entry.value("asset").toString()};
        for (int column = 0; column < columns.size(); ++column)
            releaseTable_->setItem(row, column + 2, new ReleaseTableWidgetItem(columns.at(column)));
        for (int column = 2; column < 6; ++column)
            releaseTable_->item(row, column)->setTextAlignment(Qt::AlignCenter);
        releaseTable_->item(row, 5)->setData(Qt::UserRole, entry.value("url").toString());
        releaseTable_->item(row, 5)->setData(Qt::UserRole + 1, entry.value("sha256").toString());
        releaseTable_->item(row, 5)->setData(Qt::UserRole + 2, entry.value("size").toVariant().toLongLong());
        const bool prerelease = entry.value("prerelease").toBool();
        const bool latest = !prerelease && entry.value("release_id").toVariant().toLongLong() == latestReleaseId_;
        const QString releaseLabel = prerelease ? "Pre-Release" : latest ? "Latest" : "Stable";
        auto *statusItem = new ReleaseTableWidgetItem(releaseLabel);
        statusItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setData(Qt::UserRole, static_cast<int>(prerelease ? ReleaseStatus::Prerelease
                                                               : latest ? ReleaseStatus::Latest : ReleaseStatus::Stable));
        releaseTable_->setItem(row, 6, statusItem);
        const auto tag = entry.value("release_tag").toString();
        auto *tagItem = new ReleaseTableWidgetItem(tag);
        tagItem->setTextAlignment(Qt::AlignCenter);
        releaseTable_->setItem(row, 7, tagItem);

        auto *newItem = new ReleaseTableWidgetItem;
        newItem->setTextAlignment(Qt::AlignCenter);
        newItem->setData(Qt::UserRole + 1, releaseEntryKey(tag, entry.value("version").toString(),
                                                          entry.value("edition").toString()));
        releaseTable_->setItem(row, 1, newItem);

        auto *installButton = new QPushButton("Install", releaseTable_);
        installButton->setToolTip("Download, verify, and launch this release's installer");
        auto *buttonContainer = releaseTable_->cellWidget(row, 0);
        if (!buttonContainer) buttonContainer = new QWidget(releaseTable_);
        buttonContainer->setObjectName("releaseInstallButtonCell");
        auto *buttonLayout = new QHBoxLayout(buttonContainer);
        buttonLayout->setContentsMargins(4, 2, 4, 2);
        buttonLayout->addStretch();
        buttonLayout->addWidget(installButton);
        buttonLayout->addStretch();
        releaseTable_->setCellWidget(row, 0, buttonContainer);
        connect(installButton, &QPushButton::clicked, this, [this, entry] {
            installVerifiedInstaller(entry);
        });
    }
#if ENABLE_TEST
    for (const auto &path : localTestInstallers_) {
        const QFileInfo installerInfo(path);
        if (!installerInfo.isFile()) continue;
        QString edition;
        QString version;
        QString platform;
        if (!parseInstallerName(installerInfo.fileName(), &edition, &version, &platform)
            || platform != currentPlatformId()) continue;
        const int row = releaseTable_->rowCount();
        releaseTable_->insertRow(row);
        const QStringList columns{version, edition, "Local", installerInfo.fileName()};
        for (int column = 0; column < columns.size(); ++column) {
            auto *item = new ReleaseTableWidgetItem(columns.at(column));
            item->setTextAlignment(Qt::AlignCenter);
            releaseTable_->setItem(row, column + 2, item);
        }
        auto *statusItem = new ReleaseTableWidgetItem("Test");
        statusItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setData(Qt::UserRole, static_cast<int>(ReleaseStatus::Test));
        releaseTable_->setItem(row, 6, statusItem);
        auto *tagItem = new ReleaseTableWidgetItem("—");
        tagItem->setTextAlignment(Qt::AlignCenter);
        releaseTable_->setItem(row, 7, tagItem);

        auto *installButton = new QPushButton("Install", releaseTable_);
        installButton->setToolTip("Launch a local test installer without signed GitHub manifest verification");
        auto *buttonContainer = new QWidget(releaseTable_);
        auto *buttonLayout = new QHBoxLayout(buttonContainer);
        buttonLayout->setContentsMargins(4, 2, 4, 2);
        buttonLayout->addStretch();
        buttonLayout->addWidget(installButton);
        buttonLayout->addStretch();
        releaseTable_->setCellWidget(row, 0, buttonContainer);
        connect(installButton, &QPushButton::clicked, this, [this, path] {
            installLocalTestInstaller(path);
        });
    }
#endif
    refreshReleaseCacheStatuses();
    releaseTable_->sortItems(releaseTable_->horizontalHeader()->sortIndicatorSection(),
                             releaseTable_->horizontalHeader()->sortIndicatorOrder());
    applyReleaseFilter();
    if (releaseTable_->rowCount() == 0) {
        setStatus("No releases with a valid signed manifest were found for " + platformLabel() + ".");
    } else {
        setStatus(QString("Showing %1 verified release entries for %2.")
                  .arg(releaseTable_->rowCount()).arg(platformLabel()));
    }
}

void MainWindow::applyReleaseFilter() {
    for (int row = 0; row < releaseTable_->rowCount(); ++row) {
        const auto *status = releaseTable_->item(row, 6);
        const bool prerelease = status && status->data(Qt::UserRole).toInt() == static_cast<int>(ReleaseStatus::Prerelease);
        releaseTable_->setRowHidden(row, prerelease && !showPrereleasesCheckBox_->isChecked());
    }
}

#if ENABLE_TEST
void MainWindow::addLocalTestInstaller() {
    QDialog dialog(this);
    dialog.setWindowTitle("Add Local Test Installer");
    dialog.setModal(true);
    dialog.resize(540, 210);
    auto *layout = new QVBoxLayout(&dialog);
    layout->addWidget(new QLabel(
        "Select a local Weaver installer to add as a Test release. "
        "It will not be verified against a signed GitHub manifest.", &dialog));

    auto *pathLayout = new QHBoxLayout;
    auto *pathEdit = new QLineEdit(&dialog);
    pathEdit->setPlaceholderText("Installer path");
    auto *browseButton = new QPushButton("Browse…", &dialog);
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(browseButton);
    layout->addLayout(pathLayout);

    auto *dropLabel = new QLabel(&dialog);
    dropLabel->setText("Drag and drop an installer file here");
    dropLabel->setAlignment(Qt::AlignCenter);
    dropLabel->setMinimumHeight(72);
    dropLabel->setFrameShape(QFrame::StyledPanel);
    layout->addWidget(dropLabel);

    connect(browseButton, &QPushButton::clicked, &dialog, [this, pathEdit] {
        const auto path = QFileDialog::getOpenFileName(
            this, "Select Test Installer", pathEdit->text(),
#if defined(Q_OS_WIN)
            "Installer packages (*.msi);;All files (*.*)"
#elif defined(Q_OS_MACOS)
            "Installer packages (*.pkg *.dmg);;All files (*)"
#else
            "Installer packages (*.deb *.rpm);;All files (*)"
#endif
        );
        if (!path.isEmpty()) pathEdit->setText(path);
    });

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText("Add Installer");
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    const auto isValidInstaller = [](const QString &path) {
        QString edition, version, platform;
        const QFileInfo info(path);
#if defined(Q_OS_WIN)
        const QStringList extensions = {"msi"};
#elif defined(Q_OS_MACOS)
        const QStringList extensions = {"pkg", "dmg"};
#else
        const QStringList extensions = {"deb", "rpm"};
#endif
        return info.isFile() && extensions.contains(info.suffix().toLower())
            && parseInstallerName(info.fileName(), &edition, &version, &platform)
            && platform == currentPlatformId();
    };
    auto *dropFilter = new InstallerDropFilter(pathEdit, isValidInstaller, &dialog);
    dialog.setAcceptDrops(true);
    pathEdit->setAcceptDrops(true);
    dropLabel->setAcceptDrops(true);
    for (QWidget *widget : {static_cast<QWidget *>(&dialog), static_cast<QWidget *>(pathEdit),
                            static_cast<QWidget *>(dropLabel)}) {
        widget->installEventFilter(dropFilter);
    }

    if (dialog.exec() != QDialog::Accepted) return;

    const QFileInfo installerInfo(QDir::cleanPath(pathEdit->text().trimmed()));
    const QStringList supportedExtensions = {
#if defined(Q_OS_WIN)
        "msi"
#elif defined(Q_OS_MACOS)
        "pkg", "dmg"
#else
        "deb", "rpm"
#endif
    };
    if (!installerInfo.isFile() || !installerInfo.isReadable()
        || !supportedExtensions.contains(installerInfo.suffix().toLower())) {
        QMessageBox::warning(this, "Invalid Test Installer",
                             "Select a readable installer package supported on this platform.");
        return;
    }

    const auto installerPath = installerInfo.absoluteFilePath();
    QString edition;
    QString version;
    QString platform;
    if (!parseInstallerName(installerInfo.fileName(), &edition, &version, &platform)
        || platform != currentPlatformId()) {
        QMessageBox::warning(this, "Invalid Test Installer Name",
                             "The installer filename must follow the Weaver-Edition-Version-OS-Architecture format for this platform.");
        return;
    }
    if (!localTestInstallers_.contains(installerPath)) localTestInstallers_.append(installerPath);
    populateReleaseTable();
    setStatus("Added local Test installer: " + installerInfo.fileName());
}

void MainWindow::installLocalTestInstaller(const QString &path) {
    const QFileInfo installerInfo(path);
    if (!installerInfo.isFile() || !installerInfo.isReadable()) {
        QMessageBox::warning(this, "Test Installer Not Found",
                             "The local installer is missing or unreadable:\n" + path);
        localTestInstallers_.removeAll(path);
        populateReleaseTable();
        return;
    }

    const auto answer = QMessageBox::warning(this, "Unverified Test Installer",
        "Launch this local installer?\n\nIt has not been verified using a signed GitHub manifest "
        "and may request administrator privileges.\n\n" + installerInfo.fileName(),
        QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (answer != QMessageBox::Yes) return;

    QString errorMessage;
    releaseTable_->setEnabled(false);
    if (!launchInstaller(installerInfo.absoluteFilePath(), &errorMessage)) {
        releaseTable_->setEnabled(true);
        QMessageBox::warning(this, "Could Not Start Test Installer", errorMessage);
        setStatus("Local Test installer could not be started.");
        return;
    }
    setStatus("Launched local Test installer: " + installerInfo.fileName());
}
#endif

// Re-query GitHub at install time so a stale catalog cannot authorize an asset.
void MainWindow::installVerifiedInstaller(const QJsonObject &entry) {
    const auto releaseId = entry.value("release_id").toVariant().toLongLong();
    if (releaseId <= 0) {
        QMessageBox::warning(this, "Could Not Verify Weaver Release",
                             "This catalog entry does not contain a valid GitHub release ID. Refresh the release list and try again.");
        return;
    }
    QNetworkRequest request(QUrl(QString("https://api.github.com/repos/%1/releases/%2")
                                 .arg(WEAVER_REPOSITORY).arg(releaseId)));
    request.setTransferTimeout(ReleaseRequestTimeoutMs);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
    request.setRawHeader("Cache-Control", "no-cache, no-store");
    request.setRawHeader("Pragma", "no-cache");
    auto url = request.url();
    QUrlQuery query(url);
    query.addQueryItem("_fresh", QUuid::createUuid().toString(QUuid::WithoutBraces));
    url.setQuery(query);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "WeaverManager/" WEAVER_MANAGER_VERSION);
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setAttribute(static_cast<QNetworkRequest::Attribute>(ReleaseValidationEntryAttribute),
                         QVariant::fromValue(entry));
    releaseValidationNetwork_.get(request);
}

void MainWindow::continueVerifiedInstallerInstall(const QJsonObject &entry) {
    const auto version = entry.value("version").toString();
    const auto edition = entry.value("edition").toString();
    const auto assetName = entry.value("asset").toString();
    const auto extension = QFileInfo(assetName).suffix().toLower();
    const QStringList supportedExtensions = {
#if defined(Q_OS_WIN)
        "msi"
#elif defined(Q_OS_MACOS)
        "pkg", "dmg"
#else
        "deb", "rpm"
#endif
    };
    if (!supportedExtensions.contains(extension)) {
        QMessageBox::warning(this, "Unsupported Installer",
                             "This release uses an installer format that Weaver Manager cannot launch on this platform.");
        return;
    }
    const auto answer = QMessageBox::question(this, "Install Weaver Version",
        QString("Install Weaver %1 (%2)?\n\nThe release manifest signature and installer SHA-256 will be verified before the installer is launched. The installer may request administrator privileges.")
            .arg(version, edition), QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (answer != QMessageBox::Yes) return;

    const auto cacheRoot = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    const auto installCache = QDir(cacheRoot).filePath("verified-installers");
    if (cacheRoot.isEmpty() || assetName.isEmpty() || QFileInfo(assetName).fileName() != assetName
        || !QDir().mkpath(installCache)) {
        QMessageBox::warning(this, "Cannot Prepare Installer", "The release installer filename or cache location is invalid.");
        return;
    }
    downloadInstaller(entry.value("url").toString(), assetName, entry.value("sha256").toString().toLatin1(),
                      entry.value("size").toVariant().toLongLong());
}

void MainWindow::downloadInstaller(const QString &url, const QString &assetName, const QByteArray &sha256,
                                   qint64 size) {
    const QUrl downloadUrl(url);
    if (downloadUrl.scheme() != "https" || downloadUrl.host() != "github.com"
        || sha256.size() != 64 || !QRegularExpression("^[A-Fa-f0-9]{64}$").match(QString::fromLatin1(sha256)).hasMatch()
        || size <= 0 || assetName.isEmpty() || QFileInfo(assetName).fileName() != assetName) {
        QMessageBox::warning(this, "Cannot Verify Installer", "The release is missing a valid GitHub download URL, filename, size, or SHA-256.");
        pendingDownloadPath_.clear();
        return;
    }
    pendingDownloadHash_ = sha256;
    pendingDownloadSize_ = size;
    const auto cacheRoot = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    const auto cacheDirectory = QDir(cacheRoot).filePath("verified-installers");
    if (cacheRoot.isEmpty() || !QDir().mkpath(cacheDirectory)) {
        QMessageBox::warning(this, "Cannot Prepare Installer", "The Weaver Manager cache directory could not be created.");
        return;
    }
    pendingDownloadPath_ = QDir(cacheDirectory).filePath(assetName);

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::UserAgentHeader, "WeaverManager/" WEAVER_MANAGER_VERSION);
    request.setAttribute(QNetworkRequest::User, DownloadRequest);
    setStatus("Downloading installer before verification…");
    releaseTable_->setEnabled(false);
    network_.get(request);
}

bool MainWindow::launchInstaller(const QString &path, QString *errorMessage) {
    installerLogPath_.clear();
    const QFileInfo installerInfo(path);
    const auto absolutePath = installerInfo.absoluteFilePath();
    const auto extension = installerInfo.suffix().toLower();
    if (!installerInfo.isFile() || !installerInfo.isReadable() || installerInfo.size() <= 0) {
        *errorMessage = "The installer is missing or unreadable at:\n" + absolutePath;
        return false;
    }
    QString program;
    QStringList arguments;
    QString nativeArguments;
#if defined(Q_OS_WIN)
    if (extension == "msi") {
        QFile package(absolutePath);
        if (!package.open(QIODevice::ReadOnly) || package.read(8) != QByteArray::fromHex("d0cf11e0a1b11ae1")) {
            *errorMessage = "The file does not have the expected Windows Installer package signature:\n" + absolutePath;
            return false;
        }
        const auto windowsDir = qEnvironmentVariable("WINDIR");
        const auto systemMsiexec = QDir(windowsDir).filePath("System32/msiexec.exe");
        program = QFileInfo::exists(systemMsiexec) ? systemMsiexec : QStandardPaths::findExecutable("msiexec.exe");
        if (program.isEmpty()) {
            *errorMessage = "Windows Installer (msiexec.exe) could not be found.";
            return false;
        }
        const auto nativePath = QDir::toNativeSeparators(absolutePath);
        const auto installRoot = QDir::toNativeSeparators(
            QDir(managerProgramInstallRoot()).filePath("installed_versions"));
        const auto appDataDirectory = QDir::toNativeSeparators(managerInstalledVersionsDirectory());
        if (appDataDirectory.isEmpty()) {
            *errorMessage = "The Weaver Manager data folder could not be resolved.";
            return false;
        }
        const auto logDirectory = managerLogDirectory();
        if (logDirectory.isEmpty() || !QDir().mkpath(logDirectory)) {
            *errorMessage = "The Weaver Manager log directory could not be created.";
            return false;
        }
        const auto logPath = QDir::toNativeSeparators(QDir(logDirectory).filePath("msiexec-install.log"));
        installerLogPath_ = logPath;
        nativeArguments = QString("WEAVER_INSTALL_ROOT=\"%1\" APPDATADIR=\"%2\" WEAVER_MANAGER_INSTALL=1")
                              .arg(installRoot, appDataDirectory);
        arguments = {"/i", nativePath, "/L*V", logPath};
        qInfo().noquote() << "Starting msiexec; package:" << nativePath
                          << "program root:" << installRoot << "application data root:" << appDataDirectory
                          << "verbose log:" << logPath
                          << "property:" << nativeArguments;
    } else {
        *errorMessage = "Weaver Manager requires an MSI installer to select its managed installation directory.";
        return false;
    }
#elif defined(Q_OS_MACOS)
    if (extension == "pkg" || extension == "dmg") {
        program = "/usr/bin/open";
        arguments = {absolutePath};
    } else {
        *errorMessage = "Unsupported macOS installer format.";
        return false;
    }
#else
    if (extension == "deb") {
        program = "pkexec";
        arguments = {"apt-get", "install", "-y", absolutePath};
    } else if (extension == "rpm") {
        program = "pkexec";
        arguments = {"dnf", "install", "-y", absolutePath};
    } else {
        *errorMessage = "Unsupported Linux installer format.";
        return false;
    }
#endif
    installerProcess_.setProgram(program);
    installerProcess_.setArguments(arguments);
    installerProcess_.setProcessChannelMode(QProcess::SeparateChannels);
#if defined(Q_OS_WIN)
    installerProcess_.setNativeArguments(nativeArguments);
#endif
    installerProcess_.start();
    if (!installerProcess_.waitForStarted(5000)) {
        qCritical() << "Could not start installer:" << program << arguments << installerProcess_.errorString();
        *errorMessage = "The operating system could not start the installer:\n" + absolutePath;
#if defined(Q_OS_WIN)
        if (extension == "msi")
            *errorMessage += "\n\nWindows Installer diagnostic log (if created):\n"
                + QDir(managerLogDirectory()).filePath("msiexec-install.log");
#endif
        return false;
    }
    qInfo() << "Installer process started:" << program << arguments;
    return true;
}

void MainWindow::removeInstallerCache() {
    const auto cacheRoot = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (cacheRoot.isEmpty()) return;
    QDir installerCache(QDir(cacheRoot).filePath("verified-installers"));
    if (installerCache.exists() && !installerCache.removeRecursively())
        QMessageBox::warning(this, "Cache Cleanup Failed",
                             "Weaver Manager could not remove all cached installer files:\n" + installerCache.path());
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (installerProcess_.state() != QProcess::NotRunning) {
        if (!closeWhenInstallerFinishes_) {
            closeWhenInstallerFinishes_ = true;
            QMessageBox::information(this, "Installer Still Running",
                "Weaver Manager will close and remove its installer cache after the installer finishes.");
        }
        event->ignore();
        return;
    }
    removeInstallerCache();
    QMainWindow::closeEvent(event);
}

// Rebuild the installed-version cards from every supported installation root.
int MainWindow::loadInstallations() {
    while (auto *item = installedListLayout_->takeAt(0)) {
        if (auto *widget = item->widget()) widget->deleteLater();
        delete item;
    }
    int count = 0;
    QSet<QString> listedInstallations;
    for (const auto &versionsRoot : weaverInstallationRoots()) {
        QDir versionsDirectory(versionsRoot);
        if (!versionsDirectory.exists()) continue;
        const auto versionDirectories = versionsDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot,
                                                                     QDir::Name | QDir::Reversed);
        for (const auto &version : versionDirectories) {
            for (const auto &edition : {QStringLiteral("Desktop"), QStringLiteral("Server")}) {
                const auto installationKey = version + QLatin1Char('|') + edition;
                if (listedInstallations.contains(installationKey)) continue;
                const auto root = QDir(versionsRoot).filePath(QString("%1/%2").arg(version, edition));
                const auto cliPath = QDir(root).filePath("bin/" + weaverCliExecutableName());
                if (!QFileInfo(cliPath).isFile()) continue;
                listedInstallations.insert(installationKey);

                const auto cleanRoot = QDir::cleanPath(root);
                auto *card = new QFrame(installedListContent_);
                card->setFrameShape(QFrame::StyledPanel);
                auto *cardLayout = new QVBoxLayout(card);
                auto *headerLayout = new QHBoxLayout;
                auto *title = new QLabel(QString("Weaver %1 — %2").arg(version, edition), card);
                QFont titleFont = title->font();
                titleFont.setPointSize(titleFont.pointSize() + 3);
                titleFont.setBold(true);
                title->setFont(titleFont);
                auto *expandButton = new QPushButton("▼ Open", card);
                expandButton->setCheckable(true);
                expandButton->setFixedSize(112, 36);
                expandButton->setStyleSheet("QPushButton { font-size: 11pt; padding: 6px 12px; background-color: #1976d2; color: white; border: none; border-radius: 4px; text-align: center; } QPushButton:hover { background-color: #1565c0; }");
                auto *modifyButton = new QPushButton("Modify", card);
                modifyButton->setFixedSize(112, 36);
                modifyButton->setStyleSheet("QPushButton { font-size: 11pt; background-color: #1976d2; color: white; padding: 6px 12px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #1565c0; }");
                auto *uninstallButton = new QPushButton("Uninstall", card);
                uninstallButton->setFixedSize(112, 36);
                uninstallButton->setStyleSheet("QPushButton { font-size: 11pt; background-color: #d32f2f; color: white; padding: 6px 12px; border: none; border-radius: 4px; } QPushButton:hover { background-color: #b71c1c; }");
#if !defined(Q_OS_WIN)
                modifyButton->hide();
                uninstallButton->hide();
#endif
                headerLayout->addWidget(title);
                headerLayout->addWidget(expandButton);
                headerLayout->addStretch();
                headerLayout->addWidget(modifyButton);
                headerLayout->addWidget(uninstallButton);
                cardLayout->addLayout(headerLayout);

                auto *actions = new QWidget(card);
                actions->setStyleSheet("QPushButton { font-size: 11pt; padding: 9px 14px; min-height: 36px; background-color: #00796b; color: white; border: none; border-radius: 4px; } QPushButton:hover { background-color: #00695c; }");
                auto *actionsLayout = new QHBoxLayout(actions);
                actionsLayout->setContentsMargins(0, 0, 0, 0);
                auto *folderButton = new QPushButton("Open installation folder", actions);
                auto *dataButton = new QPushButton("Open data folder", actions);
                auto *cliButton = new QPushButton("Open Weaver CLI", actions);
                auto *rButton = new QPushButton("Open R", actions);
                QPushButton *hubButton = nullptr;
                QPushButton *guiButton = nullptr;
                actionsLayout->addWidget(folderButton);
                actionsLayout->addWidget(dataButton);
                actionsLayout->addWidget(cliButton);
                actionsLayout->addWidget(rButton);
                if (edition == "Desktop") {
                    hubButton = new QPushButton("Open Weaver Hub", actions);
                    guiButton = new QPushButton("Open Weaver GUI", actions);
                    actionsLayout->addWidget(hubButton);
                    actionsLayout->addWidget(guiButton);
                }
                actionsLayout->addStretch();
                actions->hide();
                cardLayout->addWidget(actions);
                installedListLayout_->addWidget(card);

                connect(expandButton, &QPushButton::toggled, actions, [actions, expandButton](bool expanded) {
                    actions->setVisible(expanded);
                    expandButton->setText(expanded ? "▲ Close" : "▼ Open");
                });
                connect(folderButton, &QPushButton::clicked, this, [this, cleanRoot] {
                    if (!ensureInstallationAvailable(cleanRoot)) return;
                    openInstallationRoot(cleanRoot);
                });
                connect(dataButton, &QPushButton::clicked, this, [this, version, edition, cleanRoot] {
                    if (!ensureInstallationAvailable(cleanRoot)) return;
                    openVersionDataFolder(version, edition);
                });
                connect(cliButton, &QPushButton::clicked, this, [this, cleanRoot] {
                    if (!ensureInstallationAvailable(cleanRoot)) return;
                    openWeaverCLI(cleanRoot);
                });
                connect(rButton, &QPushButton::clicked, this, [this, cleanRoot] {
                    if (!ensureInstallationAvailable(cleanRoot)) return;
                    openRForInstallation(cleanRoot);
                });
                if (hubButton) {
                    connect(hubButton, &QPushButton::clicked, this, [this, edition, cleanRoot] {
                        if (!ensureInstallationAvailable(cleanRoot)) return;
                        openHubForInstallation(edition, cleanRoot);
                    });
                }
                if (guiButton) {
                    connect(guiButton, &QPushButton::clicked, this, [this, edition, cleanRoot] {
                        if (!ensureInstallationAvailable(cleanRoot)) return;
                        openWeaverGUI(edition, cleanRoot);
                    });
                }
                connect(uninstallButton, &QPushButton::clicked, this, [this, version, edition] {
                    const auto root = findWeaverInstallationRoot(version, edition);
                    if (!ensureInstallationAvailable(root)) return;
                    uninstallInstallation(version, edition);
                });
                connect(modifyButton, &QPushButton::clicked, this, [this, version, edition] {
                    const auto root = findWeaverInstallationRoot(version, edition);
                    if (!ensureInstallationAvailable(root)) return;
                    modifyInstallation(version, edition);
                });
                ++count;
            }
        }
    }
    if (count == 0)
        installedListLayout_->addWidget(new QLabel("No Weaver installed versions were found.", installedListContent_));
    installedListLayout_->addStretch();
    return count;
}

void MainWindow::refreshInstalledVersions() {
    const int count = loadInstallations();
    refreshReleaseCacheStatuses();
    setStatus(QString("Found %1 Weaver installed version(s).").arg(count));
}

void MainWindow::refreshReleaseCacheStatuses() {
    const auto colors = releaseColors();
    for (int row = 0; row < releaseTable_->rowCount(); ++row) {
        if (auto *statusItem = releaseTable_->item(row, 6)) {
            const auto status = static_cast<ReleaseStatus>(statusItem->data(Qt::UserRole).toInt());
            switch (status) {
            case ReleaseStatus::Latest:
                statusItem->setForeground(colors.installedText);
                statusItem->setBackground(colors.installedBackground);
                break;
            case ReleaseStatus::Prerelease:
                statusItem->setForeground(colors.prereleaseText);
                statusItem->setBackground(colors.prereleaseBackground);
                break;
            case ReleaseStatus::Test:
                statusItem->setForeground(colors.testText);
                statusItem->setBackground(colors.testBackground);
                break;
            case ReleaseStatus::Stable:
                statusItem->setForeground(colors.stableText);
                statusItem->setBackground(colors.stableBackground);
                break;
            }
        }
        const auto *versionItem = releaseTable_->item(row, 2);
        const auto *editionItem = releaseTable_->item(row, 3);
        auto *cacheStatusItem = releaseTable_->item(row, 1);
        if (!versionItem || !editionItem || !cacheStatusItem) continue;

        const auto version = versionItem->text();
        const auto edition = editionItem->text();
        const bool installed = !findWeaverInstallationRoot(version, edition).isEmpty();
        const auto releaseKey = cacheStatusItem->data(Qt::UserRole + 1).toString();
        const bool isNew = newReleaseEntries_.contains(releaseKey);
        const bool highlighted = installed || isNew;
        const QColor rowBackground = installed ? colors.installedBackground : colors.newBackground;
        for (int column = 1; column < releaseTable_->columnCount(); ++column) {
            if (column == 6) continue;
            if (auto *item = releaseTable_->item(row, column)) {
                item->setBackground(highlighted ? QBrush(rowBackground) : QBrush());
                item->setForeground(highlighted ? QBrush(colors.rowText) : QBrush());
            }
        }

        if (auto *buttonContainer = releaseTable_->cellWidget(row, 0)) {
            buttonContainer->setStyleSheet(highlighted
                ? QStringLiteral("QWidget#releaseInstallButtonCell { background-color: %1; }").arg(rowBackground.name())
                : QString());
            if (auto *installButton = buttonContainer->findChild<QPushButton *>())
                installButton->setVisible(!installed);
        }

        auto font = cacheStatusItem->font();
        font.setBold(highlighted);
        cacheStatusItem->setFont(font);
        if (installed) {
            cacheStatusItem->setText("INSTALLED");
            cacheStatusItem->setForeground(colors.installedText);
            cacheStatusItem->setToolTip("This version and edition are installed by Weaver Manager.");
        } else if (isNew) {
            cacheStatusItem->setText("NEW");
            cacheStatusItem->setForeground(colors.newText);
            cacheStatusItem->setToolTip("Recently available; cleared after the next refresh or when Weaver Manager restarts.");
        } else {
            cacheStatusItem->setText({});
            cacheStatusItem->setToolTip({});
        }
    }
}

bool MainWindow::ensureInstallationAvailable(const QString &root) {
    const auto executable = QDir(root).filePath("bin/" + weaverCliExecutableName());
    if (QFileInfo(executable).isFile()) return true;

    QMessageBox warning(this);
    warning.setIcon(QMessageBox::Warning);
    warning.setWindowTitle("Weaver Version Uninstalled");
    warning.setText("This Weaver version is no longer available at its managed installation path.");
    auto *refreshButton = warning.addButton("Refresh Installed Versions", QMessageBox::AcceptRole);
    warning.exec();
    if (warning.clickedButton() == refreshButton) refreshInstalledVersions();
    return false;
}

void MainWindow::openInstallationRoot(const QString &root) {
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(root)))
        QMessageBox::warning(this, "Could Not Open Folder", "The installation folder could not be opened:\n" + root);
}

void MainWindow::openVersionDataFolder(const QString &version, const QString &edition) {
    const auto dataRoot = managerInstalledVersionsDirectory();
    if (dataRoot.isEmpty()) {
        QMessageBox::warning(this, "Data Folder Unavailable", "The Manager data directory could not be resolved.");
        return;
    }
    const auto dataFolder = QDir(QDir(dataRoot).filePath(version)).filePath(edition);
    if (!QDir(dataFolder).exists()) {
        QMessageBox::information(this, "Data Folder Not Found", "The version data folder does not exist:\n" + dataFolder);
        return;
    }
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(dataFolder)))
        QMessageBox::warning(this, "Could Not Open Folder", "The version data folder could not be opened:\n" + dataFolder);
}

void MainWindow::openWeaverCLI(const QString &root) {
    const auto executable = QDir(root).filePath("bin/" + weaverCliExecutableName());
    const auto workingDirectory = QDir(root).filePath("bin");
    if (!QFileInfo(executable).isFile()) {
        QMessageBox::warning(this, "Weaver CLI Not Installed",
                             "The Weaver CLI executable was not found:\n" + executable);
        return;
    }
#if defined(Q_OS_WIN)
    const auto commandProcessor = QStandardPaths::findExecutable("cmd.exe");
    if (commandProcessor.isEmpty()) {
        QMessageBox::warning(this, "Terminal Not Found", "The Windows command prompt (cmd.exe) could not be found.");
        return;
    }
    auto commandLine = QString("\"%1\" /K \"\"%2\" & pause\"")
                           .arg(QDir::toNativeSeparators(commandProcessor),
                                QDir::toNativeSeparators(executable));
    auto commandLineWide = commandLine.toStdWString();
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_SHOW;
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(commandProcessor.toStdWString().c_str(), commandLineWide.data(), nullptr, nullptr,
                        FALSE, CREATE_NEW_CONSOLE, nullptr,
                        QDir::toNativeSeparators(workingDirectory).toStdWString().c_str(),
                        &startup, &process)) {
        QMessageBox::warning(this, "Could Not Start Weaver CLI",
                             "A terminal could not be opened for:\n" + executable
                                 + "\n\nWindows error: " + QString::number(GetLastError()));
        return;
    }
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
#elif defined(Q_OS_MACOS)
    const auto osascript = QStandardPaths::findExecutable("osascript");
    if (osascript.isEmpty()) {
        QMessageBox::warning(this, "Terminal Not Found", "AppleScript (osascript) could not be found to open Terminal.");
        return;
    }
    const auto shellCommand = QString("cd %1; %2; cli_status=$?; printf '\\nWeaver CLI exited with status %s.\\n' \"$cli_status\"; exec \"${SHELL:-/bin/zsh}\"")
                                  .arg(shellQuoted(workingDirectory), shellQuoted(executable));
    QString escapedShellCommand = shellCommand;
    escapedShellCommand.replace(QLatin1Char('\\'), QStringLiteral("\\\\"));
    escapedShellCommand.replace(QLatin1Char('"'), QStringLiteral("\\\""));
    const auto appleScript = QString("tell application \"Terminal\" to do script \"%1\"\ntell application \"Terminal\" to activate")
                                 .arg(escapedShellCommand);
    if (!QProcess::startDetached(osascript, {"-e", appleScript}))
        QMessageBox::warning(this, "Could Not Start Weaver CLI",
                             "Terminal.app could not be opened for:\n" + executable);
#else
    const auto shellCommand = QString("cd %1; %2; cli_status=$?; printf '\\nWeaver CLI exited with status %s.\\n' \"$cli_status\"; exec \"${SHELL:-/bin/sh}\"")
                                  .arg(shellQuoted(workingDirectory), shellQuoted(executable));
    const QStringList terminalCandidates{
        QStringLiteral("x-terminal-emulator"), QStringLiteral("gnome-terminal"),
        QStringLiteral("kgx"), QStringLiteral("konsole"),
        QStringLiteral("xfce4-terminal"), QStringLiteral("xterm")
    };
    QString terminal;
    QString terminalName;
    for (const auto &candidate : terminalCandidates) {
        terminal = QStandardPaths::findExecutable(candidate);
        if (!terminal.isEmpty()) {
            terminalName = candidate;
            break;
        }
    }
    if (terminal.isEmpty()) {
        QMessageBox::warning(this, "Terminal Not Found",
                             "No supported terminal emulator was found. Install a terminal emulator and try again.");
        return;
    }

    QStringList arguments;
    if (terminalName == "gnome-terminal" || terminalName == "kgx")
        arguments = {"--", "sh", "-c", shellCommand};
    else if (terminalName == "konsole")
        arguments = {"-e", "sh", "-c", shellCommand};
    else if (terminalName == "xfce4-terminal")
        arguments = {"--command", "sh -c " + shellQuoted(shellCommand)};
    else
        arguments = {"-e", "sh", "-c", shellCommand};
    if (!QProcess::startDetached(terminal, arguments, workingDirectory))
        QMessageBox::warning(this, "Could Not Start Weaver CLI",
                             "A terminal could not be opened for:\n" + executable);
#endif
}

void MainWindow::openWeaverGUI(const QString &edition, const QString &root) {
    if (edition != "Desktop") {
        QMessageBox::warning(this, "Weaver GUI Unavailable",
                             "Weaver GUI is available only in the Desktop edition.");
        return;
    }

    const auto executable = QDir(root).filePath("bin/WeaverGUI.exe");
    if (!QFileInfo(executable).isFile()) {
        QMessageBox::warning(this, "Weaver Graphical Interface Not Installed",
                             "The 'Weaver graphical interface' feature is not installed for this version.\n\n"
                             "Use Modify to add the feature, then try again.");
        return;
    }
    if (!QProcess::startDetached(executable, {}, QDir(root).filePath("bin")))
        QMessageBox::warning(this, "Could Not Start Weaver GUI",
                             "WeaverGUI.exe could not be started from:\n" + executable);
}

void MainWindow::openRForInstallation(const QString &root) {
    const auto rLibrary = QDir(root).filePath("lib/R");
    if (!QDir(rLibrary).exists()) {
        QMessageBox::warning(this, "R Packages Not Installed",
            "This Weaver installation does not have the R packages feature installed.");
        return;
    }
#if defined(Q_OS_WIN)
    const auto rExecutable = findRguiExecutable();
#else
    auto rExecutable = QStandardPaths::findExecutable("Rgui.exe");
    if (rExecutable.isEmpty()) rExecutable = QStandardPaths::findExecutable("R");
#endif
    if (rExecutable.isEmpty()) {
        QMessageBox::warning(this, "R Not Found",
                             "Rgui.exe could not be found. Make sure R is installed and registered with Windows, "
                             "or add its bin folder to PATH.");
        return;
    }
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert("R_LIBS_USER", QDir::toNativeSeparators(rLibrary));
    environment.insert("R_LIBS", QDir::toNativeSeparators(rLibrary));
    environment.insert("R_LIBS_SITE", QString());
    QProcess process;
    process.setProcessEnvironment(environment);
    process.setProgram(rExecutable);
    process.setArguments({"--no-save", "--no-restore"});
    if (!process.startDetached())
        QMessageBox::warning(this, "Could Not Start R",
                             "R could not be started from:\n" + rExecutable);
}

void MainWindow::openHubForInstallation(const QString &edition, const QString &root) {
    if (edition != "Desktop") {
        QMessageBox::warning(this, "Weaver Hub Unavailable",
                             "Weaver Hub is available only in the Desktop edition.");
        return;
    }

    const auto rLibrary = QDir(root).filePath("lib/R");
    const auto packageDirectory = QDir(rLibrary).filePath("weaverR");
    if (!QFileInfo(packageDirectory).isDir()) {
        QMessageBox::warning(this, "R Packages Not Installed",
                             "This Desktop installation does not have the R packages feature installed.");
        return;
    }

    const auto rscript = findRscriptExecutable();
    if (rscript.isEmpty()) {
        QMessageBox::warning(this, "R Not Found",
                             "Rscript.exe could not be found. Install R 4.2 or newer and try again.");
        return;
    }

    auto prepareProcess = [rLibrary](QProcess *process) {
        QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
        environment.insert("R_LIBS_USER", QDir::toNativeSeparators(rLibrary));
        environment.insert("R_LIBS", QDir::toNativeSeparators(rLibrary));
        environment.insert("R_LIBS_SITE", QString());
        process->setProcessEnvironment(environment);
    };

    QProcess validation;
    prepareProcess(&validation);
    validation.setProgram(rscript);
    validation.setArguments({"--vanilla", "-e",
        "tryCatch({ if (getRversion() < '4.2.0') stop('R 4.2 or newer is required'); "
        "loadNamespace('weaverR') }, error=function(e) { message(conditionMessage(e)); quit(status=2) })"});
    validation.start();
    if (!validation.waitForStarted(5000)) {
        QMessageBox::warning(this, "Could Not Start R",
                             "Rscript.exe could not be started: " + validation.errorString());
        return;
    }
    if (!validation.waitForFinished(60000)) {
        validation.kill();
        validation.waitForFinished();
        QMessageBox::warning(this, "R Validation Timed Out",
                             "R did not finish validating the Hub package and its dependencies in time.");
        return;
    }
    if (validation.exitStatus() != QProcess::NormalExit || validation.exitCode() != 0) {
        auto details = QString::fromLocal8Bit(validation.readAllStandardError()).trimmed();
        if (details.isEmpty()) details = QString::fromLocal8Bit(validation.readAllStandardOutput()).trimmed();
        if (details.isEmpty()) details = "The R package weaverR or one of its dependencies could not be loaded.";
        QMessageBox::warning(this, "Weaver Hub Requirements Not Met", details);
        return;
    }

    QProcess hub;
    prepareProcess(&hub);
    hub.setProgram(rscript);
    hub.setArguments({"--vanilla", "-e", "weaverR::run_hub(launch.browser = TRUE)"});
    hub.setWorkingDirectory(root);
    if (!hub.startDetached())
        QMessageBox::warning(this, "Could Not Start Weaver Hub",
                             "Rscript.exe could not launch the Hub.");
}

void MainWindow::modifyInstallation(const QString &version, const QString &edition) {
#if defined(Q_OS_WIN)
    if (installerProcess_.state() != QProcess::NotRunning) {
        QMessageBox::information(this, "Installer Running", "Wait for the current installer operation to finish first.");
        return;
    }

    const auto productNamespace = QUuid(QStringLiteral("{6ba7b810-9dad-11d1-80b4-00c04fd430c8}"));
    const auto productName = QString("Weaver-%1-%2-product").arg(version, edition).toUtf8();
    const auto productCode = QUuid::createUuidV5(productNamespace, productName).toString().toUpper();
    const auto windowsDir = qEnvironmentVariable("WINDIR");
    const auto systemMsiexec = QDir(windowsDir).filePath("System32/msiexec.exe");
    const auto msiexec = QFileInfo::exists(systemMsiexec)
        ? systemMsiexec : QStandardPaths::findExecutable("msiexec.exe");
    if (msiexec.isEmpty()) {
        QMessageBox::warning(this, "Windows Installer Not Found", "msiexec.exe could not be found.");
        return;
    }

    const auto installRoot = QDir::toNativeSeparators(
        QDir(managerProgramInstallRoot()).filePath("installed_versions"));
    const auto appDataDirectory = QDir::toNativeSeparators(managerInstalledVersionsDirectory());
    if (appDataDirectory.isEmpty()) {
        QMessageBox::warning(this, "Could Not Open Maintenance", "The Weaver Manager data folder could not be resolved.");
        return;
    }
    const auto logDirectory = managerLogDirectory();
    if (logDirectory.isEmpty() || !QDir().mkpath(logDirectory)) {
        QMessageBox::warning(this, "Could Not Open Maintenance", "The Weaver Manager log directory could not be created.");
        return;
    }
    installerLogPath_ = QDir::toNativeSeparators(QDir(logDirectory).filePath("msiexec-modify.log"));

    releaseTable_->setEnabled(false);
    installerProcess_.setProgram(msiexec);
    installerProcess_.setArguments({"/i", productCode, "/L*V", installerLogPath_});
    installerProcess_.setNativeArguments(QString("WEAVER_INSTALL_ROOT=\"%1\" APPDATADIR=\"%2\" WEAVER_MANAGER_INSTALL=1")
                                             .arg(installRoot, appDataDirectory));
    qInfo().noquote() << "Starting msiexec maintenance; product:" << productCode
                      << "verbose log:" << installerLogPath_;
    installerProcess_.start();
    if (!installerProcess_.waitForStarted(5000)) {
        releaseTable_->setEnabled(true);
        QMessageBox::warning(this, "Could Not Start Maintenance",
                             installerProcess_.errorString() + "\n\nMSI log (if created):\n" + installerLogPath_);
        installerLogPath_.clear();
        return;
    }
    setStatus(QString("Opening maintenance for Weaver %1 (%2)…").arg(version, edition));
#else
    Q_UNUSED(version);
    Q_UNUSED(edition);
    QMessageBox::warning(this, "Unsupported Platform", "Version modification through Weaver Manager is available on Windows only.");
#endif
}

void MainWindow::uninstallInstallation(const QString &version, const QString &edition) {
#if defined(Q_OS_WIN)
    if (installerProcess_.state() != QProcess::NotRunning) {
        QMessageBox::information(this, "Installer Running", "Wait for the current installer operation to finish first.");
        return;
    }

    const auto answer = QMessageBox::question(
        this, "Uninstall Weaver Version",
        QString("Uninstall Weaver %1 (%2)?").arg(version, edition),
        QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (answer != QMessageBox::Yes) return;

    const auto productNamespace = QUuid(QStringLiteral("{6ba7b810-9dad-11d1-80b4-00c04fd430c8}"));
    const auto productName = QString("Weaver-%1-%2-product").arg(version, edition).toUtf8();
    const auto productCode = QUuid::createUuidV5(productNamespace, productName).toString().toUpper();
    const auto windowsDir = qEnvironmentVariable("WINDIR");
    const auto systemMsiexec = QDir(windowsDir).filePath("System32/msiexec.exe");
    const auto msiexec = QFileInfo::exists(systemMsiexec)
        ? systemMsiexec : QStandardPaths::findExecutable("msiexec.exe");
    if (msiexec.isEmpty()) {
        QMessageBox::warning(this, "Windows Installer Not Found", "msiexec.exe could not be found.");
        return;
    }

    releaseTable_->setEnabled(false);
    installerProcess_.setProgram(msiexec);
    installerProcess_.setArguments({"/x", productCode});
    installerProcess_.start();
    if (!installerProcess_.waitForStarted(5000)) {
        releaseTable_->setEnabled(true);
        QMessageBox::warning(this, "Could Not Start Uninstaller", installerProcess_.errorString());
        return;
    }
    setStatus(QString("Uninstalling Weaver %1 (%2)…").arg(version, edition));
#else
    Q_UNUSED(version);
    Q_UNUSED(edition);
    QMessageBox::warning(this, "Unsupported Platform", "Version uninstallation through Weaver Manager is available on Windows only.");
#endif
}

void MainWindow::setStatus(const QString &message) { status_->setText(message); }
