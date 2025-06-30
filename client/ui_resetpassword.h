/********************************************************************************
** Form generated from reading UI file 'resetpassword.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESETPASSWORD_H
#define UI_RESETPASSWORD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_resetpassword
{
public:

    void setupUi(QWidget *resetpassword)
    {
        if (resetpassword->objectName().isEmpty())
            resetpassword->setObjectName("resetpassword");
        resetpassword->resize(400, 300);

        retranslateUi(resetpassword);

        QMetaObject::connectSlotsByName(resetpassword);
    } // setupUi

    void retranslateUi(QWidget *resetpassword)
    {
        resetpassword->setWindowTitle(QCoreApplication::translate("resetpassword", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class resetpassword: public Ui_resetpassword {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESETPASSWORD_H
