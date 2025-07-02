#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include <QWidget>
#include <QLabel>

class CardWidget : public QWidget {
    Q_OBJECT

public:
    enum CardState { Hidden, Normal, Red };

    explicit CardWidget(const QString &text = "", QWidget *parent = nullptr);

    void setCardState(int newState);
    void setCardText(const QString& newText);
    void setCardImage(const QString& imagePath);

signals:
    void cardClicked(const QString& cardText);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    void updateStyle();
    QLabel *label;
    QLabel *imageLabel;
    QString originalText;
    CardState state;
};

#endif // CARDWIDGET_H
