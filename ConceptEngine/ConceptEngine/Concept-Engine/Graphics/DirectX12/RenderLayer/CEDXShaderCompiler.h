#pragma once
#include <d3d12shader.h>
#include <dxcapi.h>
#include <wrl/client.h>

#include "CEDXShader.h"
#include "../../Main/RenderLayer/CEShaderCompiler.h"

#pragma comment(lib, "dxcompiler.lib")

#ifndef MAKE_FOUR_CC
#define MAKE_FOUR_CC(a,b,c,d) (unsigned int)((unsigned char)(a) | (unsigned char)(b) << 8 | (unsigned char)(c) << 16 | (unsigned char)(d) << 24)
#endif

using namespace Microsoft::WRL;

namespace ConceptEngine::Graphics { namespace DirectX12::RenderLayer {
	using namespace Main::RenderLayer;

	enum DxilFourCC {
		DFCC_Container = MAKEFOURCC('D', 'X', 'B', 'C'),
		DFCC_ResourceDef = MAKEFOURCC('R', 'D', 'E', 'F'),
		DFCC_InputSignature = MAKEFOURCC('I', 'S', 'G', '1'),
		DFCC_OutputSignature = MAKEFOURCC('O', 'S', 'G', '1'),
		DFCC_PatchConstantSignature = MAKEFOURCC('P', 'S', 'G', '1'),
		DFCC_ShaderStatistics = MAKEFOURCC('S', 'T', 'A', 'T'),
		DFCC_ShaderDebugInfoDXIL = MAKEFOURCC('I', 'L', 'D', 'B'),
		DFCC_ShaderDebugName = MAKEFOURCC('I', 'L', 'D', 'N'),
		DFCC_FeatureInfo = MAKEFOURCC('S', 'F', 'I', '0'),
		DFCC_PrivateData = MAKEFOURCC('P', 'R', 'I', 'V'),
		DFCC_RootSignature = MAKEFOURCC('R', 'T', 'S', '0'),
		DFCC_DXIL = MAKEFOURCC('D', 'X', 'I', 'L'),
		DFCC_PipelineStateValidation = MAKEFOURCC('P', 'S', 'V', '0'),
		DFCC_RuntimeData = MAKEFOURCC('R', 'D', 'A', 'T'),
		DFCC_ShaderHash = MAKEFOURCC('H', 'A', 'S', 'H'),
	};

	class CEDXShaderCompiler : public CEIShaderCompiler {
	public:
		CEDXShaderCompiler();
		~CEDXShaderCompiler() override;

		bool Create(bool useLib = true) override;

		bool CreateDLL();

		bool CreateLib();

		virtual bool CompileFromFile(const std::string& filePath,
		                             const std::string& entryPoint,
		                             const CEArray<CEShaderDefine>* defines,
		                             CEShaderStage shaderStage,
		                             CEShaderModel shaderModel,
		                             CEArray<uint8>& code) override;

		bool CompileWithEncodingFromPinned(const std::string& filePath, const std::string& entryPoint,
                                                       const CEArray<CEShaderDefine>* defines,
                                                       CEShaderStage shaderStage, CEShaderModel shaderModel,
                                                       CEArray<uint8>& code) override;

		virtual bool CompileShader(const std::string& shaderSource,
		                           const std::string& entryPoint,
		                           const CEArray<CEShaderDefine>* defines,
		                           CEShaderStage shaderStage,
		                           CEShaderModel shaderModel,
		                           CEArray<uint8>& code) override;

		bool GetReflection(CEDXBaseShader* shader, ID3D12ShaderReflection** reflection);
		bool GetLibraryReflection(CEDXBaseShader* shader, ID3D12LibraryReflection** reflection);

		bool HasRootSignature(CEDXBaseShader* shader);

		bool ValidateShader(IDxcBlob* Blob);

	private:
		bool InternalCompileFromSource(
			IDxcBlob* sourceBlob,
			LPCWSTR filePath,
			LPCWSTR entryPoint,
			CEShaderStage shaderStage,
			CEShaderModel shaderModel,
			const CEArray<CEShaderDefine>* defines,
			CEArray<uint8>& code
		);

		bool InternalGetReflection(const ComPtr<IDxcBlob>& shaderBlob, REFIID IID, void** ppvObject);
		bool ValidateRayTracingShader(const ComPtr<IDxcBlob>& shaderBlob, LPCWSTR entryPoint);

	private:
		ComPtr<IDxcCompiler> DXCompiler;
		ComPtr<IDxcLibrary> DXLibrary;
		ComPtr<IDxcValidator> DXValidator;
		ComPtr<IDxcLinker> DXLinker;
		ComPtr<IDxcIncludeHandler> DXIncludeHandler;
		ComPtr<IDxcContainerReflection> DXReflection;
		HMODULE DXCompilerDLL;
		bool UseLib = false;
	};
}}


extern DxcCreateInstanceProc DxcCreateInstanceFunc;
