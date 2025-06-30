/********************************************************************************
** Form generated from reading UI file 'waitingroom.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WAITINGROOM_H
#define UI_WAITINGROOM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WaitingRoom
{
public:

    void setupUi(QWidget *WaitingRoom)
    {
        if (WaitingRoom->objectName().isEmpty())
            WaitingRoom->setObjectName("WaitingRoom");
        WaitingRoom->resize(400, 300);

        retranslateUi(WaitingRoom);

        QMetaObject::connectSlotsByName(WaitingRoom);
    } // setupUi

    void retranslateUi(QWidget *WaitingRoom)
    {
        WaitingRoom->setWindowTitle(QCoreApplication::translate("WaitingRoom", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WaitingRoom: public Ui_WaitingRoom {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WAITINGROOM_H
