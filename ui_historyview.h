/********************************************************************************
** Form generated from reading UI file 'historyview.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HISTORYVIEW_H
#define UI_HISTORYVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_HistoryView
{
public:

    void setupUi(QWidget *HistoryView)
    {
        if (HistoryView->objectName().isEmpty())
            HistoryView->setObjectName("HistoryView");
        HistoryView->resize(400, 300);

        retranslateUi(HistoryView);

        QMetaObject::connectSlotsByName(HistoryView);
    } // setupUi

    void retranslateUi(QWidget *HistoryView)
    {
        HistoryView->setWindowTitle(QCoreApplication::translate("HistoryView", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class HistoryView: public Ui_HistoryView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HISTORYVIEW_H
