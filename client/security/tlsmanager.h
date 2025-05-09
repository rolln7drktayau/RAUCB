#pragma once
#include <QSslConfiguration>

class TLSManager
{
public:
    static QSslConfiguration getConfiguration();
};
