#include "CEWebGL.h"

void CEWebGL::Update(CETimestamp DeltaTime, boost::function<void()> ExecuteFunction) {
}

bool CEWebGL::Resize() {
	return false;
}

bool CEWebGL::CreateGraphicsManager() {
	return false;
}

bool CEWebGL::CreateTextureManager() {
	return false;
}

bool CEWebGL::CreateMeshManager() {
	return false;
}

bool CEWebGL::CreateShaderCompiler() {
	return false;
}

bool CEWebGL::CreateDebugUI() {
	return false;
}
