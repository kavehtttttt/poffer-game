/********************************************************************************
** Form generated from reading UI file 'cardwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CARDWIDGET_H
#define UI_CARDWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CardWidget
{
public:

    void setupUi(QWidget *CardWidget)
    {
        if (CardWidget->objectName().isEmpty())
            CardWidget->setObjectName("CardWidget");
        CardWidget->resize(400, 300);

        retranslateUi(CardWidget);

        QMetaObject::connectSlotsByName(CardWidget);
    } // setupUi

    void retranslateUi(QWidget *CardWidget)
    {
        CardWidget->setWindowTitle(QCoreApplication::translate("CardWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CardWidget: public Ui_CardWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CARDWIDGET_H
