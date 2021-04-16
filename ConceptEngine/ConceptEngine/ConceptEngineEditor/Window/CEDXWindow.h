#pragma once
#include <QWindow>

#include "QDirect3D12Widget.h"

class CEDXWindow : public QWindow {
Q_OBJECT
public:
	CEDXWindow();
protected:
private:
	QDirect3D12Widget* m_dxWidget;
};
