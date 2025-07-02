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
#include "GameBoard.h"
#include "qjsonarray.h"

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
        QByteArray data = socket->readAll().trimmed();
        int startIndex = 0;

        while (startIndex < data.size()) {
            int openBraces = 0;
            int endIndex = -1;

            // Find a complete JSON message
            for (int i = startIndex; i < data.size(); ++i) {
                if (data[i] == '{') openBraces++;
                else if (data[i] == '}') openBraces--;

                if (openBraces == 0 && data[startIndex] == '{') {
                    endIndex = i;
                    break;
                }
            }

            if (endIndex == -1) break; // Wait for more data if the message is incomplete

            // Extract the complete JSON message
            QByteArray jsonData = data.mid(startIndex, endIndex - startIndex + 1);
            startIndex = endIndex + 1;

            // Parse the JSON message
            QJsonParseError err;
            QJsonDocument doc = QJsonDocument::fromJson(jsonData, &err);
            if (err.error != QJsonParseError::NoError || !doc.isObject()) {
                qDebug() << "Invalid JSON received:" << jsonData;
                continue;
            }

            QJsonObject obj = doc.object();
            QString type = obj["type"].toString();

            // Handle Get_History message
            if (type == "Get_History") {
                QString status = obj["status"].toString();
                if (status != "success") {
                    QMessageBox::warning(this, "Error", obj["message"].toString());
                    return;
                }

                // Process game history
                QJsonArray historyArray = obj["history"].toArray();
                QString allHistoryDetails;

                for (const QJsonValue &val : historyArray) {
                    QJsonObject entry = val.toObject();

                    QString dateOfPlay = entry["date_of_play"].toString();
                    QString finalResult = entry["final_result"].toString();
                    QString opponents = entry["opponent_username"].toString();
                    QJsonArray roundResults = entry["round_results"].toArray();

                    QString roundDetails;
                    for (const QJsonValue &round : roundResults) {
                        roundDetails += round.toString() + ", ";
                    }
                    if (!roundDetails.isEmpty()) {
                        roundDetails.chop(2); // Remove extra comma and space
                    }

                    QString historyItem = QString("Date: %1\nResult: %2\nOpponents: %3\nRounds: %4\n\n")
                                              .arg(dateOfPlay)
                                              .arg(finalResult)
                                              .arg(opponents)
                                              .arg(roundDetails);

                    allHistoryDetails += historyItem; // Collect all game details
                }

                // Show all game details in a message box
                QMessageBox::information(this, "Game History", allHistoryDetails);

            } else if (type == "Game_Start") {
                qDebug() << "Processing Game_Start message";

                // Extract players list from the Game_Start message
                QStringList playersList;
                if (obj.contains("players_in_game") && obj["players_in_game"].isArray()) {
                    QJsonArray arr = obj["players_in_game"].toArray();
                    for (const QJsonValue &val : arr)
                        playersList.append(val.toString());
                }

                // Display a message to indicate the game is starting
                QString infoText = "✅ بازی شروع شد!\n\n👥 بازیکنان:\n";
                for (const QString &player : playersList)
                    infoText += "• " + player + "\n";

                QMessageBox::information(this, "Game Starting", infoText);

                // Create and show the GameBoard page
                this->hide();
                auto *gameWindow = new GameBoard(nullptr, socket, username, &playersList, "");
                gameWindow->show();
                this->deleteLater();
                return;

            } else {
                qDebug() << "Unhandled message type:" << type;
            }
        }
    }
}
void UserPanel::onHistoryClicked()
{
    // بررسی اتصال به سرور
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        QMessageBox::warning(this, "Error", "Not connected to server.");
        return;
    }

    // ایجاد پیام JSON برای درخواست تاریخچه
    QJsonObject req;
    req["type"] = "Get_History";
    req["username"] = username;

    QByteArray reqData = QJsonDocument(req).toJson(QJsonDocument::Compact) + "\n";

    // ارسال پیام به سرور
    socket->write(reqData);
    socket->flush();

    qDebug() << "Sent Get_History request:" << QString::fromUtf8(reqData);
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
