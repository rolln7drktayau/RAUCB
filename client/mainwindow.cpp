#include "mainwindow.h"
#include "ui_mainwindow.h" // Add this include for the UI
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QNetworkRequest>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include "network/networkmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      networkManager(new NetworkManager(this))
{
    ui->setupUi(this);
    connect(ui->checkUpdateButton, &QPushButton::clicked, this, &MainWindow::onButtonClick);
    connect(networkManager, &NetworkManager::progressChanged, ui->progressBar, &QProgressBar::setValue);
    connect(networkManager, &NetworkManager::statusMessage, this, &MainWindow::appendLogMessage);
}

void MainWindow::onButtonClick()
{
    ui->statusLabel->setText("Statut : Vérification de mise à jour...");
    ui->logTextEdit->clear();
    ui->progressBar->setValue(0);
    networkManager->startDownload();
}

void MainWindow::appendLogMessage(const QString &msg)
{
    ui->logTextEdit->append(msg);
    ui->statusLabel->setText("Statut : " + msg);
}

MainWindow::~MainWindow()
{
    delete ui;
}