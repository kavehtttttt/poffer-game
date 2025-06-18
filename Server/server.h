#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include "chanells.h"
#include <QList>
#include<QJsonParseError>
#include<QJsonDocument>
#include<QJsonObject>

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
    void handleMessage(chanells* source, QString msg);
    void handleDisconnection();
};

#endif // SERVER_H
