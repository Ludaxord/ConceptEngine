#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ConceptEditor.h"

class ConceptEditor : public QMainWindow
{
    Q_OBJECT

public:
    ConceptEditor(QWidget *parent = Q_NULLPTR);

private:
    Ui::ConceptEditorClass ui;
};
