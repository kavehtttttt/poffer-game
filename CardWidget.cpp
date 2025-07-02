#include "CardWidget.h"
#include <QMouseEvent>

CardWidget::CardWidget(const QString &text, QWidget *parent)
    : QWidget(parent), state(Hidden)
{
    setFixedSize(60, 90);

    label = new QLabel(text, this);
    originalText = text;
    label->setAlignment(Qt::AlignCenter);
    label->setFixedSize(60, 90);

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setFixedSize(60, 90);
    imageLabel->setStyleSheet("background-color: transparent;");
    imageLabel->hide();

    imageLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

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
        imageLabel->show();
        label->hide();
        setStyleSheet("background-color: transparent;");
        break;
    case Red:
        imageLabel->show();
        label->hide();
        setStyleSheet(R"(
            background-color: transparent;
            border: 2px solid red;
            border-radius: 12px;
        )");
        break;
    case Hidden:
        imageLabel->hide();
        label->hide();
        setStyleSheet("background-color: transparent;");
        break;
    }
}

void CardWidget::mousePressEvent(QMouseEvent *event)
{
    if (state == Normal && event->button() == Qt::LeftButton) {
        emit cardClicked(originalText);
    }

    QWidget::mousePressEvent(event);
}


void CardWidget::setCardImage(const QString& imagePath)
{
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        imageLabel->setPixmap(pixmap.scaled(60, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->show();
        label->hide();
    } else {
        qWarning() << "Failed to load image:" << imagePath;
    }
}

