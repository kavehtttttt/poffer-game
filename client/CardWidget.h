#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include <QWidget>
#include <QLabel>

class CardWidget : public QWidget {
    Q_OBJECT

public:
    enum CardState { Hidden, Normal, Red };

    explicit CardWidget(const QString &text, QWidget *parent = nullptr);

    void setCardState(int newState);
    void setCardText(const QString& newText);

signals:
    void cardClicked(const QString& cardText); // Signal emitted when the card is clicked

protected:
    void mousePressEvent(QMouseEvent *event) override; // Handle mouse clicks

private:
    void updateStyle();
    QLabel *label;
    QString originalText;
    CardState state;
};

#endif // CARDWIDGET_H
