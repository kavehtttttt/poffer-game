#include "BaseWindow.h"

BaseWindow::BaseWindow(QWidget *parent, QTcpSocket *socket)
    : QMainWindow(parent), socket(socket)
{
    setupConnectionStatus();
    setupSocketSignals();
}

BaseWindow::~BaseWindow()
{
    if (socket) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
}

void BaseWindow::setupConnectionStatus()
{
    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setFont(QFont("Georgia", 10, QFont::Bold));
    connectionStatusLabel->setAlignment(Qt::AlignLeft);
    connectionStatusLabel->setGeometry(10, 10, 300, 20);
    connectionStatusLabel->setStyleSheet("background: transparent;");

    if (!socket) {
        connectionStatusLabel->setText("Socket is null!");
        connectionStatusLabel->setStyleSheet("color: red; font-weight: bold;");
    } else if (socket->state() == QAbstractSocket::ConnectedState) {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: lightgreen; font-weight: bold;");
    } else {
        connectionStatusLabel->setText("Connecting...");
        connectionStatusLabel->setStyleSheet("color: gray; font-weight: bold;");
    }
}

void BaseWindow::setupSocketSignals()
{
    if (!socket) return;

    connect(socket, &QTcpSocket::connected, this, [this]() {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: lightgreen; font-weight: bold;");
    });

    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        connectionStatusLabel->setText("Disconnected from server");
        connectionStatusLabel->setStyleSheet("color: orange; font-weight: bold;");
    });

    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        connectionStatusLabel->setText("Connection error");
        connectionStatusLabel->setStyleSheet("color: red; font-weight: bold;");
    });
}
