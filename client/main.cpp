#include <QApplication>
#include "mainwindow.h"
#include <QSslSocket>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!QSslSocket::supportsSsl())
    {
        qFatal("Erreur : SSL non supporté sur cette machine.");
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}
