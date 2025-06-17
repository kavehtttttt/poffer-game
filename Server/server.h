#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include "chanells.h"
#include <QList>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    void startServer();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    QList<chanells*> clients;

private slots:
    void handleMessage(QString msg);
    void handleDisconnection();
};

#endif // SERVER_H
