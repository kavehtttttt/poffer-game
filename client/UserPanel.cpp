#include "UserPanel.h"
#include "EditInfo.h"
#include "HistoryView.h"
#include "MainMenu.h"

#include <QFont>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

UserPanel::UserPanel(QWidget *parent, QTcpSocket *socket, const QString &username)
    : QMainWindow(parent), socket(socket), username(username)
{
    setupUI();
    updateConnectionStatus();

    connect(playButton, &QPushButton::clicked, this, &UserPanel::onPlayGameClicked);
    connect(historyButton, &QPushButton::clicked, this, &UserPanel::onHistoryClicked);
    connect(editInfoButton, &QPushButton::clicked, this, &UserPanel::onEditInfoClicked);
    connect(exitButton, &QPushButton::clicked, this, &UserPanel::onExitAccountClicked);

    if (socket) {
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
}

UserPanel::~UserPanel()
{
    // Clean up if needed
}

void UserPanel::setupUI()
{
    setFixedSize(800, 600);
    setWindowTitle("Welcome, " + username);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    backgroundLabel = new QLabel(centralWidget);
    backgroundLabel->setPixmap(QPixmap(":/images/images/1000025478.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setFixedSize(800, 600);

    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setFont(QFont("Georgia", 10, QFont::Bold));
    connectionStatusLabel->setAlignment(Qt::AlignLeft);
    connectionStatusLabel->setGeometry(10, 10, 300, 20);
    connectionStatusLabel->setStyleSheet("color: white; background: transparent;");

    QFont btnFont("Georgia", 16, QFont::Bold);

    playButton = new QPushButton("PLAY GAME");
    historyButton = new QPushButton("HISTORY");
    editInfoButton = new QPushButton("EDIT INFO");
    exitButton = new QPushButton("LOG OUT");

    QPushButton* buttons[] = {playButton, historyButton, editInfoButton};

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

    QString exitBtnStyle =
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
        "}";

    for (QPushButton* btn : buttons) {
        btn->setFont(btnFont);
        btn->setStyleSheet(btnStyle);
    }

    exitButton->setFont(btnFont);
    exitButton->setStyleSheet(exitBtnStyle);

    QWidget *buttonsWidget = new QWidget(backgroundLabel);
    buttonsWidget->setFixedSize(800, 600);

    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonsWidget);
    buttonLayout->addStretch();
    buttonLayout->addWidget(playButton);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(historyButton);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(editInfoButton);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addStretch();

    buttonLayout->setContentsMargins(20, 500, 20, 30);
    buttonLayout->setSpacing(15);
    buttonsWidget->setLayout(buttonLayout);

    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->addWidget(backgroundLabel);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralWidget->setLayout(centralLayout);
}

void UserPanel::updateConnectionStatus()
{
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
}

void UserPanel::onPlayGameClicked()
{
    QMessageBox::information(this, "Play Game", "You clicked Play Game.");
}

void UserPanel::onHistoryClicked()
{
    this->hide();
    HistoryView* historyview = new  HistoryView(nullptr, socket, username);
    historyview->show();
    this->deleteLater();
}

void UserPanel::onEditInfoClicked()
{
    this->hide();
    EditInfo* editinfo = new EditInfo(nullptr, socket, username);
    editinfo->show();
    this->deleteLater();
}

void UserPanel::onExitAccountClicked()
{
    if (!socket) {
        QMessageBox::warning(this, "Error", "Socket not available.");
        return;
    }

    // ساخت پیام logout به صورت JSON
    QJsonObject json;
    json["type"] = "logout";
    json["username"] = username;

    QJsonDocument doc(json);
    socket->write(doc.toJson(QJsonDocument::Compact));
    socket->flush();

    // غیر فعال کردن دکمه‌ها تا دریافت پاسخ
    playButton->setEnabled(false);
    historyButton->setEnabled(false);
    editInfoButton->setEnabled(false);
    exitButton->setEnabled(false);

    // اتصال به readyRead برای دریافت پاسخ سرور (با Qt::UniqueConnection که چند اتصال نداشته باشیم)
    connect(socket, &QTcpSocket::readyRead, this, &UserPanel::handleLogoutResponse, Qt::UniqueConnection);
}

void UserPanel::handleLogoutResponse()
{
    QByteArray responseData = socket->readAll();

    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (!doc.isObject()) {
        QMessageBox::warning(this, "Server Error", "Invalid response from server.");
        // فعال کردن دکمه‌ها مجدد
        playButton->setEnabled(true);
        historyButton->setEnabled(true);
        editInfoButton->setEnabled(true);
        exitButton->setEnabled(true);
        return;
    }

    QJsonObject obj = doc.object();

    if (obj.contains("status") && obj.contains("message")) {
        QString status = obj["status"].toString();
        QString message = obj["message"].toString();

        if (status == "success") {
            QMessageBox::information(this, "Logout", message);

            // باز کردن صفحه MainMenu
            this->hide();
            MainMenu* mainmenu = new MainMenu(nullptr, socket);
            mainmenu->show();
            this->deleteLater();
        } else {
            QMessageBox::warning(this, "Logout Failed", message);

            // فعال کردن دکمه‌ها مجدد
            playButton->setEnabled(true);
            historyButton->setEnabled(true);
            editInfoButton->setEnabled(true);
            exitButton->setEnabled(true);
        }
    } else {
        QMessageBox::warning(this, "Server Error", "Unexpected response format.");

        // فعال کردن دکمه‌ها مجدد
        playButton->setEnabled(true);
        historyButton->setEnabled(true);
        editInfoButton->setEnabled(true);
        exitButton->setEnabled(true);
    }

    // جدا کردن کانکت پس از دریافت پاسخ
    disconnect(socket, &QTcpSocket::readyRead, this, &UserPanel::handleLogoutResponse);
}
