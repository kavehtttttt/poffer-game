#include "login.h"
#include "MainMenu.h"
#include "UserPanel.h"
#include "resetpassword.h"
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

Login::Login(QWidget *parent, QTcpSocket *socket)
    : QWidget(parent), socket(socket)
{
    setWindowTitle("Log in to your account");
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
    formFrame->setFixedSize(400, 320);
    formFrame->setStyleSheet("QFrame { background-color: #e8d4b0; }");

    QVBoxLayout *formLayout = new QVBoxLayout(formFrame);
    formLayout->setContentsMargins(30, 30, 30, 30);
    formLayout->setSpacing(15);

    titleLabel = new QLabel("Log In", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Georgia", 22, QFont::Bold));
    titleLabel->setStyleSheet("color: #4e3b2b;");
    formLayout->addWidget(titleLabel);

    QFont inputFont("Consolas", 12);

    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Username");
    usernameEdit->setFont(inputFont);
    usernameEdit->setMinimumHeight(44);
    usernameEdit->setStyleSheet("QLineEdit { background-color: #fff9f2; border: 2px solid #a67c52;"
                                " border-radius: 10px; padding: 4px; color: #3a2a1e; font-size: 14px; }"
                                "QLineEdit:focus { border-color: #d2a679; }");
    formLayout->addWidget(usernameEdit);

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFont(inputFont);
    passwordEdit->setMinimumHeight(44);
    passwordEdit->setStyleSheet("QLineEdit { background-color: #fff9f2; border: 2px solid #a67c52;"
                                " border-radius: 10px; padding: 4px; color: #3a2a1e; font-size: 14px; }"
                                "QLineEdit:focus { border-color: #d2a679; }");
    formLayout->addWidget(passwordEdit);

    QFont buttonFont("Georgia", 12, QFont::Bold);
    QString buttonStyle =
        "QPushButton { background-color: #814040; color: #fceacb; border: 2px solid #c2955d; "
        "border-radius: 10px; padding: 10px; }"
        "QPushButton:hover { background-color: #a0522d; }";

    loginButton = new QPushButton("Log In", this);
    loginButton->setFont(buttonFont);
    loginButton->setMinimumHeight(42);
    loginButton->setStyleSheet(buttonStyle);
    formLayout->addWidget(loginButton);

    forgotButton = new QPushButton("Forgot Password?", this);
    forgotButton->setFont(buttonFont);
    forgotButton->setMinimumHeight(42);
    forgotButton->setStyleSheet(buttonStyle);
    formLayout->addWidget(forgotButton);

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

    disconnect(socket, nullptr, this, nullptr);

    connect(socket, &QTcpSocket::connected, this, [this]() {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: green; font-weight: bold;");
    });

    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        connectionStatusLabel->setText("Disconnected from server");
        connectionStatusLabel->setStyleSheet("color: orange; font-weight: bold;");
    });

    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        connectionStatusLabel->setText("Connection error");
        connectionStatusLabel->setStyleSheet("color: red; font-weight: bold;");
    });

    connect(socket, &QTcpSocket::readyRead, this, &Login::handleServerResponse);

    outerLayout->addWidget(formFrame, 0, Qt::AlignHCenter);
    outerLayout->addStretch();

    backButton = new QPushButton("Back", this);
    backButton->setFont(QFont("Georgia", 12, QFont::Bold));
    backButton->setFixedSize(100, 36);
    backButton->setStyleSheet("QPushButton { background-color: #4e3b2b; color: #fceacb; border: 2px solid #d2a679; "
                              "border-radius: 10px; font-weight: bold; letter-spacing: 1px; }"
                              "QPushButton:hover { background-color: #6b4c35; border: 2px solid #e6c27a; }"
                              "QPushButton:pressed { background-color: #3a2a1e; border-style: inset; }");

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(backButton, 0, Qt::AlignLeft);
    bottomLayout->addStretch();
    outerLayout->addLayout(bottomLayout);

    connect(backButton, &QPushButton::clicked, this, &Login::goBackToMainMenu);
    connect(loginButton, &QPushButton::clicked, this, &Login::handleLogin);

    connect(forgotButton, &QPushButton::clicked, this, [=]() {
        // قطع اتصال قبل از رفتن به صفحه‌ی reset
        disconnect(socket, &QTcpSocket::readyRead, this, &Login::handleServerResponse);

        this->hide();
        ResetPassword *resetPage = new ResetPassword(nullptr, socket);
        resetPage->show();
        this->deleteLater();
    });

}

void Login::handleLogin()
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "Connection Error", "Not connected to server.");
        return;
    }

    QString username = usernameEdit->text();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter both username and password.");
        return;
    }

    QJsonObject loginJson;
    loginJson["type"] = "login";
    loginJson["username"] = username;
    loginJson["password"] = password;

    QJsonDocument doc(loginJson);
    socket->write(doc.toJson(QJsonDocument::Compact));
    socket->flush();
}

void Login::handleServerResponse()
{
    QByteArray response = socket->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(response, &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        QMessageBox::warning(this, "Login Failed", "Invalid response from server.");
        return;
    }

    QJsonObject obj = doc.object();
    QString type = obj.value("type").toString();
    QString status = obj.value("status").toString();
    QString message = obj.value("message").toString();

    if (type == "login" && status == "success") {
        QMessageBox::information(this, "Login", message);
        this->hide();
        UserPanel *panel = new UserPanel(nullptr, socket , usernameEdit->text());
        panel->show();
        this->deleteLater();
    } else {
        QMessageBox::warning(this, "Login Failed", message);
    }
}

void Login::goBackToMainMenu()
{
    disconnect(socket, &QTcpSocket::readyRead, this, &Login::handleServerResponse);

    this->hide();
    MainMenu *mainMenu = new MainMenu(nullptr, socket);
    mainMenu->show();
    this->deleteLater();
}

Login::~Login() {}
