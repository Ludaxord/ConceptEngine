#pragma once
#include <QWidget>

class CEDX12WidgetPrivate;

class CEDX12Widget final : public QWidget {
Q_OBJECT
	Q_DECLARE_PRIVATE(CEDX12Widget)

public:
	explicit CEDX12Widget(QWidget* parent = nullptr);

	void Create();

protected:
	// Disable Qt internal rendering process.
	QPaintEngine* paintEngine() const override;

	// Handle painter event.
	virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

	// Handle resize event.
	virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
};
