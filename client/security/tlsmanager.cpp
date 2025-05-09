#include "tlsmanager.h"
#include <QSslSocket>

QSslConfiguration TLSManager::getConfiguration()
{
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone); // Accept self-signed certs
    return config;
}
