#include "CEOpenGL.h"

void CEOpenGL::Update(CETimestamp DeltaTime, boost::function<void()> ExecuteFunction) {
}

bool CEOpenGL::Resize() {
	return false;
}

bool CEOpenGL::CreateGraphicsManager() {
	return false;
}

bool CEOpenGL::CreateTextureManager() {
	return false;
}

bool CEOpenGL::CreateMeshManager() {
	return false;
}

bool CEOpenGL::CreateShaderCompiler() {
	return false;
}

bool CEOpenGL::CreateDebugUI() {
	return false;
}
