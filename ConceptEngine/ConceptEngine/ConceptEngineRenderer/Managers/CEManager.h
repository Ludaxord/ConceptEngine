#pragma once

class CEManager {
public:
	virtual ~CEManager() = default;
	virtual bool Create() = 0;
	virtual void Release() = 0;
};
