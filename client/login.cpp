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
    backgroundLabel->setPixmap(QPixmap(":/images/images/1000025479.png")); // your wooden background
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setFixedSize(800, 600);
    backgroundLabel->lower();

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addStretch();

    QFrame *formFrame = new QFrame(this);
    formFrame->setFixedSize(400, 320);
    // Removed border and radius - just plain background color
    formFrame->setStyleSheet("QFrame { background-color: #e8d4b0; }");

    QVBoxLayout *formLayout = new QVBoxLayout(formFrame);
    formLayout->setContentsMargins(30, 30, 30, 30);
    formLayout->setSpacing(15);

    titleLabel = new QLabel("Log In", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Georgia", 22, QFont::Bold));
    titleLabel->setStyleSheet("color: #4e3b2b;");
    formLayout->addWidget(titleLabel);

    QFont inputFont("Georgia", 12);

    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("Username");
    usernameEdit->setFont(inputFont);
    usernameEdit->setMinimumHeight(36);
    usernameEdit->setStyleSheet(
        "QLineEdit {"
        " background-color: #fff9f2;"
        " border: 2px solid #a67c52;"
        " border-radius: 10px;"
        " padding: 8px;"
        " color: #3a2a1e;"
        "}"
        "QLineEdit:focus {"
        " border-color: #d2a679;"
        "}"
        );
    formLayout->addWidget(usernameEdit);

    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFont(inputFont);
    passwordEdit->setMinimumHeight(36);
    passwordEdit->setStyleSheet(
        "QLineEdit {"
        " background-color: #fff9f2;"
        " border: 2px solid #a67c52;"
        " border-radius: 10px;"
        " padding: 8px;"
        " color: #3a2a1e;"
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

    outerLayout->addWidget(formFrame, 0, Qt::AlignHCenter);
    outerLayout->addStretch();

    backButton = new QPushButton("Back", this);
    backButton->setFont(QFont("Georgia", 12, QFont::Bold));
    backButton->setFixedSize(100, 36);  // much smaller size
    backButton->setStyleSheet(
        "QPushButton {"
        " background-color: #4e3b2b;"
        " color: #fceacb;"
        " border: 2px solid #d2a679;"
        " border-radius: 10px;"
        " font-weight: bold;"
        " letter-spacing: 1px;"
        "}"
        "QPushButton:hover {"
        " background-color: #6b4c35;"
        " border: 2px solid #e6c27a;"
        "}"
        "QPushButton:pressed {"
        " background-color: #3a2a1e;"
        " border-style: inset;"
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
