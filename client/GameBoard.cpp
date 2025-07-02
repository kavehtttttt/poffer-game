#include "GameBoard.h"
#include "CardWidget.h"
#include "UserPanel.h"
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

GameBoard::GameBoard(QWidget *parent, QTcpSocket* socket, QString& username, QStringList* otherPlayers, const QString& initialBuffer)
    : QWidget(parent), socket(socket), otherPlayers(otherPlayers), username(username), buffer("")
{
    qDebug() << "GameBoard created!";
    setFixedSize(900, 700);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // تنظیم استایل CSS برای تصویر پس‌زمینه
    this->setStyleSheet("background-image: url(:/images/images/1000027315.jpg); background-repeat: no-repeat; background-position: center;");

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

    // Connect the cardClicked signal to the handleCardClick slot
    connect(card, &CardWidget::cardClicked, this, &GameBoard::handleCardClick);

    QGraphicsProxyWidget* proxy = scene->addWidget(card);
    proxy->setTransformOriginPoint(30, 45);
    proxy->setRotation(rotation);

    if (rotation != 0)
        proxy->setPos(x + 45 - 30, y + 30 - 45);
    else
        proxy->setPos(x, y);
}

void GameBoard::handleCardClick(const QString& cardText)
{
    qDebug() << "Card clicked:" << cardText;

    // استخراج اطلاعات کارت از متن کارت
    QStringList cardDetails = cardText.split('\n');
    if (cardDetails.size() != 2) {
        qWarning() << "Invalid card text format!";
        return;
    }

    int suit = cardDetails[0].toInt();
    int rank = cardDetails[1].toInt();

    // ساخت پیام JSON
    QJsonObject cardObj{
        {"suit", suit},
        {"rank", rank}
    };
    QJsonObject message{
        {"type", "Player_Selected_Card"},
        {"username", username},
        {"card", cardObj}
    };

    QJsonDocument doc(message);
    QString jsonString = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    qDebug() << "Sending JSON to server:" << jsonString;
    socket->write(jsonString.toUtf8());

    // gameMessageLabel->setText("You selected: Suit: " + QString::number(suit) + ", Rank: " + QString::number(rank));
    // gameMessageLabel->setStyleSheet("color: green; background: #ddd; padding: 6px;");
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
    int x = 100; // مقدار قبلی 30 بود، افزایش به سمت مرکز

    for (int i = 0; i < 5; ++i)
        addCard(x, sy + i * (w + gap), QString("L%1").arg(i + 1), 90);
}

