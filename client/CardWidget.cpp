#include "CardWidget.h"
#include <QMouseEvent>

CardWidget::CardWidget(const QString &text, QWidget *parent)
    : QWidget(parent), state(Hidden)
{
    setFixedSize(60, 90);

    // Label for text
    label = new QLabel(text, this);
    originalText = text;
    label->setAlignment(Qt::AlignCenter);
    label->setFixedSize(60, 90);

    // Label for image
    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setFixedSize(60, 90);
    imageLabel->setStyleSheet("background-color: transparent;");
    imageLabel->hide(); // Default to hidden

    // Ensure imageLabel does not block mouse events
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
        imageLabel->show(); // Show image
        label->hide();      // Hide text
        setStyleSheet("background-color: transparent;");
        break;
    case Red:
        imageLabel->show(); // Show image
        label->hide();      // Hide text
        setStyleSheet(R"(
            background-color: transparent;
            border: 2px solid red;
            border-radius: 12px;
        )");
        break;
    case Hidden:
        imageLabel->hide(); // Hide image
        label->hide();      // Hide text
        setStyleSheet("background-color: transparent;");
        break;
    }
}

void CardWidget::mousePressEvent(QMouseEvent *event)
{
    if (state == Normal && event->button() == Qt::LeftButton) {
        emit cardClicked(originalText); // Emit signal with the card's text
    }

    QWidget::mousePressEvent(event);
}


void CardWidget::setCardImage(const QString& imagePath)
{
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        imageLabel->setPixmap(pixmap.scaled(60, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->show(); // Show image
        label->hide();      // Hide text
    } else {
        qWarning() << "Failed to load image:" << imagePath; // Log failure
    }
}

