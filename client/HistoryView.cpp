#include "HistoryView.h"
#include "UserPanel.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QPixmap>
#include <QHBoxLayout>
#include <QFrame>

HistoryView::HistoryView(QWidget *parent, QTcpSocket *socket, const QString &username, const QJsonObject &gameHistory)
    : QWidget(parent), socket(socket), username(username), gameHistory(gameHistory)
{
    setFixedSize(800, 600);
    setWindowTitle("Game History - " + username);

    setupUI();

    // Automatically send the Get_History request when the page is opened
    sendHistoryRequest();

    // Check and display the connection status
    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: green;");
    } else {
        connectionStatusLabel->setText("Not connected");
        connectionStatusLabel->setStyleSheet("color: red;");
    }

    // Connect signals for socket data and back button
    connect(backButton, &QPushButton::clicked, this, &HistoryView::goBack);
    connect(socket, &QTcpSocket::readyRead, this, &HistoryView::onDataReceived);
}

void HistoryView::setupUI()
{
    QLabel *backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(QPixmap(":/images/images/1000025479.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setFixedSize(800, 600);
    backgroundLabel->lower();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(10);

    QFrame *titleFrame = new QFrame(this);
    titleFrame->setStyleSheet("background-color: #e8d4b0; border-radius: 10px;");
    titleFrame->setFixedHeight(80);

    QVBoxLayout *titleLayout = new QVBoxLayout(titleFrame);
    QLabel *title = new QLabel("Your Game History", titleFrame);
    title->setFont(QFont("Georgia", 24, QFont::Bold));
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: #4e3b2b;");
    titleLayout->addWidget(title);

    layout->addWidget(titleFrame);

    QFrame *listFrame = new QFrame(this);
    listFrame->setStyleSheet("background-color: #fceacb; border: 2px solid #a67c52; border-radius: 10px;");
    QVBoxLayout *listLayout = new QVBoxLayout(listFrame);
    listLayout->setContentsMargins(10, 10, 10, 10);

    historyListWidget = new QListWidget(this);
    historyListWidget->setFont(QFont("Georgia", 13));
    historyListWidget->setStyleSheet(
        "QListWidget { background-color: transparent; color: #4e3b2b; border: none; }"
        "QListWidget::item { padding: 8px; }"
        "QListWidget::item:selected { background-color: #d2a679; color: white; }"
        );

    listLayout->addWidget(historyListWidget);
    layout->addWidget(listFrame, 1);

    backButton = new QPushButton("Back", this);
    backButton->setFixedSize(100, 36);
    backButton->setFont(QFont("Georgia", 12, QFont::Bold));
    backButton->setStyleSheet(
        "QPushButton { background-color: #4e3b2b; color: #fceacb; border: 2px solid #d2a679; "
        "border-radius: 10px; font-weight: bold; }"
        "QPushButton:hover { background-color: #6b4c35; }"
        );

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(backButton, 0, Qt::AlignLeft);
    bottomLayout->addStretch();
    layout->addLayout(bottomLayout);

    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setAlignment(Qt::AlignCenter);
    connectionStatusLabel->setFont(QFont("Georgia", 10, QFont::Bold));
    layout->addWidget(connectionStatusLabel, 0, Qt::AlignBottom);
}

void HistoryView::sendHistoryRequest()
{
    // Create and send the Get_History request with the username
    QJsonObject json;
    json["type"] = "Get_History";
    json["username"] = username;

    QJsonDocument doc(json);
    socket->write(doc.toJson(QJsonDocument::Compact));
    socket->flush();
}

void HistoryView::onDataReceived()
{
    static QByteArray buffer; // Buffer to store partial data
    QByteArray newData = socket->readAll();
    buffer += newData; // Append new data to the buffer

    // Print raw data for debugging
    qDebug() << "Raw data received:" << newData;

    while (true) {
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(buffer, &parseError);

        if (parseError.error == QJsonParseError::NoError) {
            // Successfully parsed a complete JSON message
            QJsonObject obj = doc.object();
            buffer.remove(0, doc.toJson(QJsonDocument::Compact).size()); // Remove processed message from the buffer

            // Process JSON message...
        } else if (parseError.error == QJsonParseError::UnterminatedObject) {
            // Wait for more data (message is incomplete)
            break;
        } else {
            // Show invalid JSON data and error
            QMessageBox::warning(this, "Invalid JSON",
                                 "Raw Data Received:\n" + QString::fromUtf8(buffer) +
                                     "\nError: " + parseError.errorString());
            buffer.clear(); // Clear the buffer to avoid future errors
            break;
        }
    }
}

void HistoryView::goBack()
{
    // Hide current view and show UserPanel
    this->hide();
    UserPanel *panel = new UserPanel(nullptr, socket, username);
    panel->show();
    this->deleteLater();
}

void HistoryView::displayHistory()
{
    // بررسی اینکه آیا تاریخچه بازی وجود دارد یا خیر
    if (gameHistory.isEmpty()) {
        QMessageBox::warning(this, "No History", "No game history data available.");
        return;
    }

    QJsonArray historyArray = gameHistory["history"].toArray();
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
            roundDetails.chop(2); // حذف کاما و فاصله اضافی
        }

        QString historyItem = QString("Date: %1\nResult: %2\nOpponents: %3\nRounds: %4\n\n")
                                  .arg(dateOfPlay)
                                  .arg(finalResult)
                                  .arg(opponents)
                                  .arg(roundDetails);

        allHistoryDetails += historyItem; // جمع‌آوری جزئیات همه بازی‌ها
    }

    // نمایش تمام جزئیات در یک پیام‌ باکس
    QMessageBox::information(this, "Game History", allHistoryDetails);
}