void GameBoard::setupRightCards()
{
    qDebug() << "Setting up right cards";
    int w = 60, h = 90, gap = 20;
    int total = 5 * w + 4 * gap;
    int sy = height() / 2 - total / 2;
    int x = width() - 100 - h; // مقدار قبلی width() - 30 - h بود، کاهش به سمت مرکز

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
        QMessageBox::warning(this, "پیام نامعتبر", "پیامی دریافت شد که JSON معتبر نیست!");
        return;
    }

    QJsonObject serverData = doc.object();
    QString type = serverData.value("type").toString();
    qDebug() << "Message type:" << type;


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

    if (type == "Starting_Player_Determined") {
        qDebug() << "Processing Starting_Player_Determined";
        QString winner = serverData.value("winner").toString();
        if (!winner.isEmpty()) {
            QString message = (username  == winner)
            ? "شما شروع‌کننده این دور هستید!"
            : QString("%1 شروع‌کننده این دور است.").arg(winner);
            //QMessageBox::information(this, "شروع‌کننده", message);
            gameMessageLabel->setText(message);
            gameMessageLabel->setStyleSheet("color: lightgreen; background: #333; padding: 8px; font-size: 18px;");
            gameMessageLabel->show();
        }
        return;
    }

    if (type == "Initial_Hand") {
        qDebug() << "Processing Initial_Hand message";
        processInitialHandMessage(serverData);
        return;
    }

    if (type == "Your_Turn") {
        qDebug() << "Processing Your_Turn message";

        if (serverData.contains("cards_in_hand") && serverData["cards_in_hand"].isArray()) {
            QJsonArray cardsArray = serverData["cards_in_hand"].toArray();

            for (int i = 0; i < cardsArray.size() && i < 7; ++i) {
                QJsonObject cardObj = cardsArray[i].toObject();
                int rank = cardObj["rank"].toInt();
                int suit = cardObj["suit"].toInt();

                QString key = QString("C%1").arg(i + 1);
                if (cardMap.contains(key)) {
                    CardWidget* card = static_cast<CardWidget*>(cardMap[key]);
                    card->setCardState(CardWidget::Normal);
                    card->setCardText(QString("%1\n%2").arg(suit).arg(rank));
                    if(suit == 0 && rank == 2) card->setCardImage(":/images/images/Dimond-2.JPG");
                    if(suit == 0 && rank == 3) card->setCardImage(":/images/images/Dimond-3.JPG");
                    if(suit == 0 && rank == 4) card->setCardImage(":/images/images/Dimond-4.JPG");
                    if(suit == 0 && rank == 5) card->setCardImage(":/images/images/Dimond-5.JPG");
                    if(suit == 0 && rank == 6) card->setCardImage(":/images/images/Dimond-6.JPG");
                    if(suit == 0 && rank == 7) card->setCardImage(":/images/images/Dimond-7.JPG");
                    if(suit == 0 && rank == 8) card->setCardImage(":/images/images/Dimond-8.JPG");
                    if(suit == 0 && rank == 9) card->setCardImage(":/images/images/Dimond-9.JPG");
                    if(suit == 0 && rank == 10) card->setCardImage(":/images/images/Dimond-10.JPG");
                    if(suit == 0 && rank == 11) card->setCardImage(":/images/images/Dimond-Soldier.JPG");
                    if(suit == 0 && rank == 12) card->setCardImage(":/images/images/Dimond-Queen.JPG");
                    if(suit == 0 && rank == 13) card->setCardImage(":/images/images/Dimond-King.JPG");
                    if(suit == 0 && rank == 14) card->setCardImage(":/images/images/Dimond-Bitcoin.JPG");
                    if(suit == 1 && rank == 2) card->setCardImage(":/images/images/Dollar-2.JPG");
                    if(suit == 1 && rank == 3) card->setCardImage(":/images/images/Dollar-3.JPG");
                    if(suit == 1 && rank == 4) card->setCardImage(":/images/images/Dollar-4.JPG");
                    if(suit == 1 && rank == 5) card->setCardImage(":/images/images/Dollar-5.JPG");
                    if(suit == 1 && rank == 6) card->setCardImage(":/images/images/Dollar-6.JPG");
                    if(suit == 1 && rank == 7) card->setCardImage(":/images/images/Dollar-7.JPG");
                    if(suit == 1 && rank == 8) card->setCardImage(":/images/images/Dollar-8.JPG");
                    if(suit == 1 && rank == 9) card->setCardImage(":/images/images/Dollar-9.JPG");
                    if(suit == 1 && rank == 10) card->setCardImage(":/images/images/Dollar-10.JPG");
                    if(suit == 1 && rank == 11) card->setCardImage(":/images/images/Dollar-Soldier.JPG");
                    if(suit == 1 && rank == 12) card->setCardImage(":/images/images/Dollar-Queen.JPG");
                    if(suit == 1 && rank == 13) card->setCardImage(":/images/images/Dollar-King.JPG");
                    if(suit == 1 && rank == 14) card->setCardImage(":/images/images/Dollar-Bitcoin.JPG");
                    if(suit == 2 && rank == 2) card->setCardImage(":/images/images/Coin-2.JPG");
                    if(suit == 2 && rank == 3) card->setCardImage(":/images/images/Coin-3.JPG");
                    if(suit == 2 && rank == 4) card->setCardImage(":/images/images/Coin-4.JPG");
                    if(suit == 2 && rank == 5) card->setCardImage(":/images/images/Coin-5.JPG");
                    if(suit == 2 && rank == 6) card->setCardImage(":/images/images/Coin-6.JPG");
                    if(suit == 2 && rank == 7) card->setCardImage(":/images/images/Coin-7.JPG");
                    if(suit == 2 && rank == 8) card->setCardImage(":/images/images/Coin-8.JPG");
                    if(suit == 2 && rank == 9) card->setCardImage(":/images/images/Coin-9.JPG");
                    if(suit == 2 && rank == 10) card->setCardImage(":/images/images/Coin-10.JPG");
                    if(suit == 2 && rank == 11) card->setCardImage(":/images/images/Coin-Soldier.JPG");
                    if(suit == 2 && rank == 12) card->setCardImage(":/images/images/Coin-Queen.JPG");
                    if(suit == 2 && rank == 13) card->setCardImage(":/images/images/Coin-King.JPG");
                    if(suit == 2 && rank == 14) card->setCardImage(":/images/images/Coin-Bitcoin.JPG");
                    if(suit == 3 && rank == 2) card->setCardImage(":/images/images/Gold-2.JPG");
                    if(suit == 3 && rank == 3) card->setCardImage(":/images/images/Gold-3.JPG");
                    if(suit == 3 && rank == 4) card->setCardImage(":/images/images/Gold-4.JPG");
                    if(suit == 3 && rank == 5) card->setCardImage(":/images/images/Gold-5.JPG");
                    if(suit == 3 && rank == 6) card->setCardImage(":/images/images/Gold-6.JPG");
                    if(suit == 3 && rank == 7) card->setCardImage(":/images/images/Gold-7.JPG");
                    if(suit == 3 && rank == 8) card->setCardImage(":/images/images/Gold-8.JPG");
                    if(suit == 3 && rank == 9) card->setCardImage(":/images/images/Gold-9.JPG");
                    if(suit == 3 && rank == 10) card->setCardImage(":/images/images/Gold-10.JPG");
                    if(suit == 3 && rank == 11) card->setCardImage(":/images/images/Gold-Soldier.JPG");
                    if(suit == 3 && rank == 12) card->setCardImage(":/images/images/Gold-Queen.JPG");
                    if(suit == 3 && rank == 13) card->setCardImage(":/images/images/Gold-King.JPG");
                    if(suit == 3 && rank == 14) card->setCardImage(":/images/images/Gold-Bitcoin.JPG");
                    qDebug() << "Updated card:" << key << "with rank:" << rank << "and suit:" << suit;
                }
            }
        } else {
            qWarning() << "Your_Turn message does not contain valid cards_in_hand!";
        }

        return;
    }

    if (type == "Round_Start") {
        qDebug() << "Processing Round_Start message";

        QStringList sides = {"B", "T", "R", "L"};
        for (const QString& side : sides) {
            for (int i = 1; i <= 5; ++i) {
                QString key = QString("%1%2").arg(side).arg(i);
                if (cardMap.contains(key)) {
                    CardWidget* card = static_cast<CardWidget*>(cardMap[key]);
                    card->setCardState(CardWidget::Hidden);
                    qDebug() << "Card hidden:" << key;
                }
            }
        }

        return;
    }
    if (type == "Round_End") {
        qDebug() << "Processing Round_End message";

        QString winnerUsername = serverData.value("winner_username").toString(); // استخراج یوزرنیم برنده
        if (winnerUsername.isEmpty()) {
            qWarning() << "Round_End message does not contain a valid username!";
            return;
        }

        QString message;
        if (winnerUsername == username) {
            message = "Congratulations! You won this round."; // پیام برای کاربر فعلی
        } else {
            message = QString("%1 won this round.").arg(winnerUsername); // پیام برای سایر کاربران
        }

        // نمایش پیام در لیبل
        gameMessageLabel->setText(message);
        gameMessageLabel->setStyleSheet("color: yellow; background: #333; padding: 8px; font-size: 16px;");
        gameMessageLabel->show();

        return;
    }
    if (type == "Player_Selection_Update") {
        qDebug() << "Processing Player_Selection_Update message";

        for (int i = 1; i <= 7; ++i) {
            QString key = QString("C%1").arg(i);
            if (cardMap.contains(key)) {
                CardWidget* card = static_cast<CardWidget*>(cardMap[key]);
                card->setCardState(CardWidget::Hidden);
                qDebug() << "Card hidden:" << key;
            }
        }

        QString usernameFromServer = serverData.value("username").toString();
        int sequenceNumber = serverData.value("sequence_number").toInt();
        QJsonObject cardObj = serverData.value("card_selected").toObject();

        if (usernameFromServer.isEmpty() || sequenceNumber < 1 || sequenceNumber > 5 || cardObj.isEmpty()) {
            qWarning() << "Invalid Player_Selection_Update message!";
            return;
        }

        QString side = playerToSide.value(usernameFromServer, "");
        if (side.isEmpty()) {
            qWarning() << "No side found for username:" << usernameFromServer;
            return;
        }

        QString cardKey = QString("%1%2").arg(side).arg(sequenceNumber);
        if (!cardMap.contains(cardKey)) {
            qWarning() << "No card found for key:" << cardKey;
            return;
        }

        CardWidget* card = static_cast<CardWidget*>(cardMap[cardKey]);
        if (usernameFromServer != username) {
            card->setCardState(CardWidget::Normal);
            card->setCardImage(":/images/images/1000027321.jpg");
            qDebug() << "Card set to Hidden for username:" << usernameFromServer;
        } else {
            int suit = cardObj.value("suit").toInt();
            int rank = cardObj.value("rank").toInt();
            card->setCardState(CardWidget::Normal);
            card->setCardText(QString("%1\n%2").arg(suit).arg(rank));
            if(suit == 0 && rank == 2) card->setCardImage(":/images/images/Dimond-2.JPG");
            if(suit == 0 && rank == 3) card->setCardImage(":/images/images/Dimond-3.JPG");
            if(suit == 0 && rank == 4) card->setCardImage(":/images/images/Dimond-4.JPG");
            if(suit == 0 && rank == 5) card->setCardImage(":/images/images/Dimond-5.JPG");
            if(suit == 0 && rank == 6) card->setCardImage(":/images/images/Dimond-6.JPG");
            if(suit == 0 && rank == 7) card->setCardImage(":/images/images/Dimond-7.JPG");
            if(suit == 0 && rank == 8) card->setCardImage(":/images/images/Dimond-8.JPG");
            if(suit == 0 && rank == 9) card->setCardImage(":/images/images/Dimond-9.JPG");
            if(suit == 0 && rank == 10) card->setCardImage(":/images/images/Dimond-10.JPG");
            if(suit == 0 && rank == 11) card->setCardImage(":/images/images/Dimond-Soldier.JPG");
            if(suit == 0 && rank == 12) card->setCardImage(":/images/images/Dimond-Queen.JPG");
            if(suit == 0 && rank == 13) card->setCardImage(":/images/images/Dimond-King.JPG");
            if(suit == 0 && rank == 14) card->setCardImage(":/images/images/Dimond-Bitcoin.JPG");
            if(suit == 1 && rank == 2) card->setCardImage(":/images/images/Dollar-2.JPG");
            if(suit == 1 && rank == 3) card->setCardImage(":/images/images/Dollar-3.JPG");
            if(suit == 1 && rank == 4) card->setCardImage(":/images/images/Dollar-4.JPG");
            if(suit == 1 && rank == 5) card->setCardImage(":/images/images/Dollar-5.JPG");
            if(suit == 1 && rank == 6) card->setCardImage(":/images/images/Dollar-6.JPG");
            if(suit == 1 && rank == 7) card->setCardImage(":/images/images/Dollar-7.JPG");
            if(suit == 1 && rank == 8) card->setCardImage(":/images/images/Dollar-8.JPG");
            if(suit == 1 && rank == 9) card->setCardImage(":/images/images/Dollar-9.JPG");
            if(suit == 1 && rank == 10) card->setCardImage(":/images/images/Dollar-10.JPG");
            if(suit == 1 && rank == 11) card->setCardImage(":/images/images/Dollar-Soldier.JPG");
            if(suit == 1 && rank == 12) card->setCardImage(":/images/images/Dollar-Queen.JPG");
            if(suit == 1 && rank == 13) card->setCardImage(":/images/images/Dollar-King.JPG");
            if(suit == 1 && rank == 14) card->setCardImage(":/images/images/Dollar-Bitcoin.JPG");
            if(suit == 2 && rank == 2) card->setCardImage(":/images/images/Coin-2.JPG");
            if(suit == 2 && rank == 3) card->setCardImage(":/images/images/Coin-3.JPG");
            if(suit == 2 && rank == 4) card->setCardImage(":/images/images/Coin-4.JPG");
            if(suit == 2 && rank == 5) card->setCardImage(":/images/images/Coin-5.JPG");
            if(suit == 2 && rank == 6) card->setCardImage(":/images/images/Coin-6.JPG");
            if(suit == 2 && rank == 7) card->setCardImage(":/images/images/Coin-7.JPG");
            if(suit == 2 && rank == 8) card->setCardImage(":/images/images/Coin-8.JPG");
            if(suit == 2 && rank == 9) card->setCardImage(":/images/images/Coin-9.JPG");
            if(suit == 2 && rank == 10) card->setCardImage(":/images/images/Coin-10.JPG");
            if(suit == 2 && rank == 11) card->setCardImage(":/images/images/Coin-Soldier.JPG");
            if(suit == 2 && rank == 12) card->setCardImage(":/images/images/Coin-Queen.JPG");
            if(suit == 2 && rank == 13) card->setCardImage(":/images/images/Coin-King.JPG");
            if(suit == 2 && rank == 14) card->setCardImage(":/images/images/Coin-Bitcoin.JPG");
            if(suit == 3 && rank == 2) card->setCardImage(":/images/images/Gold-2.JPG");
            if(suit == 3 && rank == 3) card->setCardImage(":/images/images/Gold-3.JPG");
            if(suit == 3 && rank == 4) card->setCardImage(":/images/images/Gold-4.JPG");
            if(suit == 3 && rank == 5) card->setCardImage(":/images/images/Gold-5.JPG");
            if(suit == 3 && rank == 6) card->setCardImage(":/images/images/Gold-6.JPG");
            if(suit == 3 && rank == 7) card->setCardImage(":/images/images/Gold-7.JPG");
            if(suit == 3 && rank == 8) card->setCardImage(":/images/images/Gold-8.JPG");
            if(suit == 3 && rank == 9) card->setCardImage(":/images/images/Gold-9.JPG");
            if(suit == 3 && rank == 10) card->setCardImage(":/images/images/Gold-10.JPG");
            if(suit == 3 && rank == 11) card->setCardImage(":/images/images/Gold-Soldier.JPG");
            if(suit == 3 && rank == 12) card->setCardImage(":/images/images/Gold-Queen.JPG");
            if(suit == 3 && rank == 13) card->setCardImage(":/images/images/Gold-King.JPG");
            if(suit == 3 && rank == 14) card->setCardImage(":/images/images/Gold-Bitcoin.JPG");
            qDebug() << "Updated card:" << cardKey << "with suit:" << suit << "and rank:" << rank;
        }

        return;
    }

    if (type == "Game_End") {
        qDebug() << "Processing Game_End message";

        QString winnerUsername = serverData.value("winner_username").toString(); // استخراج یوزرنیم برنده
        if (winnerUsername.isEmpty()) {
            qWarning() << "Game_End message does not contain a valid username!";
            return;
        }

        QString message = QString("The game has ended! Winner: %1").arg(winnerUsername); // پیام پایان بازی

        // نمایش پیام در QMessageBox
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Game Ended");
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        this->hide();
        UserPanel *panel = new UserPanel(nullptr, socket , username);
        panel->show();
        this->deleteLater();

        return;
    }
    qDebug() << "Unknown message type received!";
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("پیام ناشناخته");
    msgBox.setText("پیام کامل JSON:\n" + QString::fromUtf8(doc.toJson(QJsonDocument::Indented)));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();}
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
                card->setCardText(QString("%1\n%2").arg(suit).arg(rank));
                if(suit == 0 && rank == 2) card->setCardImage(":/images/images/Dimond-2.JPG");
                if(suit == 0 && rank == 3) card->setCardImage(":/images/images/Dimond-3.JPG");
                if(suit == 0 && rank == 4) card->setCardImage(":/images/images/Dimond-4.JPG");
                if(suit == 0 && rank == 5) card->setCardImage(":/images/images/Dimond-5.JPG");
                if(suit == 0 && rank == 6) card->setCardImage(":/images/images/Dimond-6.JPG");
                if(suit == 0 && rank == 7) card->setCardImage(":/images/images/Dimond-7.JPG");
                if(suit == 0 && rank == 8) card->setCardImage(":/images/images/Dimond-8.JPG");
                if(suit == 0 && rank == 9) card->setCardImage(":/images/images/Dimond-9.JPG");
                if(suit == 0 && rank == 10) card->setCardImage(":/images/images/Dimond-10.JPG");
                if(suit == 0 && rank == 11) card->setCardImage(":/images/images/Dimond-Soldier.JPG");
                if(suit == 0 && rank == 12) card->setCardImage(":/images/images/Dimond-Queen.JPG");
                if(suit == 0 && rank == 13) card->setCardImage(":/images/images/Dimond-King.JPG");
                if(suit == 0 && rank == 14) card->setCardImage(":/images/images/Dimond-Bitcoin.JPG");
                if(suit == 1 && rank == 2) card->setCardImage(":/images/images/Dollar-2.JPG");
                if(suit == 1 && rank == 3) card->setCardImage(":/images/images/Dollar-3.JPG");
                if(suit == 1 && rank == 4) card->setCardImage(":/images/images/Dollar-4.JPG");
                if(suit == 1 && rank == 5) card->setCardImage(":/images/images/Dollar-5.JPG");
                if(suit == 1 && rank == 6) card->setCardImage(":/images/images/Dollar-6.JPG");
                if(suit == 1 && rank == 7) card->setCardImage(":/images/images/Dollar-7.JPG");
                if(suit == 1 && rank == 8) card->setCardImage(":/images/images/Dollar-8.JPG");
                if(suit == 1 && rank == 9) card->setCardImage(":/images/images/Dollar-9.JPG");
                if(suit == 1 && rank == 10) card->setCardImage(":/images/images/Dollar-10.JPG");
                if(suit == 1 && rank == 11) card->setCardImage(":/images/images/Dollar-Soldier.JPG");
                if(suit == 1 && rank == 12) card->setCardImage(":/images/images/Dollar-Queen.JPG");
                if(suit == 1 && rank == 13) card->setCardImage(":/images/images/Dollar-King.JPG");
                if(suit == 1 && rank == 14) card->setCardImage(":/images/images/Dollar-Bitcoin.JPG");
                if(suit == 2 && rank == 2) card->setCardImage(":/images/images/Coin-2.JPG");
                if(suit == 2 && rank == 3) card->setCardImage(":/images/images/Coin-3.JPG");
                if(suit == 2 && rank == 4) card->setCardImage(":/images/images/Coin-4.JPG");
                if(suit == 2 && rank == 5) card->setCardImage(":/images/images/Coin-5.JPG");
                if(suit == 2 && rank == 6) card->setCardImage(":/images/images/Coin-6.JPG");
                if(suit == 2 && rank == 7) card->setCardImage(":/images/images/Coin-7.JPG");
                if(suit == 2 && rank == 8) card->setCardImage(":/images/images/Coin-8.JPG");
                if(suit == 2 && rank == 9) card->setCardImage(":/images/images/Coin-9.JPG");
                if(suit == 2 && rank == 10) card->setCardImage(":/images/images/Coin-10.JPG");
                if(suit == 2 && rank == 11) card->setCardImage(":/images/images/Coin-Soldier.JPG");
                if(suit == 2 && rank == 12) card->setCardImage(":/images/images/Coin-Queen.JPG");
                if(suit == 2 && rank == 13) card->setCardImage(":/images/images/Coin-King.JPG");
                if(suit == 2 && rank == 14) card->setCardImage(":/images/images/Coin-Bitcoin.JPG");
                if(suit == 3 && rank == 2) card->setCardImage(":/images/images/Gold-2.JPG");
                if(suit == 3 && rank == 3) card->setCardImage(":/images/images/Gold-3.JPG");
                if(suit == 3 && rank == 4) card->setCardImage(":/images/images/Gold-4.JPG");
                if(suit == 3 && rank == 5) card->setCardImage(":/images/images/Gold-5.JPG");
                if(suit == 3 && rank == 6) card->setCardImage(":/images/images/Gold-6.JPG");
                if(suit == 3 && rank == 7) card->setCardImage(":/images/images/Gold-7.JPG");
                if(suit == 3 && rank == 8) card->setCardImage(":/images/images/Gold-8.JPG");
                if(suit == 3 && rank == 9) card->setCardImage(":/images/images/Gold-9.JPG");
                if(suit == 3 && rank == 10) card->setCardImage(":/images/images/Gold-10.JPG");
                if(suit == 3 && rank == 11) card->setCardImage(":/images/images/Gold-Soldier.JPG");
                if(suit == 3 && rank == 12) card->setCardImage(":/images/images/Gold-Queen.JPG");
                if(suit == 3 && rank == 13) card->setCardImage(":/images/images/Gold-King.JPG");
                if(suit == 3 && rank == 14) card->setCardImage(":/images/images/Gold-Bitcoin.JPG");
                qDebug() << "Updated card:" << key << "with rank:" << rank << "and suit:" << suit;
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

