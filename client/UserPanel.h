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

private slots:
    void onPlayGameClicked();
    void handleStartGameResponse();
    void onHistoryClicked();
    void onEditInfoClicked();
    void onExitAccountClicked();
};

#endif // USERPANEL_H
