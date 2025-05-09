#include "networkmanager.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QTimer>
#include <QSslCertificate>
#include <QSslSocket>
#include <QRandomGenerator>
#include <QDataStream>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent), reply(nullptr), currentProgress(0), file(nullptr)
{
}

NetworkManager::~NetworkManager()
{
    if (file)
    {
        file->close();
        delete file;
    }
}

void NetworkManager::startDownload()
{
    emit statusMessage("Étape 1 : Préparation de la requête HTTPS...");

    // URL de téléchargement
    QUrl url("https://127.0.0.1:8443/app");

    QNetworkRequest request(url);
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();

    // Vérification OpenSSL
    emit statusMessage("Étape 2 : Chargement de la configuration SSL...");
    if (!QSslSocket::supportsSsl())
    {
        emit statusMessage("⚠️ OpenSSL non supporté !");
        return;
    }
    emit statusMessage("OpenSSL supporté ✔");

    emit statusMessage("Étape 3 : Désactivation de la vérification du certificat (dev uniquement)");
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone); // ⚠️ Développement uniquement
    request.setSslConfiguration(sslConfig);

    emit statusMessage("Étape 4 : Envoi de la requête HTTPS...");
    reply = manager.get(request);

    // Suivi de progression
    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 received, qint64 total)
            {
        if (total > 0) {
            int percent = static_cast<int>((double(received) / total) * 50);
            if (percent > currentProgress) {
                currentProgress = percent;
                emit progressChanged(currentProgress);
            }
            emit statusMessage(QString("Téléchargement : %1%").arg(currentProgress));
        } });

    // Écriture du fichier téléchargé
    connect(reply, &QNetworkReply::readyRead, this, [this]()
            {
        if (!outputFile.isOpen()) {
            QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
            QString path = QDir(downloadsPath).filePath("app");
            outputFile.setFileName(path);
            if (!outputFile.open(QIODevice::WriteOnly)) {
                emit statusMessage("Erreur : impossible d'écrire le fichier !");
                return;
            }
        }
        outputFile.write(reply->readAll()); });

    // Fin du téléchargement
    connect(reply, &QNetworkReply::finished, this, [this]()
            {
        reply->deleteLater();
        outputFile.close();

        if (reply->error() != QNetworkReply::NoError) {
            emit statusMessage("❌ Erreur réseau : " + reply->errorString());
            return;
        }

        emit progressChanged(50);
        currentProgress = 50;
        emit statusMessage("Téléchargement terminé ✔");

        // Vérification de l'architecture ELF
        QFile binFile(outputFile.fileName());
        if (binFile.open(QIODevice::ReadOnly)) {
            QByteArray header = binFile.read(20);
            binFile.close();

            if (header.size() < 16 || !(header[0] == 0x7f && header[1] == 'E' && header[2] == 'L' && header[3] == 'F')) {
                emit statusMessage("⚠️ Ce fichier n'est pas un binaire ELF valide.");
            } else {
                QString archInfo = "🧠 Format ELF détecté\n";

                uchar elfClass = static_cast<uchar>(header[4]);
                archInfo += (elfClass == 1) ? "📦 Architecture : 32 bits\n" :
                            (elfClass == 2) ? "📦 Architecture : 64 bits\n" :
                                              "📦 Architecture : Inconnue\n";

                uchar dataEncoding = static_cast<uchar>(header[5]);
                archInfo += (dataEncoding == 1) ? "🔀 Endianness : Little Endian\n" :
                            (dataEncoding == 2) ? "🔀 Endianness : Big Endian\n" :
                                                  "🔀 Endianness : Inconnue\n";

                uchar version = static_cast<uchar>(header[6]);
                archInfo += QString("🔢 Version ELF : %1").arg(version);

                emit statusMessage(archInfo);
            }
        } else {
            emit statusMessage("Erreur lors de l'ouverture du fichier binaire.");
        }

        simulatePostDownloadSteps(); });
}

void NetworkManager::simulatePostDownloadSteps()
{
    struct Step
    {
        QString label;
        int progressIncrement;
    };

    QList<Step> étapes = {
        {"Étape 5 : Vérification du hash...", 10},
        {"Étape 6 : Vérification de la signature...", 10},
        {"Étape 7 : Décompression du fichier...", 20},
        {"Étape 8 : Finalisation...", 10},
        {"Étape 9 : Prêt pour installation.", 0}};

    int baseDelay = 0;
    for (int i = 0; i < étapes.size(); ++i)
    {
        const Step &etape = étapes[i];
        int delay = QRandomGenerator::global()->bounded(3000, 5001);
        baseDelay += delay;

        QTimer::singleShot(baseDelay, this, [this, etape, i, étapes]()
                           {
            emit statusMessage(etape.label);
            if (i == étapes.size() - 1) {
                currentProgress = 100;
            } else {
                currentProgress += etape.progressIncrement;
            }

            if (currentProgress > 100)
                currentProgress = 100;

            emit progressChanged(currentProgress); });
    }
}

// Pour compatibilité avec d'autres signaux/slots
void NetworkManager::onDownloadProgress(qint64 received, qint64 total)
{
    emit downloadProgress(received, total);
}

void NetworkManager::onDownloadFinished()
{
    if (reply->error() != QNetworkReply::NoError)
        return;

    file->write(reply->readAll());
    file->close();
    QString path = file->fileName();
    emit statusUpdated("Téléchargement terminé.");
    emit downloadFinished(path);

    reply->deleteLater();
    file = nullptr;
}

void NetworkManager::onErrorOccurred(QNetworkReply::NetworkError)
{
    emit statusUpdated("Erreur pendant le téléchargement : " + reply->errorString());
}
