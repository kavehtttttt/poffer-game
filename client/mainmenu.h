#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTcpSocket>

class MainMenu : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget *parent = nullptr, QTcpSocket *socket = nullptr);
    ~MainMenu();

private slots:
    void openSignup();
    void openLogin();

private:
    QTcpSocket *socket;
    QWidget *centralWidget;
    QLabel *backgroundLabel;
    QLabel *connectionStatusLabel;

    QPushButton *signupButton;
    QPushButton *loginButton;
    QPushButton *exitButton;
};

#endif // MAINMENU_H
