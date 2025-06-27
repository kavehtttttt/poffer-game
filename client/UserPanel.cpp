#include "UserPanel.h"
#include "EditInfo.h"
#include "HistoryView.h"
#include "MainMenu.h"
#include "WaitingRoom.h"
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

        // اتصال عمومی برای دریافت همه پیام‌ها از جمله Game_Start
        connect(socket, &QTcpSocket::readyRead, this, &UserPanel::handleServerMessage);
    }
}

UserPanel::~UserPanel() {}

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
        "QPushButton { background-color: #4e3b2b; color: #fceacb; border: 2px solid #d2a679; "
        "border-radius: 20px; min-width: 150px; min-height: 60px; font-weight: bold; letter-spacing: 1px; }"
        "QPushButton:hover { background-color: #6b4c35; border: 2px solid #e6c27a; }"
        "QPushButton:pressed { background-color: #3a2a1e; border-style: inset; }";

    QString exitBtnStyle =
        "QPushButton { background-color: #8B0000; color: #fff2e6; border: 2px solid #b30000; "
        "border-radius: 20px; min-width: 150px; min-height: 60px; font-weight: bold; letter-spacing: 1px; }"
        "QPushButton:hover { background-color: #a00000; }"
        "QPushButton:pressed { background-color: #600000; border-style: inset; }";

    for (QPushButton* btn : buttons)
        btn->setFont(btnFont), btn->setStyleSheet(btnStyle);

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
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "Error", "Not connected to server.");
        return;
    }

    QJsonObject req;
    req["type"] = "start_game";
    req["username"] = username;

    QByteArray reqData = QJsonDocument(req).toJson(QJsonDocument::Compact) + "\n";
    socket->write(reqData);
    socket->flush();
}

void UserPanel::handleServerMessage()
{
    while (socket->bytesAvailable()) {
        QByteArray data = socket->readLine().trimmed(); // فرض بر اینه که سرور با \n جدا می‌کنه
        if (data.isEmpty()) continue;

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject())
            continue;

        QJsonObject obj = doc.object();
        QString type = obj["type"].toString();
        QString status = obj["status"].toString();
        QString message = obj["message"].toString();

        if (type == "start_game") {
            // فقط نمایش پیام بدون رفتن به صفحه‌ی جدید
            QMessageBox::information(this, "Start Game", message);
        }
        else if (type == "Game_Start") {
            // نمایش پیام شروع بازی و رفتن به WaitingRoom
            QMessageBox::information(this, "Game Starting", message);
            this->hide();
            auto *wr = new WaitingRoom(nullptr, socket, username);
            wr->show();
            this->deleteLater();
            return; // دیگر ادامه نده چون this نابود شده
        }
        else if (type == "error") {
            QMessageBox::warning(this, "Error", message);
        }
    }
}

void UserPanel::onHistoryClicked()
{
    this->hide();
    auto *history = new HistoryView(nullptr, socket, username);
    history->show();
    this->deleteLater();
}

void UserPanel::onEditInfoClicked()
{
    this->hide();
    auto *edit = new EditInfo(nullptr, socket, username);
    edit->show();
    this->deleteLater();
}

void UserPanel::onExitAccountClicked()
{
    this->hide();
    auto *menu = new MainMenu(nullptr, socket);
    menu->show();
    this->deleteLater();
}
