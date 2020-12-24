#pragma once
#include "CEManager.h"

class CEOpenGLManager : public CEManager {
protected:
	CEDevice GetDevice() override;
};
