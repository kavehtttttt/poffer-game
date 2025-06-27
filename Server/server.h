#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QList>
#include <QMap> // Added for QMap
#include "chanells.h"
#include "Users.h"
#include "Account.h"

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
    QMap<QString, chanells*> waitingClients; // Map to store clients in waiting room (username to channel)
    Users* users;
    Account* account;
};

#endif // SERVER_H
