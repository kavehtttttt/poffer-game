#ifndef USERPANEL_H
#define USERPANEL_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTcpSocket>

class UserPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserPanel(QWidget *parent = nullptr, QTcpSocket *socket = nullptr);
    ~UserPanel();

private slots:
    void onPlayGameClicked();
    void onHistoryClicked();
    void onEditInfoClicked();
    void onExitAccountClicked();

private:
    QTcpSocket *socket;
    QWidget *centralWidget;
    QLabel *backgroundLabel;
    QLabel *connectionStatusLabel;

    QPushButton *playButton;
    QPushButton *historyButton;
    QPushButton *editInfoButton;
    QPushButton *exitButton;
};

#endif // USERPANEL_H
