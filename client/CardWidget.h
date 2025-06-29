#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include <QWidget>
#include <QLabel>

class CardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardWidget(const QString &text, QWidget *parent = nullptr);

    enum CardState { Normal = 0, Red = 1, Hidden = 2 };
    void setCardState(int newState);
    void setCardText(const QString& newText);

private:
    void updateStyle();
    QLabel *label;
    QString originalText;
    CardState state;
};

#endif // CARDWIDGET_H
