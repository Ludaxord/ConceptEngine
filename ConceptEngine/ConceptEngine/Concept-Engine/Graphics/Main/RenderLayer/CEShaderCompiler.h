#pragma once
#include <string>

#include "../../../Core/Containers/CEArray.h"
#include "../../../Core/Common/CETypes.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	enum class CEShaderStage {
		Unknown = 0,
		Vertex = 1,
		Hull = 2,
		Domain = 3,
		Geometry = 4,
		Mesh = 5,
		Amplification = 6,
		Pixel = 7,
		Compute = 8,
		RayGen = 9,
		RayAnyHit = 10,
		RayClosestHit = 11,
		RayMiss = 12
	};

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

		virtual bool Create(bool useLib) = 0;

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

	inline bool ShaderStageIsGraphics(CEShaderStage shaderStage) {
		switch (shaderStage) {
		case CEShaderStage::Vertex:
		case CEShaderStage::Hull:
		case CEShaderStage::Domain:
		case CEShaderStage::Geometry:
		case CEShaderStage::Pixel:
		case CEShaderStage::Mesh:
		case CEShaderStage::Amplification: {
			return true;
		}
		default: {
			return false;
		}
		}
	}

	inline bool ShaderStageIsCompute(CEShaderStage shaderStage) {
		switch (shaderStage) {
		case CEShaderStage::Compute:
		case CEShaderStage::RayGen:
		case CEShaderStage::RayClosestHit:
		case CEShaderStage::RayAnyHit:
		case CEShaderStage::RayMiss: {
			return true;
		}
		default: {
			return false;
		}
		}
	}

	inline bool ShaderStageIsRayTracing(CEShaderStage shaderStage) {
		switch (shaderStage) {
		case CEShaderStage::RayGen:
		case CEShaderStage::RayClosestHit:
		case CEShaderStage::RayAnyHit:
		case CEShaderStage::RayMiss: {
			return true;
		}
		default: {
			return false;
		}
		}
	}
}
