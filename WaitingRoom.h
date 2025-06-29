#ifndef WAITINGROOM_H
#define WAITINGROOM_H

#include <QWidget>
#include <QTcpSocket>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>

class WaitingRoom : public QWidget
{
    Q_OBJECT

public:
    explicit WaitingRoom(QWidget *parent, QTcpSocket *socket, const QString &username);

private slots:
    void onWaitingMessage();

private:
    void setupUI();

    QTcpSocket *socket;
    QString username;

    QLabel *connectionStatusLabel;
    QLabel *countLabel;
    QPushButton *backButton;
    QListWidget *playerListWidget;
};

#endif // WAITINGROOM_H
