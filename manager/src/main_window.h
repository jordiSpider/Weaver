#pragma once

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QProcess>
#include <QJsonArray>
#include <QJsonObject>
#include <QHash>
#include <QSet>
#include <QStringList>

class QTableWidget;
class QLabel;
class QAction;
class QCheckBox;
class QNetworkReply;
class QCloseEvent;
class QVBoxLayout;
class QWidget;

// Main application window for managing installations and verified releases.
class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    // Create the interface and load saved preferences for this Manager instance.
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    // Refresh the release catalog and validate its signed manifests.
    void refreshReleases();
    // Re-fetch release metadata before accepting a catalog entry for installation.
    void installVerifiedInstaller(const QJsonObject &entry);
    // Confirm and download an installer after the release metadata is validated.
    void continueVerifiedInstallerInstall(const QJsonObject &entry);
#if ENABLE_TEST
    void addLocalTestInstaller();
    void installLocalTestInstaller(const QString &path);
#endif
    // Download an asset and retain its expected signed hash and size for verification.
    void downloadInstaller(const QString &url, const QString &assetName, const QByteArray &sha256,
                          qint64 size);
    // Launch a supported installer, returning a diagnostic on failure.
    bool launchInstaller(const QString &path, QString *errorMessage);
    // Dispatch completed catalog, manifest, and installer network responses.
    void onReleasesReceived(QNetworkReply *reply);

private:
    // Installation discovery and launch actions for each supported Weaver edition.
    int loadInstallations();
    void refreshInstalledVersions();
    bool ensureInstallationAvailable(const QString &root);
    void openInstallationRoot(const QString &root);
    void openVersionDataFolder(const QString &version, const QString &edition);
    void openWeaverCLI(const QString &root);
    void openWeaverGUI(const QString &edition, const QString &root);
    void openRForInstallation(const QString &root);
    void openHubForInstallation(const QString &edition, const QString &root);
    void modifyInstallation(const QString &version, const QString &edition);
    void uninstallInstallation(const QString &version, const QString &edition);
    void setStatus(const QString &message);
    // Fetch paginated Weaver releases; stale requests are ignored by generation ID.
    void requestReleasePage(int page);
    void retryReleasePage(int page, const QString &errorMessage);
    void requestManagerReleasePage(int page);
    void handleManagerReleasePage(QNetworkReply *reply, const QByteArray &data);
    void showManagerUpdateNotification();
    void fetchReleaseManifests();
    void processReleaseManifest(int index);
    void finishReleaseSearch();
    void populateReleaseTable();
    void applyReleaseFilter();
    void savePreference(const QString &key, bool enabled);
    // Remove cache entries that no longer correspond to available releases.
    void pruneReleaseCache();
    void refreshReleaseCacheStatuses();
    void removeInstallerCache();
    void applyTheme();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QNetworkAccessManager network_;
    QNetworkAccessManager managerUpdateNetwork_;
    QNetworkAccessManager releaseValidationNetwork_;
    QProcess installerProcess_;
    QAction *updateNotificationsAction_{};
    QAction *weaverNotificationsAction_{};
    QAction *weaverPrereleaseNotificationsAction_{};
    QCheckBox *showPrereleasesCheckBox_{};
    QString theme_{QStringLiteral("system")};
    bool systemDarkFallback_{};
    QWidget *installedListContent_{};
    QVBoxLayout *installedListLayout_{};
    QTableWidget *releaseTable_{};
    QLabel *status_{};
    QString installerLogPath_;
    QJsonArray releaseItems_;
    QJsonArray releaseManifests_;
    QJsonArray catalogEntries_;
    int pendingManifests_{};
    qint64 latestReleaseId_{};
    QHash<int, QByteArray> downloadedManifests_;
    QHash<int, QByteArray> downloadedSignatures_;
    QSet<QString> newReleaseEntries_;
    QSet<QString> seenReleaseEntriesAtSearchStart_;
    enum class SeenCacheFormat { Tag, ReleaseId, VersionEdition };
    SeenCacheFormat seenCacheFormat_{SeenCacheFormat::Tag};
    quint64 releaseSearchGeneration_{};
    int releasePageRetryCount_{};
    QString latestManagerStableTag_;
    QString latestManagerStableUrl_;
    QString latestManagerStableVersion_;
    QString latestManagerPrereleaseTag_;
    QString latestManagerPrereleaseUrl_;
    QString latestManagerPrereleaseVersion_;
    int latestManagerPrereleaseNumber_{};
    bool managerReleasesLoaded_{};
    QString pendingDownloadPath_;
    QByteArray pendingDownloadHash_;
    qint64 pendingDownloadSize_{};
    bool closeWhenInstallerFinishes_{};
#if ENABLE_TEST
    QStringList localTestInstallers_;
#endif
};
