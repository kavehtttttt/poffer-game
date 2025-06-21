#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr, QTcpSocket *socket = nullptr);
    ~Login();

private slots:
    void goBackToMainMenu();
    void handleLogin();

private:
    QLabel *titleLabel;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *forgotButton;
    QPushButton *backButton;
    QLabel *connectionStatusLabel;

    QTcpSocket *socket;
};

#endif // LOGIN_H
