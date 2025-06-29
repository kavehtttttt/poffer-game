#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QTcpSocket>
#include <QMap>

class CardWidget;

class GameBoard : public QWidget
{
    Q_OBJECT
public:
    explicit GameBoard(QWidget *parent = nullptr,
                       QTcpSocket* socket = nullptr,
                       QStringList* otherPlayers = nullptr);

private:
    QGraphicsScene *scene;
    QGraphicsView *view;
    QLabel *connectionStatusLabel;
    QTcpSocket* socket;
    QStringList* otherPlayers;
    QMap<QString, CardWidget*> cardMap;
    QMap<QString, QChar> playerToSide;
    QMap<QChar, QString> sideToPlayer;

    void setupScene();
    void setupTopCards();
    void setupBottomCards();
    void setupLeftCards();
    void setupRightCards();
    void setupCenterCards();
    void addCard(int x, int y, const QString &text, int rotation = 0);
    void updateConnectionStatus();
    void showPlayersMessage();
};

#endif // GAMEBOARD_H
