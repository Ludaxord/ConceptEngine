/********************************************************************************
** Form generated from reading UI file 'ConceptEngineEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONCEPTENGINEEDITOR_H
#define UI_CONCEPTENGINEEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConceptEngineEditorClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ConceptEngineEditorClass)
    {
        if (ConceptEngineEditorClass->objectName().isEmpty())
            ConceptEngineEditorClass->setObjectName(QString::fromUtf8("ConceptEngineEditorClass"));
        ConceptEngineEditorClass->resize(600, 400);
        menuBar = new QMenuBar(ConceptEngineEditorClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        ConceptEngineEditorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ConceptEngineEditorClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ConceptEngineEditorClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(ConceptEngineEditorClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        ConceptEngineEditorClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ConceptEngineEditorClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ConceptEngineEditorClass->setStatusBar(statusBar);

        retranslateUi(ConceptEngineEditorClass);

        QMetaObject::connectSlotsByName(ConceptEngineEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *ConceptEngineEditorClass)
    {
        ConceptEngineEditorClass->setWindowTitle(QCoreApplication::translate("ConceptEngineEditorClass", "ConceptEngineEditor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConceptEngineEditorClass: public Ui_ConceptEngineEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONCEPTENGINEEDITOR_H
