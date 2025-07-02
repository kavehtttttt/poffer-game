/********************************************************************************
** Form generated from reading UI file 'cardrevealmanager.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CARDREVEALMANAGER_H
#define UI_CARDREVEALMANAGER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CardRevealManager
{
public:

    void setupUi(QWidget *CardRevealManager)
    {
        if (CardRevealManager->objectName().isEmpty())
            CardRevealManager->setObjectName("CardRevealManager");
        CardRevealManager->resize(400, 300);

        retranslateUi(CardRevealManager);

        QMetaObject::connectSlotsByName(CardRevealManager);
    } // setupUi

    void retranslateUi(QWidget *CardRevealManager)
    {
        CardRevealManager->setWindowTitle(QCoreApplication::translate("CardRevealManager", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CardRevealManager: public Ui_CardRevealManager {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CARDREVEALMANAGER_H
