#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "qcombobox.h"
#include "qjsonarray.h"
#include "qpushbutton.h"
#include <QWidget>
#include <QTcpSocket>
#include <QStringList>
#include <QMap>
#include <QVector>
#include <QJsonObject>

class QGraphicsScene;
class QGraphicsView;
class QLabel;
class CardWidget;

struct PlayerCardInfo {
    QString username;
    QString side;
    int rank;
    int suit;
};

class GameBoard : public QWidget
{
    Q_OBJECT
public:
    explicit GameBoard(QWidget *parent, QTcpSocket* socket, QString& username, QStringList* otherPlayers, const QString& initialBuffer = "");

private slots:
    void handleCardClick(const QString& cardText);
    void onExitClicked();
    void onStopClicked();


private:
    void setupScene();
    void addCard(int x, int y, const QString &text, int rotation = 0);
    void setupTopCards();
    void setupBottomCards();
    void setupLeftCards();
    void setupRightCards();
    void setupCenterCards();
    void updateConnectionStatus();
    void handleServerMessage(const QString& message);
    void processInitialData(const QString& data);
    void revealThirdCardsSequentially(const QVector<PlayerCardInfo>& cardInfos);
    void processBufferedCardMessages();
    void processInitialHandMessage(const QJsonObject& obj);
    void processYourTurnMessage(const QJsonObject& obj);
    bool thirdCardsDisplayed = false;
    QTcpSocket *socket;
    QStringList *otherPlayers;
    QString username;
    QString buffer;
    bool isPaused;
    QMap<QString, QString> playerToSide;
    QMap<QString, QString> sideToPlayer;
    QComboBox* playerComboBox;
    QComboBox* numberComboBox;
    QPushButton* confirmButton;

    QLabel *connectionStatusLabel;
    QLabel *gameMessageLabel;
    QGraphicsScene *scene;
    QGraphicsView *view;
    QMap<QString, QWidget*> cardMap;
    QPushButton *stopButton;
    QPushButton *exitButton;
    bool readyForCardMessages = false;
    QVector<QJsonObject> bufferedInitialHandMessages;
    QVector<QJsonObject> bufferedYourTurnMessages;
};

#endif // GAMEBOARD_H
