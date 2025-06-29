#include "HistoryView.h"
#include "UserPanel.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QPixmap>
#include <QHBoxLayout>
#include <QFrame>

HistoryView::HistoryView(QWidget *parent, QTcpSocket *socket, const QString &username)
    : QWidget(parent), socket(socket), username(username)
{
    setFixedSize(800, 600);
    setWindowTitle("Game History - " + username);

    setupUI();
    sendHistoryRequest();

    if (socket && socket->state() == QAbstractSocket::ConnectedState) {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: green;");
    } else {
        connectionStatusLabel->setText("Not connected");
        connectionStatusLabel->setStyleSheet("color: red;");
    }

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
    layout->addWidget(listFrame, 1); // به عنوان بالا چسب

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

    // وضعیت اتصال در پایین‌ترین قسمت
    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setAlignment(Qt::AlignCenter);
    connectionStatusLabel->setFont(QFont("Georgia", 10, QFont::Bold));
    layout->addWidget(connectionStatusLabel, 0, Qt::AlignBottom);
}

void HistoryView::sendHistoryRequest()
{
    QJsonObject json;
    json["type"] = "Get_History";
    json["username"] = username;

    QJsonDocument doc(json);
    socket->write(doc.toJson(QJsonDocument::Compact));
    socket->flush();
}

void HistoryView::onDataReceived()
{
    QByteArray data = socket->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        QMessageBox::warning(this, "Error", "Invalid data received from server.");
        return;
    }

    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();
    if (type != "Get_History") return;

    QString status = obj["status"].toString();
    if (status != "success") {
        QMessageBox::warning(this, "Error", obj["message"].toString());
        return;
    }

    historyListWidget->clear();
    QJsonArray historyArray = obj["history"].toArray();

    for (const QJsonValue &val : historyArray) {
        QJsonObject entry = val.toObject();
        QString date = entry["date"].toString();
        QString result = entry["result"].toString();
        int score = entry["score"].toInt();

        QString line = QString("🕓 %1 | %2 | 🎯 Score: %3")
                           .arg(date)
                           .arg(result)
                           .arg(score);

        historyListWidget->addItem(line);
    }
}

void HistoryView::goBack()
{
    this->hide();
    UserPanel *panel = new UserPanel(nullptr, socket, username);
    panel->show();
    this->deleteLater();
}
