#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QString>
#include <QUrl>

/**
 * @brief Gère le téléchargement sécurisé d’un fichier binaire via HTTPS,
 *        avec suivi de progression et analyse ELF post-téléchargement.
 */
class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    void startDownload();

signals:
    void statusMessage(const QString &message);           // Messages d'étapes/état
    void progressChanged(int progress);                   // Mise à jour de la progression (0 à 100)
    void downloadProgress(qint64 received, qint64 total); // Progression brute (octets)
    void statusUpdated(const QString &status);            // Compatibilité alternative
    void downloadFinished(const QString &filePath);       // Fichier prêt

private slots:
    void onDownloadProgress(qint64 received, qint64 total);
    void onDownloadFinished();
    void onErrorOccurred(QNetworkReply::NetworkError error);

private:
    void simulatePostDownloadSteps(); // Étapes factices post-téléchargement

    QNetworkAccessManager manager;
    QNetworkReply *reply;
    QFile *file;         // utilisé dans onDownloadFinished()
    QFile outputFile;    // utilisé dans startDownload()
    int currentProgress; // progression logique (0 à 100)
};

#endif // NETWORKMANAGER_H
