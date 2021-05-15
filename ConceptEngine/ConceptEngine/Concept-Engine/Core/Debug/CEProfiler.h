#pragma once
#include "../../Graphics/Main/RenderLayer/CECommandList.h"

using namespace ConceptEngine::Graphics::Main::RenderLayer;

// Function signature as a const char* string
#ifdef COMPILER_VISUAL_STUDIO
    #define __FUNCTION_SIG__ __FUNCTION__
#else
#define __FUNCTION_SIG__ __PRETTY_FUNCTION__
#endif

#define ENABLE_PROFILER 1
#define NUM_PROFILER_SAMPLES 200

#if ENABLE_PROFILER
#define TRACE_SCOPE(Name)      ConceptEngine::Core::Debug::CEScopedTrace PREPROCESS_CONCAT(ScopedTrace_Line_, __LINE__)(Name)
#define TRACE_FUNCTION_SCOPE() TRACE_SCOPE(__FUNCTION_SIG__)

#define GPU_TRACE_SCOPE(CommandList, Name) ConceptEngine::Core::Debug::CEGPUScopedTrace PREPROCESS_CONCAT(ScopedTrace_Line_, __LINE__)(CommandList, Name)
#else
    #define TRACE_SCOPE(Name)
    #define TRACE_FUNCTION_SCOPE()

    #define GPU_TRACE_SCOPE(CmdList, Name)
#endif

namespace ConceptEngine::Core::Debug {

	class CEProfiler {
	public:
		static void Init();
		static void Update();

		static void Enable();
		static void Disable();
		static void Reset();

		static void BeginTraceScope(const char* Name);
		static void EndTraceScope(const char* Name);

		static void BeginGPUFrame(CECommandList& commandList);
		static void BeginGPUTrace(CECommandList& commandList, const char* Name);

		static void EndGPUFrame(CECommandList& commandList);
		static void EndGPUTrace(CECommandList& commandList, const char* Name);

		static void SetGPUProfiler(class ConceptEngine::Graphics::Main::RenderLayer::CEGPUProfiler* Profiler);
	protected:
	private:
	};

	struct CEScopedTrace {
	public:
		CEScopedTrace(const char* name) : Name(name) {
			CEProfiler::BeginTraceScope(Name);
		}

		~CEScopedTrace() {
			CEProfiler::EndTraceScope(Name);
		}

	private:
		const char* Name = nullptr;
	};

	struct CEGPUScopedTrace {
	public:
		CEGPUScopedTrace(CECommandList& commandList, const char* name): CommandList(commandList), Name(name) {
			CEProfiler::BeginGPUTrace(CommandList, Name);
		}

		~CEGPUScopedTrace() {
			CEProfiler::EndGPUTrace(CommandList, Name);
		}

	private:
		CECommandList& CommandList;
		const char* Name = nullptr;
	};
}
