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
class QNetworkReply;
class QCloseEvent;
class QVBoxLayout;
class QWidget;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void refreshReleases();
    void installVerifiedInstaller(const QJsonObject &entry);
    void continueVerifiedInstallerInstall(const QJsonObject &entry);
#if ENABLE_TEST
    void addLocalTestInstaller();
    void installLocalTestInstaller(const QString &path);
#endif
    void downloadInstaller(const QString &url, const QString &assetName, const QByteArray &sha256,
                          qint64 size);
    bool launchInstaller(const QString &path, QString *errorMessage);
    void onReleasesReceived(QNetworkReply *reply);

private:
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
    void requestReleasePage(int page);
    void retryReleasePage(int page, const QString &errorMessage);
    void requestManagerReleasePage(int page);
    void handleManagerReleasePage(QNetworkReply *reply, const QByteArray &data);
    void showManagerUpdateNotification();
    void fetchReleaseManifests();
    void processReleaseManifest(int index);
    void populateReleaseTable();
    void pruneReleaseCache();
    void refreshReleaseCacheStatuses();
    void removeInstallerCache();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QNetworkAccessManager network_;
    QNetworkAccessManager managerUpdateNetwork_;
    QNetworkAccessManager releaseValidationNetwork_;
    QProcess installerProcess_;
    QAction *updateNotificationsAction_{};
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
