#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

class NetworkManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void appendLogMessage(const QString &msg);
    void onButtonClick();

private:
    Ui::MainWindow *ui;
    NetworkManager *networkManager;
};

#endif // MAINWINDOW_H
