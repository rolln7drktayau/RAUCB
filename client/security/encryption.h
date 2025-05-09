#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <QByteArray>
#include <QString>

class Encryption
{
public:
    // AES encryption/decryption
    static QByteArray aesEncrypt(const QByteArray &data, const QByteArray &key, const QByteArray &iv);
    static QByteArray aesDecrypt(const QByteArray &data, const QByteArray &key, const QByteArray &iv);

    // RSA encryption/decryption
    static QByteArray rsaEncrypt(const QByteArray &data, const QString &publicKeyFile);
    static QByteArray rsaDecrypt(const QByteArray &data, const QString &privateKeyFile);
};

#endif // ENCRYPTION_H