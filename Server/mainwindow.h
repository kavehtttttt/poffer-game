#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void displayServerIp(const QString& ipAddress);

private:
    Ui::MainWindow *ui;
    QLabel *ipAddressLabel;
    QLineEdit *ipAddressLineEdit;
    Server *myServer;
};

#endif // MAINWINDOW_WINDOW_H
