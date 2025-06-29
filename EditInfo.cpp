#include "EditInfo.h"
#include "UserPanel.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QRegularExpression>
#include <QHBoxLayout>
#include <QDebug>

EditInfo::EditInfo(QWidget *parent, QTcpSocket *socket, const QString &username)
    : QWidget(parent), socket(socket), username(username)
{
    setWindowTitle("Edit Your Information, " + username + "!");
    setFixedSize(800, 600);

    QLabel *backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(QPixmap(":/images/images/1000025479.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setFixedSize(800, 600);
    backgroundLabel->lower();

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addStretch();

    QFrame *formFrame = new QFrame(this);
    formFrame->setFixedSize(420, 520);
    formFrame->setStyleSheet("QFrame { background-color: #e8d4b0; }");

    QVBoxLayout *formLayout = new QVBoxLayout(formFrame);
    formLayout->setContentsMargins(30, 30, 30, 30);
    formLayout->setSpacing(15);

    QLabel *titleLabel = new QLabel("Edit Your Info", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Georgia", 22, QFont::Bold));
    titleLabel->setStyleSheet("color: #4e3b2b;");
    formLayout->addWidget(titleLabel);

    setupField(formLayout, nameEdit, nameCheck, "First Name");
    setupField(formLayout, lastnameEdit, lastnameCheck, "Last Name");
    setupField(formLayout, emailEdit, emailCheck, "Email");
    setupField(formLayout, passwordEdit, passwordCheck, "Password", true);
    setupField(formLayout, phoneEdit, phoneCheck, "Phone");
    setupField(formLayout, usernameEdit, usernameCheck, "Username");

    submitButton = new QPushButton("Edit", this);
    submitButton->setFont(QFont("Georgia", 12, QFont::Bold));
    submitButton->setMinimumHeight(42);
    submitButton->setStyleSheet(
        "QPushButton { background-color: #814040; color: #fceacb; border: 2px solid #c2955d; border-radius: 10px; padding: 10px; } "
        "QPushButton:hover { background-color: #a0522d; }"
        );
    formLayout->addWidget(submitButton);

    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setAlignment(Qt::AlignCenter);
    connectionStatusLabel->setFont(QFont("Georgia", 10, QFont::Bold));
    formLayout->addWidget(connectionStatusLabel);

    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: green;");
    } else {
        connectionStatusLabel->setText("Not connected");
        connectionStatusLabel->setStyleSheet("color: red;");
    }

    outerLayout->addWidget(formFrame, 0, Qt::AlignHCenter);
    outerLayout->addStretch();

    backButton = new QPushButton("Back", this);
    backButton->setFont(QFont("Georgia", 12, QFont::Bold));
    backButton->setFixedSize(100, 36);
    backButton->setStyleSheet(
        "QPushButton { background-color: #4e3b2b; color: #fceacb; border: 2px solid #d2a679; "
        "border-radius: 10px; font-weight: bold; letter-spacing: 1px; }"
        "QPushButton:hover { background-color: #6b4c35; border: 2px solid #e6c27a; }"
        "QPushButton:pressed { background-color: #3a2a1e; border-style: inset; }"
        );

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(backButton, 0, Qt::AlignLeft);
    bottomLayout->addStretch();
    outerLayout->addLayout(bottomLayout);

    connect(backButton, &QPushButton::clicked, this, &EditInfo::goBack);
    connect(submitButton, &QPushButton::clicked, this, &EditInfo::handleEditRequest);
    connectAllCheckboxes();

    // اینجا کانکت برای دریافت پاسخ از سرور
    connect(socket, &QTcpSocket::readyRead, this, &EditInfo::handleServerResponse);
}

void EditInfo::setupField(QVBoxLayout *layout, QLineEdit *&edit, QCheckBox *&check, const QString &placeholder, bool isPassword)
{
    QHBoxLayout *hLayout = new QHBoxLayout;
    check = new QCheckBox;
    edit = new QLineEdit;
    edit->setPlaceholderText(placeholder);
    edit->setEnabled(false);
    if (isPassword)
        edit->setEchoMode(QLineEdit::Password);
    edit->setMinimumHeight(40);
    edit->setFont(QFont("Consolas", 12));
    edit->setStyleSheet("QLineEdit { background-color: #fff9f2; border: 2px solid #a67c52; border-radius: 10px; padding: 4px; color: #3a2a1e; font-size: 14px; }");
    hLayout->addWidget(check);
    hLayout->addWidget(edit);
    layout->addLayout(hLayout);
}

void EditInfo::connectAllCheckboxes()
{
    QList<QCheckBox*> checks = {nameCheck, lastnameCheck, emailCheck, passwordCheck, phoneCheck, usernameCheck};
    for (QCheckBox *check : checks) {
        connect(check, &QCheckBox::stateChanged, this, [=](int) {
            for (QCheckBox *c : checks) {
                if (c != check)
                    c->setChecked(false);
            }
            updateEditStates();
        });
    }
}

void EditInfo::updateEditStates()
{
    nameEdit->setEnabled(nameCheck->isChecked());
    lastnameEdit->setEnabled(lastnameCheck->isChecked());
    emailEdit->setEnabled(emailCheck->isChecked());
    passwordEdit->setEnabled(passwordCheck->isChecked());
    phoneEdit->setEnabled(phoneCheck->isChecked());
    usernameEdit->setEnabled(usernameCheck->isChecked());
}

void EditInfo::handleEditRequest()
{
    QString field, newValue;

    try {
        if (nameCheck->isChecked()) {
            field = "Edit_fName";
            newValue = nameEdit->text();
        } else if (lastnameCheck->isChecked()) {
            field = "Edit_Lname";
            newValue = lastnameEdit->text();
        } else if (emailCheck->isChecked()) {
            field = "Edit_email";
            newValue = emailEdit->text();
            QRegularExpression emailRegex("^[\\w\\.]+@[\\w\\.]+\\.[a-zA-Z]{2,}$");
            if (!emailRegex.match(newValue).hasMatch())
                throw std::runtime_error("Invalid email format.");
        } else if (passwordCheck->isChecked()) {
            field = "Edit_password";
            newValue = passwordEdit->text();
            QRegularExpression passRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{8,}$");
            if (!passRegex.match(newValue).hasMatch())
                throw std::runtime_error("Password must be at least 8 characters and include uppercase, lowercase, and digit.");
        } else if (phoneCheck->isChecked()) {
            field = "Edit_Phone";
            newValue = phoneEdit->text();
            QRegularExpression phoneRegex("^09\\d{9}$");
            if (!phoneRegex.match(newValue).hasMatch())
                throw std::runtime_error("Invalid phone number.");
        } else if (usernameCheck->isChecked()) {
            field = "Edit_username";
            newValue = usernameEdit->text();
        } else {
            throw std::runtime_error("Please select one field to edit.");
        }

        if (newValue.isEmpty())
            throw std::runtime_error("Field cannot be empty.");

        QJsonObject json;
        json["type"] = field;
        json["username"] = username;
        if (field == "Edit_username")
            json["new_username"] = newValue;
        else
            json["new_value"] = newValue;

        QJsonDocument doc(json);
        socket->write(doc.toJson(QJsonDocument::Compact));
        socket->flush();

        // پیام اینجا حذف شده — حالا پیام بعد از پاسخ سرور نمایش داده می‌شود.

    } catch (const std::exception &ex) {
        QMessageBox::warning(this, "Validation Error", ex.what());
    }
}

void EditInfo::handleServerResponse()
{
    QByteArray responseData = socket->readAll();

    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (!doc.isObject()) {
        QMessageBox::warning(this, "Server Error", "Invalid response from server.");
        return;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("status") && obj.contains("message")) {
        QString status = obj["status"].toString();
        QString message = obj["message"].toString();

        if (status == "success") {
            QMessageBox::information(this, "Success", message);
        } else {
            QMessageBox::warning(this, "Failed", message);
        }
    } else {
        QMessageBox::warning(this, "Server Error", "Unexpected response format.");
    }
}

void EditInfo::goBack()
{
    this->hide();
    UserPanel *userpanel = new UserPanel(nullptr, socket, username);
    userpanel->show();
    this->deleteLater();
}
