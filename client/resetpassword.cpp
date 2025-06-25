#include "resetpassword.h"
#include "login.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QFont>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QRegularExpression>

ResetPassword::ResetPassword(QWidget *parent, QTcpSocket *socket)
    : QWidget(parent), socket(socket)
{
    setWindowTitle("Don't forget it this time :)");
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
    formFrame->setFixedSize(400, 330);
    formFrame->setStyleSheet("QFrame { background-color: #e8d4b0; }");

    QVBoxLayout *formLayout = new QVBoxLayout(formFrame);
    formLayout->setContentsMargins(30, 30, 30, 30);
    formLayout->setSpacing(15);

    QLabel *titleLabel = new QLabel("Reset Password", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Georgia", 22, QFont::Bold));
    titleLabel->setStyleSheet("color: #4e3b2b;");
    formLayout->addWidget(titleLabel);

    QFont inputFont("Consolas", 12);
    QStringList placeholders = { "Username", "Phone", "New Password" };
    QList<QLineEdit*> edits = {
        usernameEdit = new QLineEdit(this),
        phoneEdit = new QLineEdit(this),
        newPasswordEdit = new QLineEdit(this)
    };

    for (int i = 0; i < edits.size(); ++i) {
        edits[i]->setPlaceholderText(placeholders[i]);
        edits[i]->setFont(inputFont);
        edits[i]->setMinimumHeight(44);
        edits[i]->setStyleSheet(
            "QLineEdit { background-color: #fff9f2; border: 2px solid #a67c52;"
            " border-radius: 10px; padding: 4px; color: #3a2a1e; font-size: 14px; }"
            "QLineEdit:focus { border-color: #d2a679; }"
            );
        if (placeholders[i].contains("Password"))
            edits[i]->setEchoMode(QLineEdit::Password);
        formLayout->addWidget(edits[i]);
    }

    resetButton = new QPushButton("Reset Password", this);
    resetButton->setFont(QFont("Georgia", 12, QFont::Bold));
    resetButton->setMinimumHeight(42);
    resetButton->setStyleSheet(
        "QPushButton { background-color: #814040; color: #fceacb; border: 2px solid #c2955d; "
        "border-radius: 10px; padding: 10px; font-weight: bold; letter-spacing: 1px; }"
        "QPushButton:hover { background-color: #a0522d; }"
        );
    formLayout->addSpacing(5);
    formLayout->addWidget(resetButton);

    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setFont(QFont("Georgia", 10, QFont::Bold));
    connectionStatusLabel->setAlignment(Qt::AlignCenter);
    formLayout->addWidget(connectionStatusLabel);

    if (!socket) {
        connectionStatusLabel->setText("Socket is null!");
        connectionStatusLabel->setStyleSheet("color: red; font-weight: bold;");
    } else if (socket->state() == QAbstractSocket::ConnectedState) {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        connectionStatusLabel->setText("Connecting...");
        connectionStatusLabel->setStyleSheet("color: gray;");
    }

    connect(socket, &QTcpSocket::connected, this, [=]() {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: green; font-weight: bold;");
    });
    connect(socket, &QTcpSocket::disconnected, this, [=]() {
        connectionStatusLabel->setText("Disconnected from server");
        connectionStatusLabel->setStyleSheet("color: orange; font-weight: bold;");
    });
    connect(socket, &QTcpSocket::errorOccurred, this, [=](QAbstractSocket::SocketError) {
        connectionStatusLabel->setText("Connection error");
        connectionStatusLabel->setStyleSheet("color: red; font-weight: bold;");
    });

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

    connect(resetButton, &QPushButton::clicked, this, &ResetPassword::handleResetPassword);
    connect(backButton, &QPushButton::clicked, this, &ResetPassword::goBackToLogin);
}

void ResetPassword::handleResetPassword()
{
    QString username = usernameEdit->text();
    QString phone = phoneEdit->text();
    QString newPassword = newPasswordEdit->text();

    QRegularExpression phoneRegex("^09\\d{9}$");
    QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{8,}$");

    QString errorMessage;

    if (!phoneRegex.match(phone).hasMatch())
        errorMessage += "• Phone number is invalid.\n";

    if (!passwordRegex.match(newPassword).hasMatch())
        errorMessage += "• Password must be at least 8 characters, contain upper/lower case letters and a digit.\n";

    if (username.isEmpty())
        errorMessage += "• Username cannot be empty.\n";

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", errorMessage.trimmed());
        return;
    }

    QJsonObject json;
    json["type"] = "forgetPassword";
    json["username"] = username;
    json["phone"] = phone;
    json["new_password"] = newPassword; // بدون هش

    QJsonDocument doc(json);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnect(SIGNAL(readyRead()));
        socket->write(jsonData);
        socket->flush();

        connect(socket, &QTcpSocket::readyRead, this, [=]() {
            QByteArray response = socket->readAll();
            QString responseStr = QString::fromUtf8(response).trimmed();
            QMessageBox::information(this, "Server", responseStr);
        });
    } else {
        QMessageBox::warning(this, "Connection Error", "Not connected to server.");
    }
}

void ResetPassword::goBackToLogin()
{
    this->hide();
    Login *loginPage = new Login(nullptr, socket);
    loginPage->show();
}

ResetPassword::~ResetPassword() {}
