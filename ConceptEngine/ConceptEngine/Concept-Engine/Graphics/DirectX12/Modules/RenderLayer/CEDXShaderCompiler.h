#pragma once
#include <d3d12shader.h>
#include <dxcapi.h>
#include <wrl/client.h>

#include "CEDXShader.h"
#include "../../../Main/RenderLayer/CEShaderCompiler.h"

using namespace Microsoft::WRL;

namespace ConceptEngine::Graphics { namespace DirectX12::Modules::RenderLayer {
	using namespace Main::RenderLayer;

	class CEDXShaderCompiler : public CEIShaderCompiler {
	public:
		CEDXShaderCompiler();
		~CEDXShaderCompiler() override;

		bool Create();

		virtual bool CompileFromFile(const std::string& filePath,
		                             const std::string& entryPoint,
		                             const Core::Containers::CEArray<CEShaderDefine>* defines,
		                             CEShaderStage shaderStage,
		                             CEShaderModel shaderModel,
		                             Core::Containers::CEArray<uint8>& code) override;

		virtual bool CompileShader(const std::string& shaderSource,
		                           const std::string& entryPoint,
		                           const Core::Containers::CEArray<CEShaderDefine>* defines,
		                           CEShaderStage shaderStage,
		                           CEShaderModel shaderModel,
		                           Core::Containers::CEArray<uint8>& code) override;

		bool GetReflection(CEDXBaseShader* shader, ID3D12ShaderReflection** reflection);
		bool GetLibraryReflection(CEDXBaseShader* shader, ID3D12LibraryReflection** reflection);

		bool HasRootSignature(CEDXBaseShader* shader);

	private:
		bool InternalCompileFromSource(
			IDxcBlob* sourceBlob,
			LPCWSTR filePath,
			LPCWSTR entryPoint,
			CEShaderStage shaderStage,
			CEShaderModel shaderModel,
			const Core::Containers::CEArray<CEShaderDefine>* defines,
			Core::Containers::CEArray<uint8>& code
		);

		bool InternalGetReflection(const ComPtr<IDxcBlob>& shaderBlob, REFIID IID, void** ppvObject);
		bool ValidateRayTracingShader(const ComPtr<IDxcBlob>& shaderBlob, LPCWSTR entryPoint);

		ComPtr<IDxcCompiler> DXCompiler;
		ComPtr<IDxcLibrary> DXLibrary;
		ComPtr<IDxcLinker> DXLinker;
		ComPtr<IDxcIncludeHandler> DXIncludeHandler;
		ComPtr<IDxcContainerReflection> DXReflection;
		HMODULE DXCompilerDLL;
	};
}}


extern ConceptEngine::Graphics::DirectX12::Modules::RenderLayer::CEDXShaderCompiler* CED3DShaderCompiler;
