#ifndef CHANELLS_H
#define CHANELLS_H

#include <QObject>
#include <QTcpSocket>
#include <thread>

class chanells : public QObject
{
    Q_OBJECT

public:
    explicit chanells(QTcpSocket *_socket, QObject *parent = nullptr);
    ~chanells();

    void start();
    void stop();

private:
    QTcpSocket *socket;
    std::thread t;
    bool running = false;

    void communication();

signals:
    void messageReceived(QString msg);
    void disconnected();

};

#endif // CHANELLS_H
