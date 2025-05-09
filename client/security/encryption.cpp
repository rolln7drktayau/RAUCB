#include "encryption.h"
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <QDebug>

#include <cerrno>

// AES encryption with OpenSSL 3.0 API
QByteArray Encryption::aesEncrypt(const QByteArray &data, const QByteArray &key, const QByteArray &iv)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        qDebug() << "Failed to create cipher context";
        return QByteArray();
    }

    // Initialize the encryption operation
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char *>(key.constData()),
                           reinterpret_cast<const unsigned char *>(iv.constData())) != 1)
    {
        qDebug() << "Failed to initialize encryption";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    // Prepare output buffer (ensure there's room for the padding)
    QByteArray encrypted;
    encrypted.resize(data.size() + EVP_CIPHER_CTX_block_size(ctx));
    int outLen1 = 0;

    // Encrypt the data
    if (EVP_EncryptUpdate(ctx,
                          reinterpret_cast<unsigned char *>(encrypted.data()),
                          &outLen1,
                          reinterpret_cast<const unsigned char *>(data.constData()),
                          data.size()) != 1)
    {
        qDebug() << "Failed during encryption";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    // Finalize encryption
    int outLen2 = 0;
    if (EVP_EncryptFinal_ex(ctx,
                            reinterpret_cast<unsigned char *>(encrypted.data() + outLen1),
                            &outLen2) != 1)
    {
        qDebug() << "Failed to finalize encryption";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    // Resize to actual encrypted data size
    encrypted.resize(outLen1 + outLen2);
    return encrypted;
}

// AES decryption with OpenSSL 3.0 API
QByteArray Encryption::aesDecrypt(const QByteArray &data, const QByteArray &key, const QByteArray &iv)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        qDebug() << "Failed to create cipher context";
        return QByteArray();
    }

    // Initialize the decryption operation
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                           reinterpret_cast<const unsigned char *>(key.constData()),
                           reinterpret_cast<const unsigned char *>(iv.constData())) != 1)
    {
        qDebug() << "Failed to initialize decryption";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    // Prepare output buffer
    QByteArray decrypted;
    decrypted.resize(data.size());
    int outLen1 = 0;

    // Decrypt the data
    if (EVP_DecryptUpdate(ctx,
                          reinterpret_cast<unsigned char *>(decrypted.data()),
                          &outLen1,
                          reinterpret_cast<const unsigned char *>(data.constData()),
                          data.size()) != 1)
    {
        qDebug() << "Failed during decryption";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    // Finalize decryption
    int outLen2 = 0;
    if (EVP_DecryptFinal_ex(ctx,
                            reinterpret_cast<unsigned char *>(decrypted.data() + outLen1),
                            &outLen2) != 1)
    {
        qDebug() << "Failed to finalize decryption";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    // Resize to actual decrypted data size
    decrypted.resize(outLen1 + outLen2);
    return decrypted;
}

// RSA encryption with OpenSSL 3.0 API
QByteArray Encryption::rsaEncrypt(const QByteArray &data, const QString &publicKeyFile)
{
    // Load the public key
    FILE *keyFile = nullptr;

#ifdef PLATFORM_WINDOWS
    errno_t err = fopen_s(&keyFile, publicKeyFile.toLocal8Bit().constData(), "r");
    if (err != 0 || !keyFile)
    {
        qDebug() << "Cannot open public key file:" << publicKeyFile;
        return QByteArray();
    }
#else
    keyFile = fopen(publicKeyFile.toLocal8Bit().constData(), "r");
    if (!keyFile)
    {
        qDebug() << "Cannot open public key file:" << publicKeyFile;
        return QByteArray();
    }
#endif
    {
        qWarning() << "Impossible d'ouvrir le fichier de clé publique:" << publicKeyFile;
        return QByteArray();
    }

    EVP_PKEY *pkey = PEM_read_PUBKEY(keyFile, NULL, NULL, NULL);
    fclose(keyFile);

    if (!pkey)
    {
        qDebug() << "Failed to read public key";
        return QByteArray();
    }

    // Create and initialize context
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx)
    {
        qDebug() << "Failed to create public key context";
        EVP_PKEY_free(pkey);
        return QByteArray();
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0)
    {
        qDebug() << "Failed to initialize encryption";
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return QByteArray();
    }

    // Set padding
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0)
    {
        qDebug() << "Failed to set padding";
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return QByteArray();
    }

    // Determine buffer length
    size_t outLen = 0;
    if (EVP_PKEY_encrypt(ctx, NULL, &outLen,
                         reinterpret_cast<const unsigned char *>(data.constData()),
                         data.size()) <= 0)
    {
        qDebug() << "Failed to calculate output length";
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return QByteArray();
    }

    // Allocate memory for encrypted data
    QByteArray encrypted;
    encrypted.resize(outLen);

    // Encrypt the data
    if (EVP_PKEY_encrypt(ctx,
                         reinterpret_cast<unsigned char *>(encrypted.data()),
                         &outLen,
                         reinterpret_cast<const unsigned char *>(data.constData()),
                         data.size()) <= 0)
    {
        qDebug() << "Encryption failed";
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return QByteArray();
    }

    // Clean up
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    // Resize to actual encrypted data size
    encrypted.resize(outLen);
    return encrypted;
}

