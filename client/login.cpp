#include "login.h"
#include "MainMenu.h"
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
#include "UserPanel.h" // اضافه در بالای فایل
#include "resetpassword.h"

Login::Login(QWidget *parent, QTcpSocket *socket)
    : QWidget(parent), socket(socket)
{
    setWindowTitle("Log in to your account");
    setFixedSize(800, 600);

    QLabel *backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(QPixmap(":/images/images/1000025479.png")); // your wooden background
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

    // USERNAME
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Username");
    usernameEdit->setFont(inputFont);
    usernameEdit->setMinimumHeight(44);  // ✅ افزایش ارتفاع
    usernameEdit->setStyleSheet(
        "QLineEdit {"
        " background-color: #fff9f2;"
        " border: 2px solid #a67c52;"
        " border-radius: 10px;"
        " padding: 4px;"  // ✅ تنظیم padding مناسب
        " color: #3a2a1e;"
        " font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        " border-color: #d2a679;"
        "}"
        );
    formLayout->addWidget(usernameEdit);

    // PASSWORD
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFont(inputFont);
    passwordEdit->setMinimumHeight(44);  // ✅ افزایش ارتفاع
    passwordEdit->setStyleSheet(
        "QLineEdit {"
        " background-color: #fff9f2;"
        " border: 2px solid #a67c52;"
        " border-radius: 10px;"
        " padding: 4px;"  // ✅ تنظیم padding مناسب
        " color: #3a2a1e;"
        " font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        " border-color: #d2a679;"
        "}"
        );
    formLayout->addWidget(passwordEdit);

    QFont buttonFont("Georgia", 12, QFont::Bold);
    QString buttonStyle =
        "QPushButton {"
        " background-color: #814040;"
        " color: #fceacb;"
        " border: 2px solid #c2955d;"
        " border-radius: 10px;"
        " padding: 10px;"
        "}"
        "QPushButton:hover {"
        " background-color: #a0522d;"
        "}";

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

    // 🔵 برچسب وضعیت اتصال
    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setFont(QFont("Georgia", 10, QFont::Bold));
    connectionStatusLabel->setAlignment(Qt::AlignCenter);
    formLayout->addWidget(connectionStatusLabel);

    // وضعیت اتصال اولیه
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

    // اتصال سیگنال‌ها برای آپدیت وضعیت اتصال
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

    outerLayout->addWidget(formFrame, 0, Qt::AlignHCenter);
    outerLayout->addStretch();

    backButton = new QPushButton("Back", this);
    backButton->setFont(QFont("Georgia", 12, QFont::Bold));
    backButton->setFixedSize(100, 36);
    backButton->setStyleSheet(
        "QPushButton { background-color: #4e3b2b; color: #fceacb; border: 2px solid #d2a679;"
        " border-radius: 10px; font-weight: bold; letter-spacing: 1px; }"
        "QPushButton:hover { background-color: #6b4c35; border: 2px solid #e6c27a; }"
        "QPushButton:pressed { background-color: #3a2a1e; border-style: inset; }"
        );

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(backButton, 0, Qt::AlignLeft);
    bottomLayout->addStretch();
    outerLayout->addLayout(bottomLayout);

    connect(backButton, &QPushButton::clicked, this, &Login::goBackToMainMenu);
    connect(loginButton, &QPushButton::clicked, this, &Login::handleLogin);
    connect(forgotButton,&QPushButton::clicked,this,[=](){
        this->hide();
        ResetPassword *resetPage = new ResetPassword(nullptr,socket);
        resetPage->show();
    });
}

void Login::goBackToMainMenu()
{
    this->hide();
    MainMenu *mainMenu = new MainMenu(nullptr, socket);
    mainMenu->show();
} /// 87t68yuy76ty

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

    // هش کردن پسورد با SHA-256
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    QString hashedPasswordHex = hashedPassword.toHex();

    // ساختن شی JSON برای لاگین
    QJsonObject loginJson;
    loginJson["request_type"] = "login";
    loginJson["username"] = username;
    loginJson["password_hash"] = hashedPasswordHex;

    QJsonDocument doc(loginJson);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    // ارسال به سرور
    socket->write(jsonData);
    socket->flush();

    // جلوگیری از اتصال‌های تکراری به readyRead:
    socket->disconnect(SIGNAL(readyRead()));

    connect(socket, &QTcpSocket::readyRead, this, [=]() {
        QByteArray response = socket->readAll();
        QString responseStr = QString::fromUtf8(response).trimmed();

        QMessageBox::information(this, "Server Response", responseStr);

        if (responseStr == "login_success") {
            this->hide();
            UserPanel *panel = new UserPanel(nullptr, socket);
            panel->show();
        }
    });
}


Login::~Login() {}
