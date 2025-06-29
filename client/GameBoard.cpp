#include "GameBoard.h"
#include "CardWidget.h"
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QGraphicsView>
#include <QTimer>
#include <QMessageBox>

GameBoard::GameBoard(QWidget *parent, QTcpSocket* socket, QString* username, QStringList* otherPlayers, const QString& initialBuffer)
    : QWidget(parent), socket(socket), otherPlayers(otherPlayers), username(username), buffer("")
{
    qDebug() << "GameBoard created!";
    setFixedSize(900, 700);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QStringList sides = {"T", "R", "L", "B"};
    for (int i = 0; i < otherPlayers->size(); ++i) {
        playerToSide[otherPlayers->at(i)] = sides[i];
        sideToPlayer[sides[i]] = otherPlayers->at(i);
    }

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(5);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setFont(QFont("Georgia", 12, QFont::Bold));
    layout->addWidget(connectionStatusLabel);

    gameMessageLabel = new QLabel(this);
    gameMessageLabel->setFont(QFont("Georgia", 14, QFont::Bold));
    gameMessageLabel->setStyleSheet("color: yellow; background: #222; padding: 6px;");
    gameMessageLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(gameMessageLabel);

    setupScene();
    layout->addWidget(view);

    setLayout(layout);

    setupTopCards();
    setupBottomCards();
    setupLeftCards();
    setupRightCards();
    setupCenterCards();

    updateConnectionStatus();

    if (!initialBuffer.isEmpty()) {
        qDebug() << "Initial buffer is not empty, processing initial data.";
        processInitialData(initialBuffer);
    }

    connect(socket, &QTcpSocket::readyRead, this, [=]() mutable {
        qDebug() << "Socket readyRead triggered!";
        buffer += QString::fromUtf8(socket->readAll());
        int braceDepth = 0;
        int messageStart = -1;
        for (int i = 0; i < buffer.length(); ++i) {
            if (buffer[i] == '{') {
                if (braceDepth == 0) messageStart = i;
                ++braceDepth;
            }
            if (buffer[i] == '}') {
                --braceDepth;
                if (braceDepth == 0 && messageStart != -1) {
                    QString singleMessage = buffer.mid(messageStart, i - messageStart + 1);
                    qDebug() << "Received JSON message:" << singleMessage;
                    handleServerMessage(singleMessage);
                    i++;
                    buffer = buffer.mid(i);
                    i = -1;
                    messageStart = -1;
                }
            }
        }
    });
}

void GameBoard::processInitialData(const QString& data)
{
    qDebug() << "processInitialData called!";
    buffer += data;
    int braceDepth = 0;
    int messageStart = -1;
    for (int i = 0; i < buffer.length(); ++i) {
        if (buffer[i] == '{') {
            if (braceDepth == 0) messageStart = i;
            ++braceDepth;
        }
        if (buffer[i] == '}') {
            --braceDepth;
            if (braceDepth == 0 && messageStart != -1) {
                QString singleMessage = buffer.mid(messageStart, i - messageStart + 1);
                qDebug() << "Processing initial JSON message:" << singleMessage;
                handleServerMessage(singleMessage);
                i++;
                buffer = buffer.mid(i);
                i = -1;
                messageStart = -1;
            }
        }
    }
}

void GameBoard::setupScene()
{
    qDebug() << "Setting up scene...";
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, width(), height());

    view = new QGraphicsView(scene, this);
    view->setFixedSize(width(), height());
    view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    view->setStyleSheet("background-color: #007f00;");
    view->setFrameShape(QFrame::NoFrame);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void GameBoard::addCard(int x, int y, const QString &text, int rotation)
{
    qDebug() << "Adding card:" << text << "at" << x << "," << y << "rotation:" << rotation;
    auto *card = new CardWidget(text);
    card->setCardState(CardWidget::Hidden);
    cardMap[text] = card;

    QGraphicsProxyWidget* proxy = scene->addWidget(card);
    proxy->setTransformOriginPoint(30, 45);
    proxy->setRotation(rotation);

    if (rotation != 0)
        proxy->setPos(x + 45 - 30, y + 30 - 45);
    else
        proxy->setPos(x, y);
}

void GameBoard::setupTopCards()
{
    qDebug() << "Setting up top cards";
    int w = 60, gap = 20;
    int total = 5 * w + 4 * gap;
    int sx = width() / 2 - total / 2;
    int y = 20;

    for (int i = 0; i < 5; ++i)
        addCard(sx + i * (w + gap), y, QString("T%1").arg(i + 1));
}

void GameBoard::setupBottomCards()
{
    qDebug() << "Setting up bottom cards";
    int w = 60, h = 90, gap = 20;
    int total = 5 * w + 4 * gap;
    int sx = width() / 2 - total / 2;
    int y = height() - h - 20;

    for (int i = 0; i < 5; ++i)
        addCard(sx + i * (w + gap), y, QString("B%1").arg(i + 1));
}

