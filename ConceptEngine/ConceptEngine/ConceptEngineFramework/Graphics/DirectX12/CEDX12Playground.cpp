#include "CEDX12Playground.h"

#include <spdlog/spdlog.h>


#include "../CEGraphicsManager.h"

using namespace ConceptEngineFramework::Graphics::DirectX12;

CEDX12Playground::CEDX12Playground() {
};

void CEDX12Playground::Init(CEGraphicsManager* manager) {
	CEDX12Manager* dx12Manager = reinterpret_cast<CEDX12Manager*>(manager);
	m_dx12manager = dx12Manager;
}

void CEDX12Playground::Create() {
}

void CEDX12Playground::Update(const CETimer& gt) {
}

void CEDX12Playground::Render(const CETimer& gt) {
}

void CEDX12Playground::Resize() {
}
