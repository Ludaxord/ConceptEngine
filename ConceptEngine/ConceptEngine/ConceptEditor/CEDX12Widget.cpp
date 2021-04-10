#include "CEDX12Widget.h"

#include "CEDX12WidgetPrivate.h"

CEDX12Widget::CEDX12Widget(QWidget* parent): QWidget(*new CEDX12WidgetPrivate(this), parent, Qt::WindowFlags()) {
}

void CEDX12Widget::Create() {
	Q_D(CEDX12Widget);
	d->Create();
}

QPaintEngine* CEDX12Widget::paintEngine() const {
	return nullptr;
}

void CEDX12Widget::paintEvent(QPaintEvent* event) {
}

void CEDX12Widget::resizeEvent(QResizeEvent* event) {
}
