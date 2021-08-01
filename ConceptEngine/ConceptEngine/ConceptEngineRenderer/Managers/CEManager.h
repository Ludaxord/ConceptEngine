#pragma once

enum class ManagerType {
	Graphics = 0,
	Texture = 1,
	Mesh = 2
};

class CEManager {
public:
	virtual ~CEManager() = default;
	virtual bool Create() = 0;
	virtual void Release() = 0;
};
