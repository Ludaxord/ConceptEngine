#include "CECommandList.h"

using namespace ConceptEngine::Graphics::Main::RenderLayer;

CECommandListExecutor CommandListExecutor;

void CECommandListExecutor::ExecuteCommandList(CECommandList& commandList) {
	Assert(commandList.IsRecording == false);
	CERenderCommand* command = commandList.FirstCommand;
	while (command != nullptr) {
		CERenderCommand* oldCommand = command;
		command = command->NextCommand;

		oldCommand->Execute(GetContext());
		oldCommand->~CERenderCommand();
	}

	commandList.FirstCommand = nullptr;
	commandList.LastCommand = nullptr;
	commandList.Reset();
}

void CECommandListExecutor::WaitForGPU() {
	CommandContext->Flush();
}
