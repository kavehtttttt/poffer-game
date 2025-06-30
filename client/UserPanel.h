#ifndef USERPANEL_H
#define USERPANEL_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include <QPushButton>

class UserPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserPanel(QWidget *parent, QTcpSocket *socket, const QString &username);
    ~UserPanel();

private slots:
    void onPlayGameClicked();
    void handleServerMessage();
    void onHistoryClicked();
    void onEditInfoClicked();
    void onExitAccountClicked();

private:
    void setupUI();
    void updateConnectionStatus();

    QTcpSocket *socket;
    QString username;

    QWidget *centralWidget;
    QLabel *backgroundLabel;
    QLabel *connectionStatusLabel;

    QPushButton *playButton;
    QPushButton *historyButton;
    QPushButton *editInfoButton;
    QPushButton *exitButton;
};

#endif // USERPANEL_H
