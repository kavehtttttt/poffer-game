#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QList>
#include <QMap>
#include "chanells.h"
#include "Users.h"
#include "Account.h"
#include "GameManager.h"

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    void startServer();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void handleMessage(chanells* source, QString msg);
    void handleDisconnection();

private:
    QList<chanells*> clients;
    QMap<QString, chanells*> waitingClients;
    Users* users;
    Account* account;
    GameManager* gameManager;
};

#endif // SERVER_H
