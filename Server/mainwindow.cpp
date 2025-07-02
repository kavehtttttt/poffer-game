#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *ipLayout = new QHBoxLayout();

    ipAddressLabel = new QLabel("Server IP:", this);
    ipAddressLabel->setStyleSheet("font-weight: bold;");

    ipAddressLineEdit = new QLineEdit(this);
    ipAddressLineEdit->setReadOnly(true);
    ipAddressLineEdit->setText("Discovering...");
    ipAddressLineEdit->setMinimumWidth(200);

    ipLayout->addWidget(ipAddressLabel);
    ipLayout->addWidget(ipAddressLineEdit);
    ipLayout->addStretch();

    mainLayout->addLayout(ipLayout);
    mainLayout->addStretch();

    myServer = new Server(this);
    connect(myServer, &Server::serverIpAvailable, this, &MainWindow::displayServerIp);


    myServer->startServer();

    setWindowTitle("Poffer Game Server");
    setFixedSize(400, 200);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayServerIp(const QString& ipAddress)
{
    ipAddressLineEdit->setText(ipAddress + ":8080");
}
