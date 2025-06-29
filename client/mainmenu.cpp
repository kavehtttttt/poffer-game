#include "MainMenu.h"
#include "signup.h"
#include "login.h"

#include <QFont>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QMessageBox>

MainMenu::MainMenu(QWidget *parent, QTcpSocket *socket)
    : QMainWindow(parent), socket(socket)
{
    setFixedSize(800, 600);
    setWindowTitle("Poffer Game");

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    backgroundLabel = new QLabel(centralWidget);
    backgroundLabel->setPixmap(QPixmap(":/images/images/1000025478.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setFixedSize(800, 600);

    // 🔹 برچسب وضعیت اتصال
    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setFont(QFont("Georgia", 10, QFont::Bold));
    connectionStatusLabel->setAlignment(Qt::AlignLeft);
    connectionStatusLabel->setGeometry(10, 10, 300, 20); // بالا سمت چپ
    connectionStatusLabel->setStyleSheet("color: white; background: transparent;");

    // مقدار اولیه‌ی وضعیت اتصال
    if (!socket) {
        connectionStatusLabel->setText("Socket is null!");
        connectionStatusLabel->setStyleSheet("color: red; font-weight: bold;");
    } else if (socket->state() == QAbstractSocket::ConnectedState) {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: lightgreen; font-weight: bold;");
    } else {
        connectionStatusLabel->setText("Connecting...");
        connectionStatusLabel->setStyleSheet("color: gray; font-weight: bold;");
    }

    QFont btnFont("Georgia", 16, QFont::Bold);

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

    // استایل برای دکمه خروج
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

    buttonLayout->setContentsMargins(30, 500, 30, 30);
    buttonLayout->setSpacing(20);
    buttonsWidget->setLayout(buttonLayout);

    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->addWidget(backgroundLabel);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralWidget->setLayout(centralLayout);

    // اتصال دکمه‌ها
    connect(exitButton, &QPushButton::clicked, this, &QWidget::close);
    connect(signupButton, &QPushButton::clicked, this, &MainMenu::openSignup);
    connect(loginButton, &QPushButton::clicked, this, &MainMenu::openLogin);

    // 🔹 به‌روزرسانی وضعیت اتصال با سیگنال‌های سوکت
    connect(socket, &QTcpSocket::connected, this, [this]() {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: lightgreen; font-weight: bold;");
    });

    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        connectionStatusLabel->setText("Disconnected from server");
        connectionStatusLabel->setStyleSheet("color: orange; font-weight: bold;");
    });

    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        connectionStatusLabel->setText("Connection error");
        connectionStatusLabel->setStyleSheet("color: red; font-weight: bold;");
    });
}

MainMenu::~MainMenu()
{
    if (socket) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }
}

void MainMenu::openSignup()
{
    this->hide();
    signup *signupWindow = new signup(nullptr, socket);
    signupWindow->show();
}

void MainMenu::openLogin()
{
    this->hide();
    Login *loginWindow = new Login(nullptr, socket);
    loginWindow->show();
}
