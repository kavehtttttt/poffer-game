#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

class signup : public QWidget
{
    Q_OBJECT

public:
    explicit signup(QWidget *parent = nullptr, QTcpSocket *socket = nullptr);
    ~signup();

private slots:
    void goBackToMainMenu();
    void handleSignUp();
    void handleServerResponse();

private:
    QLabel *titleLabel;
    QLineEdit *nameEdit;
    QLineEdit *lastnameEdit;
    QLineEdit *emailEdit;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *phoneEdit;
    QPushButton *submitButton;
    QPushButton *backButton;
    QLabel *connectionStatusLabel;

    QTcpSocket *socket;
};

#endif // SIGNUP_H
