/********************************************************************************
** Form generated from reading UI file 'ConceptEngineEditorView.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONCEPTENGINEEDITORVIEW_H
#define UI_CONCEPTENGINEEDITORVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConceptEngineEditorViewClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ConceptEngineEditorViewClass)
    {
        if (ConceptEngineEditorViewClass->objectName().isEmpty())
            ConceptEngineEditorViewClass->setObjectName(QString::fromUtf8("ConceptEngineEditorViewClass"));
        ConceptEngineEditorViewClass->resize(600, 400);
        menuBar = new QMenuBar(ConceptEngineEditorViewClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        ConceptEngineEditorViewClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ConceptEngineEditorViewClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ConceptEngineEditorViewClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(ConceptEngineEditorViewClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        ConceptEngineEditorViewClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ConceptEngineEditorViewClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ConceptEngineEditorViewClass->setStatusBar(statusBar);

        retranslateUi(ConceptEngineEditorViewClass);

        QMetaObject::connectSlotsByName(ConceptEngineEditorViewClass);
    } // setupUi

    void retranslateUi(QMainWindow *ConceptEngineEditorViewClass)
    {
        ConceptEngineEditorViewClass->setWindowTitle(QCoreApplication::translate("ConceptEngineEditorViewClass", "ConceptEngineEditorView", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConceptEngineEditorViewClass: public Ui_ConceptEngineEditorViewClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONCEPTENGINEEDITORVIEW_H
