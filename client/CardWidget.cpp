#include "CardWidget.h"
#include <QVBoxLayout>

CardWidget::CardWidget(const QString &text, QWidget *parent)
    : QWidget(parent), state(Hidden)
{
    setFixedSize(60, 90);
    label = new QLabel(text, this);
    originalText = text;
    label->setAlignment(Qt::AlignCenter);
    label->setFixedSize(60, 90);
    updateStyle();
}

void CardWidget::setCardState(int newState)
{
    if (newState >= 0 && newState <= 2) {
        state = static_cast<CardState>(newState);
        updateStyle();
    }
}

void CardWidget::setCardText(const QString& newText)
{
    originalText = newText;
    if (state != Hidden) {
        label->setText(newText);
        label->show();
    }
}

void CardWidget::updateStyle()
{
    switch (state) {
    case Normal:
        label->setText(originalText);
        label->show();
        setStyleSheet("background-color: transparent;");
        label->setStyleSheet(R"(
            background-color: white;
            border: 2px solid black;
            border-radius: 12px;
            font-weight: bold;
            font-size: 14px;
            color: #333;
        )");
        break;
    case Red:
        label->setText(originalText);
        label->show();
        setStyleSheet("background-color: transparent;");
        label->setStyleSheet(R"(
            background-color: red;
            border: 2px solid darkred;
            border-radius: 12px;
            font-weight: bold;
            font-size: 14px;
            color: white;
        )");
        break;
    case Hidden:
        label->hide();
        setStyleSheet("background-color: transparent;");
        break;
    }
}
