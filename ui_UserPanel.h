/********************************************************************************
** Form generated from reading UI file 'UserPanel.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERPANEL_H
#define UI_USERPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UserPanel
{
public:

    void setupUi(QWidget *UserPanel)
    {
        if (UserPanel->objectName().isEmpty())
            UserPanel->setObjectName("UserPanel");
        UserPanel->resize(400, 300);

        retranslateUi(UserPanel);

        QMetaObject::connectSlotsByName(UserPanel);
    } // setupUi

    void retranslateUi(QWidget *UserPanel)
    {
        UserPanel->setWindowTitle(QCoreApplication::translate("UserPanel", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UserPanel: public Ui_UserPanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERPANEL_H
