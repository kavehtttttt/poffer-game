/********************************************************************************
** Form generated from reading UI file 'mainmenu.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINMENU_H
#define UI_MAINMENU_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Mainmenu
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Mainmenu)
    {
        if (Mainmenu->objectName().isEmpty())
            Mainmenu->setObjectName("Mainmenu");
        Mainmenu->resize(800, 600);
        centralwidget = new QWidget(Mainmenu);
        centralwidget->setObjectName("centralwidget");
        Mainmenu->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Mainmenu);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        Mainmenu->setMenuBar(menubar);
        statusbar = new QStatusBar(Mainmenu);
        statusbar->setObjectName("statusbar");
        Mainmenu->setStatusBar(statusbar);

        retranslateUi(Mainmenu);

        QMetaObject::connectSlotsByName(Mainmenu);
    } // setupUi

    void retranslateUi(QMainWindow *Mainmenu)
    {
        Mainmenu->setWindowTitle(QCoreApplication::translate("Mainmenu", "Mainmenu", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Mainmenu: public Ui_Mainmenu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINMENU_H