void GameBoard::setupLeftCards()
{
    qDebug() << "Setting up left cards";
    int w = 60, gap = 20;
    int total = 5 * w + 4 * gap;
    int sy = height() / 2 - total / 2;
    int x = 30;

    for (int i = 0; i < 5; ++i)
        addCard(x, sy + i * (w + gap), QString("L%1").arg(i + 1), 90);
}

void GameBoard::setupRightCards()
{
    qDebug() << "Setting up right cards";
    int w = 60, h = 90, gap = 20;
    int total = 5 * w + 4 * gap;
    int sy = height() / 2 - total / 2;
    int x = width() - 30 - h;

    for (int i = 0; i < 5; ++i)
        addCard(x, sy + i * (w + gap), QString("R%1").arg(i + 1), -90);
}

void GameBoard::setupCenterCards()
{
    qDebug() << "Setting up center cards";
    int w = 60, h = 90, gap = 15;
    int total = 7 * w + 6 * gap;
    int sx = width() / 2 - total / 2;
    int y = height() / 2 - h / 2;

    for (int i = 0; i < 7; ++i)
        addCard(sx + i * (w + gap), y, QString("C%1").arg(i + 1));
}

void GameBoard::updateConnectionStatus()
{
    if (!socket) {
        connectionStatusLabel->setText("Socket is null!");
        connectionStatusLabel->setStyleSheet("color: red;");
        qDebug() << "Socket is null!";
    } else if (socket->state() == QAbstractSocket::ConnectedState) {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: lightgreen;");
        qDebug() << "Connected to server!";
    } else {
        connectionStatusLabel->setText("Connecting...");
        connectionStatusLabel->setStyleSheet("color: gray;");
        qDebug() << "Connecting...";
    }
}

void GameBoard::handleServerMessage(const QString& message) {
    qDebug() << "handleServerMessage called with message:" << message;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) {
        qDebug() << "Message is not a valid JSON object!";
        return;
    }

    QJsonObject serverData = doc.object();
    QString type = serverData.value("type").toString();
    qDebug() << "Message type:" << type;

    // پیام مقایسه چهار کارت
    if (type == "Starting_Player_Comparison_Cards") {
        qDebug() << "Processing Starting_Player_Comparison_Cards";
        QVector<PlayerCardInfo> cardInfos;
        if (serverData.contains("cards_dealt") && serverData["cards_dealt"].isArray()) {
            QJsonArray arr = serverData["cards_dealt"].toArray();
            QVector<QString> sides = {"T", "R", "L", "B"};
            for (int i = 0; i < arr.size() && i < sides.size(); ++i) {
                QJsonObject obj = arr[i].toObject();
                PlayerCardInfo info;
                info.username = obj["username"].toString();
                info.side = sides[i];
                QJsonObject cardObj = obj["card"].toObject();
                info.suit = cardObj["suit"].toInt();
                info.rank = cardObj["rank"].toInt();
                cardInfos.append(info);
            }
        }
        revealThirdCardsSequentially(cardInfos);
        return;
    }

    // پیام تعیین برنده (شروع کننده)
    if (type == "Starting_Player_Determined") {
        qDebug() << "Processing Starting_Player_Determined";
        QString winner = serverData.value("winner").toString();
        if (!winner.isEmpty()) {
            if (username && *username == winner) {
                gameMessageLabel->setText(tr("شما شروع‌کننده این دور هستید!"));
                gameMessageLabel->setStyleSheet("color: lightgreen; background: #333; padding: 8px; font-size: 18px;");
                qDebug() << "You are the starter!";
            } else {
                gameMessageLabel->setText(tr("%1 شروع‌کننده این دور است.").arg(winner));
                gameMessageLabel->setStyleSheet("color: yellow; background: #333; padding: 8px; font-size: 16px;");
                qDebug() << winner << "is the starter!";
            }
            gameMessageLabel->show();
        }
        return;
    }

    // اطمینان از اینکه پیام واقعاً برای این یوزر است (اگر username در پیام هست)
    if (serverData.contains("username")) {
        QString msgUser = serverData.value("username").toString();
        if (!username || *username != msgUser) {
            qDebug() << "Message is not for this user, skipping!";
            return;
        }
    }

    // بافر یا پردازش پیام
    if (!readyForCardMessages) {
        if (type == "Initial_Hand") {
            qDebug() << "Buffering Initial_Hand message";
            bufferedInitialHandMessages.append(serverData);
        } else if (type == "Your_Turn") {
            qDebug() << "Buffering Your_Turn message";
            bufferedYourTurnMessages.append(serverData);
        }
        return;
    }

    // آماده‌ایم، مستقیم پردازش کن
    if (type == "Initial_Hand") {
        qDebug() << "Processing Initial_Hand message";
        processInitialHandMessage(serverData);
    } else if (type == "Your_Turn") {
        qDebug() << "Processing Your_Turn message";
        processYourTurnMessage(serverData);
    }
}

