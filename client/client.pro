QT += core gui network concurrent core-private
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           network/networkmanager.cpp \
           security/encryption.cpp \
           security/tlsmanager.cpp

HEADERS += mainwindow.h \
           network/networkmanager.h \
           security/encryption.h \
           security/tlsmanager.h

FORMS += mainwindow.ui

INCLUDEPATH += "C:/Program Files/OpenSSL-Win64/include"
LIBS += -L"C:/Program Files/OpenSSL-Win64/lib/VC/x64/MD" -llibssl -llibcrypto
