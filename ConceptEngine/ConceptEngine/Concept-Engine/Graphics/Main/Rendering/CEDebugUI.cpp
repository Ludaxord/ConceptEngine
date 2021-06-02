#include "CEDebugUI.h"

#include <string>

#include "../RenderLayer/CEPipelineState.h"
#include "../RenderLayer/CEBuffer.h"
#include "../RenderLayer/CESamplerState.h"

#include "../../../Core/Containers/CEArray.h"

#include "../../../Time/CETimer.h"


using namespace ConceptEngine::Graphics::Main::Rendering;
using namespace ConceptEngine::Graphics::Main::RenderLayer;
using namespace ConceptEngine::Core::Common;
using namespace ConceptEngine::Core::Containers;
using namespace ConceptEngine::Time;

static CEArray<CEDebugUI::UIDrawFunc> DrawFuncs;
static CEArray<std::string> DebugStrings;

struct ImGuiState {
	void Reset() {
		FontTexture.Reset();
		PipelineState.Reset();
		PipelineStateNoBlending.Reset();
		VertexBuffer.Reset();
		IndexBuffer.Reset();
		PointSampler.Reset();
		PixelShader.Reset();
	}

	ConceptEngine::Time::CETimer FrameClock;

	CERef<CETexture2D> FontTexture;
	CERef<CEGraphicsPipelineState> PipelineState;
	CERef<CEGraphicsPipelineState> PipelineStateNoBlending;
	CERef<CEPixelShader> PixelShader;
	CERef<CEVertexBuffer> VertexBuffer;
	CERef<CEIndexBuffer> IndexBuffer;
	CERef<CESamplerState> PointSampler;
	CEArray<ImGuiImage*> Images;

	ImGuiContext* Context = nullptr;
};

static ImGuiState CEImGuiState;

CEDebugUI::CEDebugUI() {
}

CEDebugUI::~CEDebugUI() {
}

void CEDebugUI::DrawUI(UIDrawFunc DrawFunc) {
	DrawFuncs.EmplaceBack(DrawFunc);
}

ImGuiContext* CEDebugUI::GetCurrentContext() {
	return CEImGuiState.Context;
}
