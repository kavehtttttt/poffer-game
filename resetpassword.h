#ifndef RESETPASSWORD_H
#define RESETPASSWORD_H

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class ResetPassword : public QWidget
{
    Q_OBJECT

public:
    explicit ResetPassword(QWidget *parent = nullptr, QTcpSocket *socket = nullptr);
    ~ResetPassword();

private slots:
    void handleResetPassword();
    void goBackToLogin();
    void handleServerResponse();

private:
    QLineEdit *usernameEdit;
    QLineEdit *phoneEdit;
    QLineEdit *newPasswordEdit;
    QPushButton *resetButton;
    QPushButton *backButton;
    QLabel *connectionStatusLabel;

    QTcpSocket *socket;
};

#endif // RESETPASSWORD_H