void GameBoard::handleServerMessage(const QString& message)


void GameBoard::revealThirdCardsSequentially(const QVector<PlayerCardInfo>& cardInfos) {
    qDebug() << "revealThirdCardsSequentially called!";
    QVector<QString> revealOrder = {"B", "R", "T", "L"};
    QVector<int> infoIndexes;
    for (const QString& side : revealOrder) {
        for (int i = 0; i < cardInfos.size(); ++i) {
            if (cardInfos[i].side == side) {
                infoIndexes.append(i);
                break;
            }
        }
    }
    for (int idx = 0; idx < infoIndexes.size(); ++idx) {
        int i = infoIndexes[idx];
        QTimer::singleShot(idx * 1000, this, [=]() {
            QString key = cardInfos[i].side + "3";
            qDebug() << "Revealing card:" << key;
            if (cardMap.contains(key)) {
                CardWidget* card = static_cast<CardWidget*>(cardMap[key]);
                card->setCardState(CardWidget::Normal);

                int rank = cardInfos[i].rank;
                int suit = cardInfos[i].suit;
                QString suitStr;
                switch (suit) {
                case 1: suitStr = "♦"; break;
                case 2: suitStr = "♣"; break;
                case 3: suitStr = "♥"; break;
                case 4: suitStr = "♠"; break;
                default: suitStr = QString::number(suit);
                }
                card->setCardText(QString("%1\n%2").arg(suitStr).arg(rank));
            }
        });
    }
    QTimer::singleShot(4000, this, [=]() {
        qDebug() << "Hiding all third cards";
        for (const QString& side : revealOrder) {
            QString key = side + "3";
            if (cardMap.contains(key)) {
                CardWidget* card = static_cast<CardWidget*>(cardMap[key]);
                card->setCardState(CardWidget::Hidden);
            }
        }
        readyForCardMessages = true;
        processBufferedCardMessages();
    });
}

void GameBoard::processBufferedCardMessages() {
    qDebug() << "processBufferedCardMessages called!";
    for (const QJsonObject& obj : bufferedInitialHandMessages) {
        qDebug() << "Processing buffered Initial_Hand message";
        processInitialHandMessage(obj);
    }
    bufferedInitialHandMessages.clear();

    for (const QJsonObject& obj : bufferedYourTurnMessages) {
        qDebug() << "Processing buffered Your_Turn message";
        processYourTurnMessage(obj);
    }
    bufferedYourTurnMessages.clear();
}

void GameBoard::processInitialHandMessage(const QJsonObject& obj) {
    qDebug() << "processInitialHandMessage called!";
    gameMessageLabel->setText("دست اولیه شما دریافت شد.");
    gameMessageLabel->setStyleSheet("color: orange; background: #333; padding: 8px;");
    gameMessageLabel->show();
    // QMessageBox::information(this, "دست اولیه", "دست اولیه شما دریافت شد.");
}

void GameBoard::processYourTurnMessage(const QJsonObject& obj) {
    qDebug() << "processYourTurnMessage called!";
    gameMessageLabel->setText("نوبت شماست!");
    gameMessageLabel->setStyleSheet("color: green; background: #333; padding: 8px;");
    gameMessageLabel->show();

    if (obj.contains("cards_in_hand") && obj["cards_in_hand"].isArray()) {
        QJsonArray arr = obj["cards_in_hand"].toArray();
        for (int i = 0; i < arr.size() && i < 7; ++i) {
            QJsonObject cardObj = arr[i].toObject();
            int rank = cardObj["rank"].toInt();
            int suit = cardObj["suit"].toInt();
            QString suitStr;
            switch (suit) {
            case 1: suitStr = "♦"; break;
            case 2: suitStr = "♣"; break;
            case 3: suitStr = "♥"; break;
            case 4: suitStr = "♠"; break;
            default: suitStr = QString::number(suit);
            }
            QString key = QString("C%1").arg(i + 1);
            qDebug() << "Updating card:" << key << "with rank:" << rank << "and suit:" << suitStr;
            if (cardMap.contains(key)) {
                CardWidget* card = static_cast<CardWidget*>(cardMap[key]);
                card->setCardState(CardWidget::Normal);
                card->setCardText(QString("%1\n%2").arg(suitStr).arg(rank));
            }
        }
    }
    // QMessageBox::information(this, "نوبت شما", "نوبت شماست!");
}