// void GameBoard::processInitialHandMessage(const QJsonObject& obj) {
//     qDebug() << "processInitialHandMessage called!";
//     gameMessageLabel->setText("دست اولیه شما دریافت شد.");
//     gameMessageLabel->setStyleSheet("color: orange; background: #333; padding: 8px;");
//     gameMessageLabel->show();
//     // QMessageBox::information(this, "دست اولیه", "دست اولیه شما دریافت شد.");
// }
void GameBoard::processInitialHandMessage(const QJsonObject& obj) {
    qDebug() << "processInitialHandMessage called!";

    if (obj.contains("cards") && obj["cards"].isArray()) {
        QTimer::singleShot(5000, this, [=]() {
            QJsonArray arr = obj["cards"].toArray();
            for (int i = 0; i < arr.size() && i < 7; ++i) {
                QJsonObject cardObj = arr[i].toObject();
                int rank = cardObj["rank"].toInt();
                int suit = cardObj["suit"].toInt();

                QString key = QString("C%1").arg(i + 1);
                if (cardMap.contains(key)) {
                    CardWidget* card = static_cast<CardWidget*>(cardMap[key]);
                    card->setCardState(CardWidget::Normal);
                    card->setCardText(QString("%1\n%2").arg(suit).arg(rank));
                    if(suit == 0 && rank == 2) card->setCardImage(":/images/images/Dimond-2.JPG");
                    if(suit == 0 && rank == 3) card->setCardImage(":/images/images/Dimond-3.JPG");
                    if(suit == 0 && rank == 4) card->setCardImage(":/images/images/Dimond-4.JPG");
                    if(suit == 0 && rank == 5) card->setCardImage(":/images/images/Dimond-5.JPG");
                    if(suit == 0 && rank == 6) card->setCardImage(":/images/images/Dimond-6.JPG");
                    if(suit == 0 && rank == 7) card->setCardImage(":/images/images/Dimond-7.JPG");
                    if(suit == 0 && rank == 8) card->setCardImage(":/images/images/Dimond-8.JPG");
                    if(suit == 0 && rank == 9) card->setCardImage(":/images/images/Dimond-9.JPG");
                    if(suit == 0 && rank == 10) card->setCardImage(":/images/images/Dimond-10.JPG");
                    if(suit == 0 && rank == 11) card->setCardImage(":/images/images/Dimond-Soldier.JPG");
                    if(suit == 0 && rank == 12) card->setCardImage(":/images/images/Dimond-Queen.JPG");
                    if(suit == 0 && rank == 13) card->setCardImage(":/images/images/Dimond-King.JPG");
                    if(suit == 0 && rank == 14) card->setCardImage(":/images/images/Dimond-Bitcoin.JPG");
                    if(suit == 1 && rank == 2) card->setCardImage(":/images/images/Dollar-2.JPG");
                    if(suit == 1 && rank == 3) card->setCardImage(":/images/images/Dollar-3.JPG");
                    if(suit == 1 && rank == 4) card->setCardImage(":/images/images/Dollar-4.JPG");
                    if(suit == 1 && rank == 5) card->setCardImage(":/images/images/Dollar-5.JPG");
                    if(suit == 1 && rank == 6) card->setCardImage(":/images/images/Dollar-6.JPG");
                    if(suit == 1 && rank == 7) card->setCardImage(":/images/images/Dollar-7.JPG");
                    if(suit == 1 && rank == 8) card->setCardImage(":/images/images/Dollar-8.JPG");
                    if(suit == 1 && rank == 9) card->setCardImage(":/images/images/Dollar-9.JPG");
                    if(suit == 1 && rank == 10) card->setCardImage(":/images/images/Dollar-10.JPG");
                    if(suit == 1 && rank == 11) card->setCardImage(":/images/images/Dollar-Soldier.JPG");
                    if(suit == 1 && rank == 12) card->setCardImage(":/images/images/Dollar-Queen.JPG");
                    if(suit == 1 && rank == 13) card->setCardImage(":/images/images/Dollar-King.JPG");
                    if(suit == 1 && rank == 14) card->setCardImage(":/images/images/Dollar-Bitcoin.JPG");
                    if(suit == 2 && rank == 2) card->setCardImage(":/images/images/Coin-2.JPG");
                    if(suit == 2 && rank == 3) card->setCardImage(":/images/images/Coin-3.JPG");
                    if(suit == 2 && rank == 4) card->setCardImage(":/images/images/Coin-4.JPG");
                    if(suit == 2 && rank == 5) card->setCardImage(":/images/images/Coin-5.JPG");
                    if(suit == 2 && rank == 6) card->setCardImage(":/images/images/Coin-6.JPG");
                    if(suit == 2 && rank == 7) card->setCardImage(":/images/images/Coin-7.JPG");
                    if(suit == 2 && rank == 8) card->setCardImage(":/images/images/Coin-8.JPG");
                    if(suit == 2 && rank == 9) card->setCardImage(":/images/images/Coin-9.JPG");
                    if(suit == 2 && rank == 10) card->setCardImage(":/images/images/Coin-10.JPG");
                    if(suit == 2 && rank == 11) card->setCardImage(":/images/images/Coin-Soldier.JPG");
                    if(suit == 2 && rank == 12) card->setCardImage(":/images/images/Coin-Queen.JPG");
                    if(suit == 2 && rank == 13) card->setCardImage(":/images/images/Coin-King.JPG");
                    if(suit == 2 && rank == 14) card->setCardImage(":/images/images/Coin-Bitcoin.JPG");
                    if(suit == 3 && rank == 2) card->setCardImage(":/images/images/Gold-2.JPG");
                    if(suit == 3 && rank == 3) card->setCardImage(":/images/images/Gold-3.JPG");
                    if(suit == 3 && rank == 4) card->setCardImage(":/images/images/Gold-4.JPG");
                    if(suit == 3 && rank == 5) card->setCardImage(":/images/images/Gold-5.JPG");
                    if(suit == 3 && rank == 6) card->setCardImage(":/images/images/Gold-6.JPG");
                    if(suit == 3 && rank == 7) card->setCardImage(":/images/images/Gold-7.JPG");
                    if(suit == 3 && rank == 8) card->setCardImage(":/images/images/Gold-8.JPG");
                    if(suit == 3 && rank == 9) card->setCardImage(":/images/images/Gold-9.JPG");
                    if(suit == 3 && rank == 10) card->setCardImage(":/images/images/Gold-10.JPG");
                    if(suit == 3 && rank == 11) card->setCardImage(":/images/images/Gold-Soldier.JPG");
                    if(suit == 3 && rank == 12) card->setCardImage(":/images/images/Gold-Queen.JPG");
                    if(suit == 3 && rank == 13) card->setCardImage(":/images/images/Gold-King.JPG");
                    if(suit == 3 && rank == 14) card->setCardImage(":/images/images/Gold-Bitcoin.JPG");
                    qDebug() << "Updated card:" << key << "with rank:" << rank << "and suit:" << suit;
                }
            }
        });
    } else {
        qDebug() << "Initial_Hand message does not contain valid cards!";
    }
}
void GameBoard::processYourTurnMessage(const QJsonObject& obj) {
    qDebug() << "processYourTurnMessage called!";

    if (obj.contains("cards_in_hand") && obj["cards_in_hand"].isArray()) {
        QJsonArray cardsArray = obj["cards_in_hand"].toArray();

        int cardCount = cardsArray.size();
        if (cardCount >= 4 && cardCount <= 7) {
            for (int i = 0; i < cardCount; ++i) {
                QJsonObject cardObj = cardsArray[i].toObject();
                int rank = cardObj["rank"].toInt();
                int suit = cardObj["suit"].toInt();

                QString key = QString("C%1").arg(i + 1);
                if (cardMap.contains(key)) {
                    CardWidget* card = static_cast<CardWidget*>(cardMap[key]);
                    card->setCardState(CardWidget::Normal);
                    card->setCardText(QString("%1\n%2").arg(suit).arg(rank));
                    if(suit == 0 && rank == 2) card->setCardImage(":/images/images/Dimond-2.JPG");
                    if(suit == 0 && rank == 3) card->setCardImage(":/images/images/Dimond-3.JPG");
                    if(suit == 0 && rank == 4) card->setCardImage(":/images/images/Dimond-4.JPG");
                    if(suit == 0 && rank == 5) card->setCardImage(":/images/images/Dimond-5.JPG");
                    if(suit == 0 && rank == 6) card->setCardImage(":/images/images/Dimond-6.JPG");
                    if(suit == 0 && rank == 7) card->setCardImage(":/images/images/Dimond-7.JPG");
                    if(suit == 0 && rank == 8) card->setCardImage(":/images/images/Dimond-8.JPG");
                    if(suit == 0 && rank == 9) card->setCardImage(":/images/images/Dimond-9.JPG");
                    if(suit == 0 && rank == 10) card->setCardImage(":/images/images/Dimond-10.JPG");
                    if(suit == 0 && rank == 11) card->setCardImage(":/images/images/Dimond-Soldier.JPG");
                    if(suit == 0 && rank == 12) card->setCardImage(":/images/images/Dimond-Queen.JPG");
                    if(suit == 0 && rank == 13) card->setCardImage(":/images/images/Dimond-King.JPG");
                    if(suit == 0 && rank == 14) card->setCardImage(":/images/images/Dimond-Bitcoin.JPG");
                    if(suit == 1 && rank == 2) card->setCardImage(":/images/images/Dollar-2.JPG");
                    if(suit == 1 && rank == 3) card->setCardImage(":/images/images/Dollar-3.JPG");
                    if(suit == 1 && rank == 4) card->setCardImage(":/images/images/Dollar-4.JPG");
                    if(suit == 1 && rank == 5) card->setCardImage(":/images/images/Dollar-5.JPG");
                    if(suit == 1 && rank == 6) card->setCardImage(":/images/images/Dollar-6.JPG");
                    if(suit == 1 && rank == 7) card->setCardImage(":/images/images/Dollar-7.JPG");
                    if(suit == 1 && rank == 8) card->setCardImage(":/images/images/Dollar-8.JPG");
                    if(suit == 1 && rank == 9) card->setCardImage(":/images/images/Dollar-9.JPG");
                    if(suit == 1 && rank == 10) card->setCardImage(":/images/images/Dollar-10.JPG");
                    if(suit == 1 && rank == 11) card->setCardImage(":/images/images/Dollar-Soldier.JPG");
                    if(suit == 1 && rank == 12) card->setCardImage(":/images/images/Dollar-Queen.JPG");
                    if(suit == 1 && rank == 13) card->setCardImage(":/images/images/Dollar-King.JPG");
                    if(suit == 1 && rank == 14) card->setCardImage(":/images/images/Dollar-Bitcoin.JPG");
                    if(suit == 2 && rank == 2) card->setCardImage(":/images/images/Coin-2.JPG");
                    if(suit == 2 && rank == 3) card->setCardImage(":/images/images/Coin-3.JPG");
                    if(suit == 2 && rank == 4) card->setCardImage(":/images/images/Coin-4.JPG");
                    if(suit == 2 && rank == 5) card->setCardImage(":/images/images/Coin-5.JPG");
                    if(suit == 2 && rank == 6) card->setCardImage(":/images/images/Coin-6.JPG");
                    if(suit == 2 && rank == 7) card->setCardImage(":/images/images/Coin-7.JPG");
                    if(suit == 2 && rank == 8) card->setCardImage(":/images/images/Coin-8.JPG");
                    if(suit == 2 && rank == 9) card->setCardImage(":/images/images/Coin-9.JPG");
                    if(suit == 2 && rank == 10) card->setCardImage(":/images/images/Coin-10.JPG");
                    if(suit == 2 && rank == 11) card->setCardImage(":/images/images/Coin-Soldier.JPG");
                    if(suit == 2 && rank == 12) card->setCardImage(":/images/images/Coin-Queen.JPG");
                    if(suit == 2 && rank == 13) card->setCardImage(":/images/images/Coin-King.JPG");
                    if(suit == 2 && rank == 14) card->setCardImage(":/images/images/Coin-Bitcoin.JPG");
                    if(suit == 3 && rank == 2) card->setCardImage(":/images/images/Gold-2.JPG");
                    if(suit == 3 && rank == 3) card->setCardImage(":/images/images/Gold-3.JPG");
                    if(suit == 3 && rank == 4) card->setCardImage(":/images/images/Gold-4.JPG");
                    if(suit == 3 && rank == 5) card->setCardImage(":/images/images/Gold-5.JPG");
                    if(suit == 3 && rank == 6) card->setCardImage(":/images/images/Gold-6.JPG");
                    if(suit == 3 && rank == 7) card->setCardImage(":/images/images/Gold-7.JPG");
                    if(suit == 3 && rank == 8) card->setCardImage(":/images/images/Gold-8.JPG");
                    if(suit == 3 && rank == 9) card->setCardImage(":/images/images/Gold-9.JPG");
                    if(suit == 3 && rank == 10) card->setCardImage(":/images/images/Gold-10.JPG");
                    if(suit == 3 && rank == 11) card->setCardImage(":/images/images/Gold-Soldier.JPG");
                    if(suit == 3 && rank == 12) card->setCardImage(":/images/images/Gold-Queen.JPG");
                    if(suit == 3 && rank == 13) card->setCardImage(":/images/images/Gold-King.JPG");
                    if(suit == 3 && rank == 14) card->setCardImage(":/images/images/Gold-Bitcoin.JPG");
                    qDebug() << "Updated card:" << key << "with rank:" << rank << "and suit:" << suit;

                    connect(card, &CardWidget::cardClicked, this, &GameBoard::handleCardClick);
                }
            }
        } else {
            qDebug() << "Your_Turn message contains invalid number of cards!";
        }
    } else {
        qDebug() << "Your_Turn message does not contain valid cards_in_hand!";
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("نوبت شما");
    msgBox.setText("پیام کامل JSON:\n" + QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Indented)));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}