// RSA decryption with OpenSSL 3.0 API
QByteArray Encryption::rsaDecrypt(const QByteArray &data, const QString &privateKeyFile)
{
    // Load the private key
    FILE *keyFile = nullptr;

#ifdef PLATFORM_WINDOWS
    errno_t err = fopen_s(&keyFile, privateKeyFile.toLocal8Bit().constData(), "r");
    if (err != 0 || !keyFile)
    {
        qDebug() << "Cannot open private key file:" << privateKeyFile;
        return QByteArray();
    }
#else
    keyFile = fopen(privateKeyFile.toLocal8Bit().constData(), "r");
    if (!keyFile)
    {
        qDebug() << "Cannot open private key file:" << privateKeyFile;
        return QByteArray();
    }
#endif
    {
        qWarning() << "Impossible d'ouvrir le fichier de clé privée:" << privateKeyFile;
        return QByteArray();
    }

    EVP_PKEY *pkey = PEM_read_PrivateKey(keyFile, NULL, NULL, NULL);
    fclose(keyFile);

    if (!pkey)
    {
        qDebug() << "Failed to read private key";
        return QByteArray();
    }

    // Create and initialize context
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx)
    {
        qDebug() << "Failed to create private key context";
        EVP_PKEY_free(pkey);
        return QByteArray();
    }

    if (EVP_PKEY_decrypt_init(ctx) <= 0)
    {
        qDebug() << "Failed to initialize decryption";
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return QByteArray();
    }

    // Set padding
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0)
    {
        qDebug() << "Failed to set padding";
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return QByteArray();
    }

    // Determine buffer length
    size_t outLen = 0;
    if (EVP_PKEY_decrypt(ctx, NULL, &outLen,
                         reinterpret_cast<const unsigned char *>(data.constData()),
                         data.size()) <= 0)
    {
        qDebug() << "Failed to calculate output length";
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return QByteArray();
    }

    // Allocate memory for decrypted data
    QByteArray decrypted;
    decrypted.resize(outLen);

    // Decrypt the data
    if (EVP_PKEY_decrypt(ctx,
                         reinterpret_cast<unsigned char *>(decrypted.data()),
                         &outLen,
                         reinterpret_cast<const unsigned char *>(data.constData()),
                         data.size()) <= 0)
    {
        qDebug() << "Decryption failed";
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return QByteArray();
    }

    // Clean up
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);

    // Resize to actual decrypted data size
    decrypted.resize(outLen);
    return decrypted;
}