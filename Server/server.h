#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include "chanells.h"
#include <QList>
#include<QJsonParseError>
#include<QJsonDocument>
#include<QJsonObject>
#include"Users.h"
#include"Account.h"


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
    Users* users;
    Account* account;


private slots:
    void handleMessage(chanells* source, QString msg);
    void handleDisconnection();
};

#endif // SERVER_H
