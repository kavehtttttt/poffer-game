/********************************************************************************
** Form generated from reading UI file 'editinfo.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITINFO_H
#define UI_EDITINFO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EditInfo
{
public:

    void setupUi(QWidget *EditInfo)
    {
        if (EditInfo->objectName().isEmpty())
            EditInfo->setObjectName("EditInfo");
        EditInfo->resize(400, 300);

        retranslateUi(EditInfo);

        QMetaObject::connectSlotsByName(EditInfo);
    } // setupUi

    void retranslateUi(QWidget *EditInfo)
    {
        EditInfo->setWindowTitle(QCoreApplication::translate("EditInfo", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class EditInfo: public Ui_EditInfo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITINFO_H
