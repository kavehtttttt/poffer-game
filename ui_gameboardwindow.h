/********************************************************************************
** Form generated from reading UI file 'gameboardwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GAMEBOARDWINDOW_H
#define UI_GAMEBOARDWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GameBoardWindow
{
public:

    void setupUi(QWidget *GameBoardWindow)
    {
        if (GameBoardWindow->objectName().isEmpty())
            GameBoardWindow->setObjectName("GameBoardWindow");
        GameBoardWindow->resize(400, 300);

        retranslateUi(GameBoardWindow);

        QMetaObject::connectSlotsByName(GameBoardWindow);
    } // setupUi

    void retranslateUi(QWidget *GameBoardWindow)
    {
        GameBoardWindow->setWindowTitle(QCoreApplication::translate("GameBoardWindow", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GameBoardWindow: public Ui_GameBoardWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GAMEBOARDWINDOW_H
