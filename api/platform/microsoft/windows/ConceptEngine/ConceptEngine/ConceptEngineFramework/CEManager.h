#pragma once
#include <wrl.h>
#include <string>
#include <memory>

#include "CEDevice.h"

namespace wrl = Microsoft::WRL;

//TODO: Initialize all virtual memebers of class
class CEManager : public std::enable_shared_from_this<CEManager> {
public:
	CEManager() = default;
	CEManager(const std::wstring& name, int width, int height, bool vSync);
	// virtual ~CEManager();
	// virtual bool Initialize();
	// virtual bool LoadContent() = 0;
	// virtual void UnloadContent() = 0;
	// virtual void Destroy();
protected:
	friend class Window;

	virtual CEDevice GetDevice() {
		return CEDevice();
	}

	// virtual void OnUpdate();
	// virtual void OnRender();
	// virtual void OnKeyPressed();
	// virtual void OnKeyReleased();
	// virtual void OnMouseMoved();
	// virtual void OnMouseButtonPressed();
	// virtual void OnMouseButtonReleased();
	// virtual void OnMouseWheel();
	// virtual void OnResize();
	// virtual void OnWindowDestroy();

	std::shared_ptr<Window> m_pWindow;

private:
	std::wstring m_Name;
	int m_Width;
	int m_Height;
	bool m_vSync;
};
