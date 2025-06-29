#include "GameBoard.h"
#include "CardWidget.h"

#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QLabel>

GameBoard::GameBoard(QWidget *parent, QTcpSocket* socket, QStringList* otherPlayers)
    : QWidget(parent), socket(socket), otherPlayers(otherPlayers)
{
    setFixedSize(900, 700);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // نگاشت بازیکنان به جهت‌ها
    QStringList sides = {"B", "R", "T", "L"};
    for (int i = 0; i < otherPlayers->size(); ++i) {
        playerToSide[otherPlayers->at(i)] = sides[i][0];
        sideToPlayer[sides[i][0]] = otherPlayers->at(i);
    }

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(5);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setFont(QFont("Georgia", 12, QFont::Bold));
    layout->addWidget(connectionStatusLabel);

    setupScene();
    layout->addWidget(view);

    setupTopCards();
    setupBottomCards();
    setupLeftCards();
    setupRightCards();
    setupCenterCards();

    // فرم کنترل کارت‌ها
    QHBoxLayout *controlLayout = new QHBoxLayout;

    QLineEdit *cardIdEdit = new QLineEdit;
    cardIdEdit->setPlaceholderText("مثلاً B3 یا رضا1");

    QLineEdit *textEdit = new QLineEdit;
    textEdit->setPlaceholderText("متن کارت");

    QComboBox *stateBox = new QComboBox;
    stateBox->addItems({"Normal", "Red", "Hidden"});

    QPushButton *applyButton = new QPushButton("Apply");

    controlLayout->addWidget(new QLabel("Card ID:"));
    controlLayout->addWidget(cardIdEdit);
    controlLayout->addWidget(new QLabel("Text:"));
    controlLayout->addWidget(textEdit);
    controlLayout->addWidget(new QLabel("State:"));
    controlLayout->addWidget(stateBox);
    controlLayout->addWidget(applyButton);

    layout->addLayout(controlLayout);

    connect(applyButton, &QPushButton::clicked, this, [=]() {
        QString id = cardIdEdit->text().trimmed();
        QString newText = textEdit->text().trimmed();
        int stateIndex = stateBox->currentIndex();

        if (cardMap.contains(id)) {
            CardWidget* card = cardMap[id];
            card->setCardState(stateIndex);
            if (stateIndex != 2)
                card->setCardText(newText);
        } else {
            QMessageBox::warning(this, "Not Found", "Card with ID " + id + " not found.");
        }
    });

    updateConnectionStatus();
    showPlayersMessage();
}

void GameBoard::setupScene()
{
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
    auto *card = new CardWidget(text);
    cardMap[text] = card;

    if (text.length() >= 2 && sideToPlayer.contains(text[0])) {
        QString playerName = sideToPlayer[text[0]];
        QString altKey = playerName + text.mid(1);
        cardMap[altKey] = card;
    }

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
    int w = 60, gap = 20;
    int total = 5 * w + 4 * gap;
    int sx = width()/2 - total/2;
    int y = 20;

    for (int i = 0; i < 5; ++i)
        addCard(sx + i * (w + gap), y, QString("T%1").arg(i+1));
}

void GameBoard::setupBottomCards()
{
    int w = 60, h = 90, gap = 20;
    int total = 5 * w + 4 * gap;
    int sx = width()/2 - total/2;
    int y = height() - h - 20;

    for (int i = 0; i < 5; ++i)
        addCard(sx + i * (w + gap), y, QString("B%1").arg(i+1));
}

void GameBoard::setupLeftCards()
{
    int w = 60, gap = 20;
    int total = 5 * w + 4 * gap;
    int sy = height()/2 - total/2;
    int x = 30;

    for (int i = 0; i < 5; ++i)
        addCard(x, sy + i * (w + gap), QString("L%1").arg(i+1), 90);
}

void GameBoard::setupRightCards()
{
    int w = 60, h = 90, gap = 20;
    int total = 5 * w + 4 * gap;
    int sy = height()/2 - total/2;
    int x = width() - 30 - h;

    for (int i = 0; i < 5; ++i)
        addCard(x, sy + i * (w + gap), QString("R%1").arg(i+1), -90);
}

void GameBoard::setupCenterCards()
{
    int w = 60, h = 90, gap = 15;
    int total = 7 * w + 6 * gap;
    int sx = width()/2 - total/2;
    int y = height()/2 - h/2;

    for (int i = 0; i < 7; ++i)
        addCard(sx + i * (w + gap), y, QString("C%1").arg(i+1));
}

void GameBoard::updateConnectionStatus()
{
    if (!socket) {
        connectionStatusLabel->setText("Socket is null!");
        connectionStatusLabel->setStyleSheet("color: red;");
    } else if (socket->state() == QAbstractSocket::ConnectedState) {
        connectionStatusLabel->setText("Connected to server");
        connectionStatusLabel->setStyleSheet("color: lightgreen;");
    } else {
        connectionStatusLabel->setText("Connecting...");
        connectionStatusLabel->setStyleSheet("color: gray;");
    }
}

void GameBoard::showPlayersMessage()
{
    QString message = "Players in the game:\n";
    for (const QString &p : *otherPlayers)
        message += "Player: " + p + "\n";

    QMessageBox::information(this, "Game Players", message);
}
