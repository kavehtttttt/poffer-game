#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include <QWidget>
#include <QLabel>

class CardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardWidget(const QString &text, QWidget *parent = nullptr);
    void setCardState(int newState);
    void setCardText(const QString& newText);

private:
    enum CardState { Normal, Red, Hidden };
    CardState state;
    QLabel *label;
    QString originalText;

    void updateStyle();
};

#endif // CARDWIDGET_H
