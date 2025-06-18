#include "login.h"
#include "MainMenu.h"
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>

Login::Login(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Log in to your account");
    setFixedSize(800, 600);

    QLabel *backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(QPixmap(":/images/images/1000025449.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setFixedSize(800, 600);
    backgroundLabel->lower();

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addStretch();

    QFrame *formFrame = new QFrame(this);
    formFrame->setFixedSize(400, 300);
    formFrame->setStyleSheet("QFrame { background-color: #F3E4CD; border-radius: 20px; }");

    QVBoxLayout *formLayout = new QVBoxLayout(formFrame);
    formLayout->setContentsMargins(30, 30, 30, 30);
    formLayout->setSpacing(12);

    titleLabel = new QLabel("Log In", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Segoe UI", 22, QFont::Bold));
    titleLabel->setStyleSheet("color: black;");
    formLayout->addWidget(titleLabel);

    QFont inputFont("Segoe UI", 12);

    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Username");
    usernameEdit->setFont(inputFont);
    usernameEdit->setMinimumHeight(38);
    usernameEdit->setStyleSheet(
        "QLineEdit {"
        " background-color: white;"
        " border: none;"
        " border-radius: 10px;"
        " padding: 8px;"
        " color: black;"
        " font-size: 13px;"
        "}"
        );
    formLayout->addWidget(usernameEdit);

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFont(inputFont);
    passwordEdit->setMinimumHeight(38);
    passwordEdit->setStyleSheet(
        "QLineEdit {"
        " background-color: white;"
        " border: none;"
        " border-radius: 10px;"
        " padding: 8px;"
        " color: black;"
        " font-size: 13px;"
        "}"
        );
    formLayout->addWidget(passwordEdit);

    QFont buttonFont("Segoe UI", 11, QFont::Bold);
    QString buttonStyle =
        "QPushButton {"
        " background-color: #AF2C2C;"
        " color: white;"
        " border: none;"
        " border-radius: 10px;"
        " padding: 10px;"
        " font-size: 14px;"
        "}"
        "QPushButton:hover {"
        " background-color: #8B1A1A;"
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

    outerLayout->addWidget(formFrame, 0, Qt::AlignHCenter);
    outerLayout->addStretch();

    backButton = new QPushButton("Back", this);
    backButton->setFont(QFont("Segoe UI", 13, QFont::Bold));
    backButton->setMinimumSize(110, 42);
    backButton->setStyleSheet(
        "QPushButton {"
        " background-color: qradialgradient(cx:0.5, cy:0.5, radius:1.0, fx:0.5, fy:0.5, stop:0 #FF6347, stop:1 #8B0000);"
        " color: white;"
        " border: 3px solid #FFD700;"
        " border-radius: 20px;"
        "}"
        "QPushButton:hover {"
        " background-color: qradialgradient(cx:0.5, cy:0.5, radius:1.0, fx:0.5, fy:0.5, stop:0 #FFA07A, stop:1 #B22222);"
        "}"
        );

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(backButton, 0, Qt::AlignLeft);
    bottomLayout->addStretch();
    outerLayout->addLayout(bottomLayout);

    connect(backButton, &QPushButton::clicked, this, &Login::goBackToMainMenu);
}

void Login::goBackToMainMenu()
{
    this->close();
    MainMenu *mainMenu = new MainMenu();
    mainMenu->setAttribute(Qt::WA_DeleteOnClose);
    mainMenu->show();
}

Login::~Login() {}
