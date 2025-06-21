#include "signup.h"
#include "MainMenu.h"
#include "ValidationException.h"

#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QPixmap>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

signup::signup(QWidget *parent, QTcpSocket *socket)
    : QWidget(parent), socket(socket)
{
    setWindowTitle("Create an account");
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
    formFrame->setFixedSize(400, 500);
    formFrame->setStyleSheet("QFrame { background-color: #e8d4b0; }");

    QVBoxLayout *formLayout = new QVBoxLayout(formFrame);
    formLayout->setContentsMargins(30, 30, 30, 30);
    formLayout->setSpacing(15);

    titleLabel = new QLabel("Sign Up", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Georgia", 22, QFont::Bold));
    titleLabel->setStyleSheet("color: #4e3b2b;");
    formLayout->addWidget(titleLabel);

    QFont inputFont("Consolas", 12);  // ✅ به جای Georgia
    QStringList placeholders = {
        "First Name", "Last Name", "Email", "Password(at least 8 characters)", "Phone", "Username"
    };
    QList<QLineEdit*> edits = {
        nameEdit = new QLineEdit(this),
        lastnameEdit = new QLineEdit(this),
        emailEdit = new QLineEdit(this),
        passwordEdit = new QLineEdit(this),
        phoneEdit = new QLineEdit(this),
        usernameEdit = new QLineEdit(this)
    };

    for (int i = 0; i < edits.size(); ++i) {
        edits[i]->setPlaceholderText(placeholders[i]);
        edits[i]->setFont(inputFont);
        edits[i]->setMinimumHeight(44);  // ✅ افزایش ارتفاع برای رفع مشکل
        edits[i]->setStyleSheet(
            "QLineEdit {"
            " background-color: #fff9f2;"
            " border: 2px solid #a67c52;"
            " border-radius: 10px;"
            " padding: 4px;"  // ✅ padding مناسب برای جلوگیری از بریدگی
            " color: #3a2a1e;"
            " font-size: 14px;"
            "}"
            "QLineEdit:focus {"
            " border-color: #d2a679;"
            "}"
            );
        if (placeholders[i].contains("Password"))
            edits[i]->setEchoMode(QLineEdit::Password);
        formLayout->addWidget(edits[i]);
    }


    submitButton = new QPushButton("Sign Up", this);
    submitButton->setFont(QFont("Georgia", 12, QFont::Bold));
    submitButton->setMinimumHeight(42);
    submitButton->setStyleSheet(
        "QPushButton { background-color: #814040; color: #fceacb; border: 2px solid #c2955d; "
        "border-radius: 10px; padding: 10px; font-weight: bold; letter-spacing: 1px; }"
        "QPushButton:hover { background-color: #a0522d; }"
        );
    formLayout->addSpacing(5);
    formLayout->addWidget(submitButton);

    // 🔵 برچسب وضعیت اتصال به سرور
    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setFont(QFont("Georgia", 10, QFont::Bold));
    connectionStatusLabel->setAlignment(Qt::AlignCenter);
    formLayout->addWidget(connectionStatusLabel);

    // نمایش وضعیت اولیه اتصال
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

    // اتصال سیگنال‌های سوکت برای آپدیت وضعیت
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
        "QPushButton { background-color: #4e3b2b; color: #fceacb; border: 2px solid #d2a679; "
        "border-radius: 10px; font-weight: bold; letter-spacing: 1px; }"
        "QPushButton:hover { background-color: #6b4c35; border: 2px solid #e6c27a; }"
        "QPushButton:pressed { background-color: #3a2a1e; border-style: inset; }"
        );

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(backButton, 0, Qt::AlignLeft);
    bottomLayout->addStretch();
    outerLayout->addLayout(bottomLayout);

    connect(backButton, &QPushButton::clicked, this, &signup::goBackToMainMenu);
    connect(submitButton, &QPushButton::clicked, this, &signup::handleSignUp);
}

void signup::goBackToMainMenu()
{
    this->hide();
    MainMenu *mainMenu = new MainMenu(nullptr, socket);
    mainMenu->show();
}


void signup::handleSignUp()
{
    try {
        QString name = nameEdit->text();
        QString lastname = lastnameEdit->text();
        QString email = emailEdit->text();
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();
        QString phone = phoneEdit->text();

        QRegularExpression phoneRegex("^09\\d{9}$");
        QRegularExpression emailValidChars("^[A-Za-z0-9@.]+$");
        QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{8,}$");

        QString errorMessage;

        if (!phoneRegex.match(phone).hasMatch())
            errorMessage += "• Phone number is invalid.\n";

        int atCount = email.count('@');
        if (atCount != 1 || !email.contains('.') || !emailValidChars.match(email).hasMatch())
            errorMessage += "• Email is invalid.\n";

        if (!passwordRegex.match(password).hasMatch())
            errorMessage += "• Password must be at least 8 characters and include an uppercase letter, a lowercase letter, and a digit.\n";

        if (!errorMessage.isEmpty())
            throw ValidationException(errorMessage.trimmed());

        QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
        QString hashedPasswordHex = hashedPassword.toHex();

        QJsonObject userJson;
        userJson["request_type"] = "sign_up";
        userJson["first_name"] = name;
        userJson["last_name"] = lastname;
        userJson["email"] = email;
        userJson["username"] = username;
        userJson["password_hash"] = hashedPasswordHex;
        userJson["phone"] = phone;

        QJsonDocument doc(userJson);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

        if (socket && socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(jsonData);
            socket->flush();

            connect(socket, &QTcpSocket::readyRead, this, [=]() {
                QByteArray response = socket->readAll();
                QMessageBox::information(this, "Server Response", QString::fromUtf8(response));
            });
        } else {
            QMessageBox::warning(this, "Connection Error", "Not connected to server.");
        }

    } catch (const ValidationException &ex) {
        QMessageBox::warning(this, "Invalid Information", ex.what());
    } catch (const std::exception &ex) {
        QMessageBox::critical(this, "Unexpected Error", ex.what());
    }
}

signup::~signup() {}
