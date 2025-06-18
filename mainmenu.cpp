#include "MainMenu.h"
#include "signup.h"
#include "login.h"  // 🔸 اضافه‌شده

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
    backgroundLabel->setPixmap(QPixmap(":/images/images/1000025443.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setFixedSize(800, 600);

    QFont btnFont("Segoe UI", 16, QFont::Bold);

    signupButton = new QPushButton("SIGN UP");
    loginButton = new QPushButton("LOG IN");
    exitButton = new QPushButton("EXIT");

    QPushButton* buttons[] = {signupButton, loginButton, exitButton};

    QString btnStyle =
        "QPushButton {"
        " background-color: qradialgradient(cx:0.5, cy:0.5, radius:1.0, fx:0.5, fy:0.5,"
        " stop:0 #FF6347, stop:1 #8B0000);"
        " color: white;"
        " border: 3px solid #FFD700;"
        " border-radius: 20px;"
        " min-width: 150px;"
        " min-height: 60px;"
        "}"
        "QPushButton:hover {"
        " background-color: qradialgradient(cx:0.5, cy:0.5, radius:1.0, fx:0.5, fy:0.5,"
        " stop:0 #FFA07A, stop:1 #B22222);"
        "}";

    for (QPushButton* btn : buttons) {
        btn->setFont(btnFont);
        btn->setStyleSheet(btnStyle);
    }

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
    connect(loginButton, &QPushButton::clicked, this, &MainMenu::openLogin);  // 🔸 اتصال دکمه لاگین
}

MainMenu::~MainMenu() {}

void MainMenu::openSignup()
{
    signup *signupWindow = new signup();
    signupWindow->show();
    this->close();
}

void MainMenu::openLogin()  // 🔸 اسلات برای باز کردن صفحه لاگین
{
    Login *loginWindow = new Login();
    loginWindow->show();
    this->close();
}
