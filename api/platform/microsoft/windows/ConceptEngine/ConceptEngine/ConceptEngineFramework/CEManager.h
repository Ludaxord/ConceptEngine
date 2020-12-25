#pragma once
#include <wrl.h>
#include <string>
#include <memory>

#include "CEDevice.h"

namespace wrl = Microsoft::WRL;

//TODO: Initialize all virtual memebers of class
class CEManager : public std::enable_shared_from_this<CEManager> {

public:
	CEManager(const std::wstring& name, int width, int height, bool vSync) {
	}
	virtual ~CEManager() = default;

	virtual bool Initialize() = 0;
	virtual bool LoadContent() = 0;
	virtual void UnloadContent() = 0;
	virtual void Destroy() = 0;
protected:
	friend class CEWindow;

	virtual CEDevice GetDevice() {
		return CEDevice();
	}

	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnKeyPressed() = 0;
	virtual void OnKeyReleased() = 0;
	virtual void OnMouseMoved() = 0;
	virtual void OnMouseButtonPressed() = 0;
	virtual void OnMouseButtonReleased() = 0;
	virtual void OnMouseWheel() = 0;
	virtual void OnResize() = 0;
	virtual void OnWindowDestroy() = 0;

	std::shared_ptr<CEWindow> m_pWindow;

private:
	std::wstring m_Name;
	int m_Width;
	int m_Height;
	bool m_vSync;
};
