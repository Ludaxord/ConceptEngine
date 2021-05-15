#pragma once
#include <string>

#include "CEShader.h"
#include "../../../Core/Containers/CEArray.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	enum class CEShaderModel {
		Unknown = 0,
		SM_5_0 = 1,
		SM_5_1 = 2,
		SM_6_0 = 3,
		SM_6_1 = 4,
		SM_6_2 = 5,
		SM_6_3 = 6,
		SM_6_4 = 7,
		SM_6_5 = 8,
	};

	struct CEShaderDefine {
		CEShaderDefine(const std::string& define) : Define(define), Value() {

		}

		CEShaderDefine(const std::string& define, const std::string& value) : Define(define), Value(value) {

		}

		std::string Define;
		std::string Value;
	};

	class CEIShaderCompiler {
	public:
		virtual ~CEIShaderCompiler() = default;

		virtual bool Create() = 0;

		virtual bool CompileFromFile(const std::string& filePath,
		                             const std::string& entryPoint,
		                             const Core::Containers::CEArray<CEShaderDefine>* defines,
		                             CEShaderStage shaderStage,
		                             CEShaderModel shaderModel,
		                             Core::Containers::CEArray<uint8>& code) = 0;

		virtual bool CompileShader(const std::string& shaderSource,
		                           const std::string& entryPoint,
		                           const Core::Containers::CEArray<CEShaderDefine>* defines,
		                           CEShaderStage shaderStage,
		                           CEShaderModel shaderModel,
		                           Core::Containers::CEArray<uint8>& code) = 0;
	};
}
