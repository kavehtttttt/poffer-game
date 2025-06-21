#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

class signup : public QWidget
{
    Q_OBJECT

public:
    explicit signup(QWidget *parent = nullptr);
    ~signup();

private slots:
    void goBackToMainMenu();
    void handleSignUp();

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

};

#endif // SIGNUP_H
