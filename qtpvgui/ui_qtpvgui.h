/********************************************************************************
** Form generated from reading UI file 'qtpvgui.ui'
**
** Created: Mon Jul 18 13:51:46 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTPVGUI_H
#define UI_QTPVGUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QStackedWidget>
#include <QtGui/QWidget>
#include "qtpvwidgets.h"

QT_BEGIN_NAMESPACE

class Ui_QEpicsPvGUI
{
public:
    QHBoxLayout *horizontalLayout;
    QMLineEdit *pv;
    QStackedWidget *set;
    QWidget *lineW;
    QHBoxLayout *horizontalLayout_2;
    QMLineEdit *lineBox;
    QWidget *doubleW;
    QHBoxLayout *horizontalLayout_4;
    QMDoubleSpinBox *doubleBox;
    QWidget *enumW;
    QHBoxLayout *horizontalLayout_5;
    QMComboBox *enumBox;
    QWidget *intW;
    QHBoxLayout *horizontalLayout_3;
    QMSpinBox *intBox;
    QLabel *get;

    void setupUi(QWidget *QEpicsPvGUI)
    {
        if (QEpicsPvGUI->objectName().isEmpty())
            QEpicsPvGUI->setObjectName(QString::fromUtf8("QEpicsPvGUI"));
        QEpicsPvGUI->resize(833, 29);
        horizontalLayout = new QHBoxLayout(QEpicsPvGUI);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        pv = new QMLineEdit(QEpicsPvGUI);
        pv->setObjectName(QString::fromUtf8("pv"));

        horizontalLayout->addWidget(pv);

        set = new QStackedWidget(QEpicsPvGUI);
        set->setObjectName(QString::fromUtf8("set"));
        set->setEnabled(false);
        lineW = new QWidget();
        lineW->setObjectName(QString::fromUtf8("lineW"));
        horizontalLayout_2 = new QHBoxLayout(lineW);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetMinAndMaxSize);
        lineBox = new QMLineEdit(lineW);
        lineBox->setObjectName(QString::fromUtf8("lineBox"));

        horizontalLayout_2->addWidget(lineBox);

        set->addWidget(lineW);
        doubleW = new QWidget();
        doubleW->setObjectName(QString::fromUtf8("doubleW"));
        horizontalLayout_4 = new QHBoxLayout(doubleW);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        doubleBox = new QMDoubleSpinBox(doubleW);
        doubleBox->setObjectName(QString::fromUtf8("doubleBox"));
        doubleBox->setDecimals(9);
        doubleBox->setMinimum(-2.14748e+09);
        doubleBox->setMaximum(2.14748e+09);

        horizontalLayout_4->addWidget(doubleBox);

        set->addWidget(doubleW);
        enumW = new QWidget();
        enumW->setObjectName(QString::fromUtf8("enumW"));
        horizontalLayout_5 = new QHBoxLayout(enumW);
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        enumBox = new QMComboBox(enumW);
        enumBox->setObjectName(QString::fromUtf8("enumBox"));

        horizontalLayout_5->addWidget(enumBox);

        set->addWidget(enumW);
        intW = new QWidget();
        intW->setObjectName(QString::fromUtf8("intW"));
        horizontalLayout_3 = new QHBoxLayout(intW);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        intBox = new QMSpinBox(intW);
        intBox->setObjectName(QString::fromUtf8("intBox"));
        intBox->setMinimum(-2147483647);
        intBox->setMaximum(2147483647);

        horizontalLayout_3->addWidget(intBox);

        set->addWidget(intW);

        horizontalLayout->addWidget(set);

        get = new QLabel(QEpicsPvGUI);
        get->setObjectName(QString::fromUtf8("get"));
        get->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout->addWidget(get);


        retranslateUi(QEpicsPvGUI);

        set->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(QEpicsPvGUI);
    } // setupUi

    void retranslateUi(QWidget *QEpicsPvGUI)
    {
        QEpicsPvGUI->setWindowTitle(QApplication::translate("QEpicsPvGUI", "Form", 0, QApplication::UnicodeUTF8));
        pv->setText(QString());
        lineBox->setText(QApplication::translate("QEpicsPvGUI", "Disconnected", 0, QApplication::UnicodeUTF8));
        get->setText(QApplication::translate("QEpicsPvGUI", "Disconnected", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QEpicsPvGUI: public Ui_QEpicsPvGUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTPVGUI_H
