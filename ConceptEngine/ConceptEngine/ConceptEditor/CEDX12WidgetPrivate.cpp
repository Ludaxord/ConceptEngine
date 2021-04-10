#include "CEDX12WidgetPrivate.h"

CEDX12WidgetPrivate::CEDX12WidgetPrivate(CEDX12Widget* parent) {
}

CEDX12WidgetPrivate::~CEDX12WidgetPrivate() {
}

void CEDX12WidgetPrivate::Create() {
	this
		->CreateViewport()
		->LoadPSO()
		->LoadAssets();
}

void CEDX12WidgetPrivate::Render() {
	this
		->OnUpdate()
		->OnRender();
}

void CEDX12WidgetPrivate::Resize() {
	Q_Q(CEDX12Widget);
	m_aspectRatio = q->width() * 1.f / q->height();

	m_viewport.TopLeftX = -100;
	m_viewport.TopLeftY = 100;
	m_viewport.Width = q->width();
	m_viewport.Height = q->height();


	m_scissorRect.right = q->width();
	m_scissorRect.bottom = q->height();
}

void CEDX12WidgetPrivate::GetHardwareAdapter(IDXGIFactory1* pFactory,
                                             IDXGIAdapter1** ppAdapter,
                                             bool requestHighPerformanceAdapter) {
}

CEDX12WidgetPrivate* CEDX12WidgetPrivate::CreateViewport() {

	return this;
}

CEDX12WidgetPrivate* CEDX12WidgetPrivate::LoadPSO() {

	return this;
}

CEDX12WidgetPrivate* CEDX12WidgetPrivate::LoadAssets() {
	return this;
}

CEDX12WidgetPrivate* CEDX12WidgetPrivate::OnUpdate() {
	return this;
}

CEDX12WidgetPrivate* CEDX12WidgetPrivate::OnRender() {
	return this;
}

CEDX12WidgetPrivate* CEDX12WidgetPrivate::PopulateCommandList() {
	return this;
}

CEDX12WidgetPrivate* CEDX12WidgetPrivate::WaitForPreviousFrame() {
	return this;
}
