/********************************************************************************
** Form generated from reading UI file 'CEWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CEWINDOW_H
#define UI_CEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CEWindow
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *CEWindow)
    {
        if (CEWindow->objectName().isEmpty())
            CEWindow->setObjectName(QString::fromUtf8("CEWindow"));
        CEWindow->resize(800, 600);
        centralwidget = new QWidget(CEWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        CEWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(CEWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        CEWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(CEWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        CEWindow->setStatusBar(statusbar);

        retranslateUi(CEWindow);

        QMetaObject::connectSlotsByName(CEWindow);
    } // setupUi

    void retranslateUi(QMainWindow *CEWindow)
    {
        CEWindow->setWindowTitle(QCoreApplication::translate("CEWindow", "CEWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CEWindow: public Ui_CEWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CEWINDOW_H
