/********************************************************************************
** Form generated from reading UI file 'graphiccard.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHICCARD_H
#define UI_GRAPHICCARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GraphicCard
{
public:

    void setupUi(QWidget *GraphicCard)
    {
        if (GraphicCard->objectName().isEmpty())
            GraphicCard->setObjectName("GraphicCard");
        GraphicCard->resize(400, 300);

        retranslateUi(GraphicCard);

        QMetaObject::connectSlotsByName(GraphicCard);
    } // setupUi

    void retranslateUi(QWidget *GraphicCard)
    {
        GraphicCard->setWindowTitle(QCoreApplication::translate("GraphicCard", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GraphicCard: public Ui_GraphicCard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHICCARD_H
