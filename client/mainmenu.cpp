#include "MainMenu.h"
#include "signup.h"
#include "login.h"

#include <QFont>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QMessageBox>

MainMenu::MainMenu(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(800, 600);
    setWindowTitle("Poffer Game");

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    backgroundLabel = new QLabel(centralWidget);
    backgroundLabel->setPixmap(QPixmap(":/images/images/1000025478.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setFixedSize(800, 600);

    QFont btnFont("Georgia", 16, QFont::Bold);  // Western-style font

    signupButton = new QPushButton("SIGN UP");
    loginButton = new QPushButton("LOG IN");
    exitButton = new QPushButton("EXIT");

    QPushButton* buttons[] = {signupButton, loginButton};

    QString btnStyle =
        "QPushButton {"
        " background-color: #4e3b2b;"
        " color: #fceacb;"
        " border: 2px solid #d2a679;"
        " border-radius: 20px;"
        " min-width: 150px;"
        " min-height: 60px;"
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
        "}";

    for (QPushButton* btn : buttons) {
        btn->setFont(btnFont);
        btn->setStyleSheet(btnStyle);
    }

    // Exit button in reddish tone
    exitButton->setFont(btnFont);
    exitButton->setStyleSheet(
        "QPushButton {"
        " background-color: #8B0000;"
        " color: #fff2e6;"
        " border: 2px solid #b30000;"
        " border-radius: 20px;"
        " min-width: 150px;"
        " min-height: 60px;"
        " font-weight: bold;"
        " letter-spacing: 1px;"
        "}"
        "QPushButton:hover {"
        " background-color: #a00000;"
        "}"
        "QPushButton:pressed {"
        " background-color: #600000;"
        " border-style: inset;"
        "}"
        );

    QWidget *buttonsWidget = new QWidget(backgroundLabel);
    buttonsWidget->setFixedSize(800, 600);

    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonsWidget);
    buttonLayout->addStretch();
    buttonLayout->addWidget(signupButton);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(loginButton);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch();

    buttonLayout->setContentsMargins(30, 500, 30, 30); // bottom alignment
    buttonLayout->setSpacing(20);
    buttonsWidget->setLayout(buttonLayout);

    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->addWidget(backgroundLabel);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralWidget->setLayout(centralLayout);

    connect(exitButton, &QPushButton::clicked, this, &QWidget::close);
    connect(signupButton, &QPushButton::clicked, this, &MainMenu::openSignup);
    connect(loginButton, &QPushButton::clicked, this, &MainMenu::openLogin);
}

MainMenu::~MainMenu() {}

void MainMenu::openSignup()
{
    signup *signupWindow = new signup();
    signupWindow->show();
    this->close();
}

void MainMenu::openLogin()
{
    Login *loginWindow = new Login();
    loginWindow->show();
    this->close();
}
