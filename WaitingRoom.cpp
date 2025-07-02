#include "WaitingRoom.h"
#include "UserPanel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPixmap>
#include <QMessageBox>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

WaitingRoom::WaitingRoom(QWidget *parent, QTcpSocket *socket, const QString &username)
    : QWidget(parent), socket(socket), username(username)
{
    setFixedSize(800, 600);
    setWindowTitle("Waiting Room - " + username);

    setupUI();

    if (socket->state() == QAbstractSocket::ConnectedState) {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: green;");
    } else {
        connectionStatusLabel->setText("Not connected");
        connectionStatusLabel->setStyleSheet("color: red;");
    }

    connect(socket, &QTcpSocket::readyRead, this, &WaitingRoom::onWaitingMessage);

    QJsonObject req;
    req["type"] = "nowadd";
    req["username"] = username;
    socket->write(QJsonDocument(req).toJson(QJsonDocument::Compact));
    socket->flush();
}

void WaitingRoom::setupUI()
{
    auto *bg = new QLabel(this);
    bg->setPixmap(QPixmap(":/images/images/1000025479.png"));
    bg->setScaledContents(true);
    bg->setFixedSize(800, 600);
    bg->lower();

    auto *mainL = new QVBoxLayout(this);
    mainL->setContentsMargins(30, 30, 30, 30);
    mainL->setSpacing(10);

    auto *tF = new QFrame(this);
    tF->setStyleSheet("background-color:#e8d4b0;border-radius:10px;");
    tF->setFixedHeight(80);
    auto *tL = new QVBoxLayout(tF);
    auto *title = new QLabel("🕒 Waiting for Players...", tF);
    title->setFont(QFont("Georgia", 24, QFont::Bold));
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#4e3b2b;");
    tL->addWidget(title);
    mainL->addWidget(tF);

    auto *lF = new QFrame(this);
    lF->setStyleSheet("background-color:#fceacb;border:2px solid #a67c52;border-radius:10px;");
    auto *lL = new QVBoxLayout(lF);
    lL->setContentsMargins(10, 10, 10, 10);

    playerListWidget = new QListWidget(this);
    playerListWidget->setFont(QFont("Georgia", 13));
    playerListWidget->setStyleSheet(
        "QListWidget {background:transparent;color:#4e3b2b;border:none;}"
        "QListWidget::item {padding:8px;}"
        "QListWidget::item:selected {background:#d2a679;color:white;}"
        );
    lL->addWidget(playerListWidget);
    mainL->addWidget(lF, 1);

    countLabel = new QLabel("Players: 0 / 4", this);
    countLabel->setFont(QFont("Georgia", 14, QFont::Bold));
    countLabel->setAlignment(Qt::AlignCenter);
    countLabel->setStyleSheet("color:#814040;");
    mainL->addWidget(countLabel);

    backButton = new QPushButton("Back", this);
    backButton->setFont(QFont("Georgia", 12, QFont::Bold));
    backButton->setFixedSize(100, 36);
    backButton->setStyleSheet(
        "QPushButton {background:#4e3b2b;color:#fceacb;border:2px solid #d2a679;border-radius:10px;}"
        "QPushButton:hover{background:#6b4c35;}"
        );
    auto *h = new QHBoxLayout;
    h->addWidget(backButton, 0, Qt::AlignLeft);
    h->addStretch();
    mainL->addLayout(h);

    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setFont(QFont("Georgia", 10, QFont::Bold));
    connectionStatusLabel->setAlignment(Qt::AlignCenter);
    mainL->addWidget(connectionStatusLabel, 0, Qt::AlignBottom);

    connect(backButton, &QPushButton::clicked, this, [=]() {
        disconnect(socket, &QTcpSocket::readyRead, this, &WaitingRoom::onWaitingMessage);
        this->hide();
        auto *up = new UserPanel(nullptr, socket, username);
        up->show();
        this->deleteLater();
    });
}

void WaitingRoom::onWaitingMessage()
{
    while (socket->bytesAvailable() > 0) {
        QByteArray data = socket->readAll();

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);

        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            qDebug() << "[WaitingRoom] Invalid JSON!";
            qDebug() << "[WaitingRoom] Raw Data:" << QString::fromUtf8(data);
            return;
        }

        QJsonObject obj = doc.object();
        QString type = obj["type"].toString();

        if (type == "Waiting_List_Update") {
            QStringList players;
            for (auto v : obj["waiting_players"].toArray())
                players << v.toString();

            playerListWidget->clear();
            playerListWidget->addItems(players);

            int c = obj["current_players_count"].toInt();
            int r = obj["required_players_count"].toInt();
            countLabel->setText(QString("Players: %1 / %2").arg(c).arg(r));
        }
        else if (type == "Game_Start") {
            QString msg = obj["message"].toString();
            QStringList pls;
            for (auto v : obj["players"].toArray())
                pls << v.toString();

            QMessageBox::information(this, "Game Starting", msg + "\n\nPlayers:\n" + pls.join("\n"));
        }
    }
}
