#include "CECommandList.h"

using namespace ConceptEngine::Graphics::Main::RenderLayer;

CECommandListExecutor CommandListExecutor;

void CECommandListExecutor::ExecuteCommandList(CECommandList& commandList) {
	Assert(commandList.IsRecording == false);
	CERenderCommand* command = commandList.FirstCommand;
	CE_LOG_VERBOSE("[CECommandListExecutor]: Execute Command List Start ======");
	while (command != nullptr) {
		CERenderCommand* oldCommand = command;
		command = command->NextCommand;
		if (command != nullptr) {
			CE_LOG_DEBUGX("[CECommandListExecutor]: Execute =>" + command->GetName());
		}

		oldCommand->Execute(GetContext());
		oldCommand->~CERenderCommand();
	}
	CE_LOG_VERBOSE("[CECommandListExecutor]: Execute Command List End ======");

	commandList.FirstCommand = nullptr;
	commandList.LastCommand = nullptr;
	commandList.Reset();
}

void CECommandListExecutor::WaitForGPU() {
	CommandContext->Flush();
}
