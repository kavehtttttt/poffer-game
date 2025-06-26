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
    explicit UserPanel(QWidget *parent = nullptr, QTcpSocket *socket = nullptr, const QString &username = "");

    ~UserPanel();

private slots:
    void onPlayGameClicked();
    void onHistoryClicked();
    void onEditInfoClicked();
    void onExitAccountClicked();
    void handleLogoutResponse();

private:
    QTcpSocket *socket;
    QString username;

    QWidget *centralWidget;
    QLabel *backgroundLabel;
    QLabel *connectionStatusLabel;

    QPushButton *playButton;
    QPushButton *historyButton;
    QPushButton *editInfoButton;
    QPushButton *exitButton;

    void setupUI();
    void updateConnectionStatus();
};

#endif // USERPANEL_H
