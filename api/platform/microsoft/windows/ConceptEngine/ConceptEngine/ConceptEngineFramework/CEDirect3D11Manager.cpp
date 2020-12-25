#include "CEDirect3D11Manager.h"

#include "CED3D11Device.h"

CEDirect3D11Manager::CEDirect3D11Manager(const std::wstring& name, int width, int height, bool vSync): CEManager(
	name, width, height, vSync) {
}

bool CEDirect3D11Manager::LoadContent() {
	return false;
}

void CEDirect3D11Manager::UnloadContent() {
}

void CEDirect3D11Manager::OnUpdate() {
}

void CEDirect3D11Manager::OnRender() {
}

void CEDirect3D11Manager::OnKeyPressed() {
}

void CEDirect3D11Manager::OnMouseWheel() {
}

void CEDirect3D11Manager::OnResize() {
}

bool CEDirect3D11Manager::Initialize() {
	return false;
}

void CEDirect3D11Manager::Destroy() {
}

void CEDirect3D11Manager::OnKeyReleased() {
}

void CEDirect3D11Manager::OnMouseMoved() {
}

void CEDirect3D11Manager::OnMouseButtonPressed() {
}

void CEDirect3D11Manager::OnMouseButtonReleased() {
}

void CEDirect3D11Manager::OnWindowDestroy() {
}

CEDevice CEDirect3D11Manager::GetDevice() {
	return CED3D11Device();
}
