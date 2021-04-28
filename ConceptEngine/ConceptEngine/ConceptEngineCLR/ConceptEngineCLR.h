#pragma once

using namespace System;
using namespace System::Reflection;

namespace ConceptEngineCLR {
	public ref class CEBinder {
	public:
		void CompileCSPlayground() {
			ConceptEngineCSharp::CEConsumer::CompilePlayground();
		}

		void GenerateCSPlayground() {
			ConceptEngineCSharp::CEConsumer::GeneratePlayground();
		}

		void CompileCSProject() {
			ConceptEngineCSharp::CEConsumer::CompileProject();
		}

		void GenerateCSProject() {
			ConceptEngineCSharp::CEConsumer::GenerateProject();
		}

		void CompilePyPlayground() {

		}

		void GeneratePyPlayground() {

		}

		void CompilePyProject() {

		}

		void GeneratePyProject() {

		}

		void CompileJSPlayground() {

		}

		void GenerateJSPlayground() {

		}

		void CompileJSProject() {

		}

		void GenerateJSProject() {

		}
	};
}

__declspec(dllexport) void CompileCSProject() {
	ConceptEngineCLR::CEBinder binder;
	binder.CompileCSPlayground();
	binder.CompileCSProject();
}

__declspec(dllexport) void CompilePyProject() {
	ConceptEngineCLR::CEBinder binder;
	binder.CompilePyPlayground();
	binder.CompilePyProject();
}

__declspec(dllexport) void CompileJSProject() {
	ConceptEngineCLR::CEBinder binder;
	binder.CompileJSPlayground();
	binder.CompileJSProject();
}
