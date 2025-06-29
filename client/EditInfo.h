#ifndef EDITINFO_H
#define EDITINFO_H

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class EditInfo : public QWidget
{
    Q_OBJECT

public:
    EditInfo(QWidget *parent, QTcpSocket *socket, const QString &username);

private slots:
    void updateEditStates();
    void handleEditRequest();
    void goBack();

    void handleServerResponse();

private:
    QTcpSocket *socket;
    QString username;

    QLabel *connectionStatusLabel;

    QLineEdit *nameEdit;
    QLineEdit *lastnameEdit;
    QLineEdit *emailEdit;
    QLineEdit *passwordEdit;
    QLineEdit *phoneEdit;
    QLineEdit *usernameEdit;

    QCheckBox *nameCheck;
    QCheckBox *lastnameCheck;
    QCheckBox *emailCheck;
    QCheckBox *passwordCheck;
    QCheckBox *phoneCheck;
    QCheckBox *usernameCheck;

    QPushButton *submitButton;
    QPushButton *backButton;

    void setupField(QVBoxLayout *layout, QLineEdit *&edit, QCheckBox *&check, const QString &placeholder, bool isPassword = false);
    void connectAllCheckboxes();
};

#endif // EDITINFO_H
