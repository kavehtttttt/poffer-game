#ifndef CHANELLS_H
#define CHANELLS_H

#include <QObject>
#include <QTcpSocket>


class chanells : public QObject
{
    Q_OBJECT
public:

    explicit chanells(QTcpSocket *socket, QObject *parent = nullptr);
    ~chanells();

    void sendMessage(const QString& msg);

public slots:
    void start();
    void readyRead();
    void disconnectedSlot();

signals:
    void messageReceived(chanells* source, const QString& msg);
    void disconnected();

private:
    QTcpSocket *socket;

};

#endif // CHANELLS_H
