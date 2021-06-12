#include "CEDXPipelineState.h"

//TODO: Implement!!!
#define D3D12_PIPELINE_STATE_STREAM_ALIGNMENT (sizeof(void*))
#define D3D12_ENABLE_PIX_MARKERS 0

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGraphicsPipelineState::CEDXGraphicsPipelineState(
	CEDXDevice* device): CEDXDeviceElement(device), PipelineState(nullptr), RootSignature(nullptr) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGraphicsPipelineState::Create(
	const CEGraphicsPipelineStateCreateInfo& createInfo) {


    struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT) GraphicsPipelineStream
    {
        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type0 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE;
            ID3D12RootSignature* RootSignature = nullptr;
        };

        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type1 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT;
            D3D12_INPUT_LAYOUT_DESC InputLayout = { };
        };

        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type2 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY;
            D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType = { };
        };

        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type3 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS;
            D3D12_SHADER_BYTECODE VertexShader = { };
        };

        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type4 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS;
            D3D12_SHADER_BYTECODE PixelShader = { };
        };

        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type5 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS;
            D3D12_RT_FORMAT_ARRAY RenderTargetInfo = { };
        };

        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type6 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT;
            DXGI_FORMAT DepthBufferFormat = { };
        };

        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type7 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER;
            D3D12_RASTERIZER_DESC RasterizerDesc = { };
        };

        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type8 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL;
            D3D12_DEPTH_STENCIL_DESC DepthStencilDesc = { };
        };

        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type9 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND;
            D3D12_BLEND_DESC BlendStateDesc = { };
        };

        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type10 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC;
            DXGI_SAMPLE_DESC SampleDesc = { };
        };
    } pipelineStream;

	D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc = pipelineStream.InputLayout;

	CEDXInputLayoutState* dxInputLayoutState = static_cast<CEDXInputLayoutState*>(createInfo.InputLayoutState);
	
	return true;
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXComputePipelineState::CEDXComputePipelineState(CEDXDevice* device,
	const CERef<CEDXComputeShader>& shader): CEComputePipelineState(), CEDXDeviceElement(device),
	                                         PipelineState(nullptr), Shader(shader), RootSignature(nullptr) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXComputePipelineState::Create() {
	return false;
}

struct CEDXRootSignatureAssociation {
	CEDXRootSignatureAssociation(ID3D12RootSignature* rootSignature, const CEArray<std::wstring>& shaderExportNames) : ExportAssociation(), RootSignature(rootSignature), ShaderExportNames(shaderExportNames), ShaderExportNamesRef(shaderExportNames.Size()) {
		for (uint32 i = 0; i < ShaderExportNames.Size(); i++) {
			ShaderExportNamesRef[i] = ShaderExportNames[i].c_str();
		}
	}

	D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION ExportAssociation;
	ID3D12RootSignature* RootSignature;
	CEArray<std::wstring> ShaderExportNames;
	CEArray<LPCWSTR> ShaderExportNamesRef;
};

struct CEDXHitGroup {
	CEDXHitGroup(std::wstring hitGroupName, std::wstring closestHit, std::wstring anyHit, std::wstring intersection) : Desc(), HitGroupName(hitGroupName), ClosestHit(closestHit), AnyHit(anyHit), Intersection(intersection){
		ConceptEngine::Memory::CEMemory::Memzero(&Desc);

		Desc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
		Desc.HitGroupExport = HitGroupName.c_str();
		Desc.ClosestHitShaderImport = ClosestHit.c_str();

		if (AnyHit != L"") {
			Desc.AnyHitShaderImport = AnyHit.c_str();
		}

		if (Desc.Type != D3D12_HIT_GROUP_TYPE_TRIANGLES) {
			Desc.IntersectionShaderImport = Intersection.c_str();
		}
	}

	D3D12_HIT_GROUP_DESC Desc;
	std::wstring HitGroupName;
	std::wstring ClosestHit;
	std::wstring AnyHit;
	std::wstring Intersection;
};

struct CEDXLibrary {
	CEDXLibrary(D3D12_SHADER_BYTECODE byteCode, const CEArray<std::wstring>& exportNames): ExportNames(exportNames), ExportDescs(exportNames.Size()), Desc() {
		for (uint32 i = 0; i < ExportDescs.Size(); i++) {
			D3D12_EXPORT_DESC& tempDesc = ExportDescs[i];
			tempDesc.Flags = D3D12_EXPORT_FLAG_NONE;
			tempDesc.Name = ExportNames[i].c_str();
			tempDesc.ExportToRename = nullptr;
		}

		Desc.DXILLibrary = byteCode;
		Desc.pExports = ExportDescs.Data();
		Desc.NumExports = ExportDescs.Size();
	}

	CEArray<std::wstring> ExportNames;
	CEArray<D3D12_EXPORT_DESC> ExportDescs;
	D3D12_DXIL_LIBRARY_DESC Desc;
};

struct CEDXRayTracingPipelineStateStream {

	void AddLibrary(D3D12_SHADER_BYTECODE byteCode, const CEArray<std::wstring>& exportNames) {
		Libraries.EmplaceBack(byteCode, exportNames);
	}

	void AddHitGroup(std::wstring hitGroupName, std::wstring closestHit, std::wstring anyHit, std::wstring intersection) {
		HitGroups.EmplaceBack(hitGroupName, closestHit, anyHit, intersection);
	}

	void AddRootSignatureAssociation(ID3D12RootSignature* rootSignature, const CEArray<std::wstring>& shaderExportNames) {
		RootSignatureAssociations.EmplaceBack(rootSignature, shaderExportNames);
	}

	void Generate() {
		uint32 numSubObjects = Libraries.Size() + HitGroups.Size() + (RootSignatureAssociations.Size() * 2) + 4;
		SubObjects.Resize(numSubObjects);

		uint32 subObjectIndex = 0;
		for (CEDXLibrary& library: Libraries) {
			D3D12_STATE_SUBOBJECT& subObject = SubObjects[subObjectIndex++];
			subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
			subObject.pDesc = &library.Desc;
		}

		for (CEDXHitGroup& hitGroup: HitGroups) {
			D3D12_STATE_SUBOBJECT& subObject = SubObjects[subObjectIndex++];
			subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
			subObject.pDesc = &hitGroup.Desc;
		}

		for(CEDXRootSignatureAssociation& association : RootSignatureAssociations) {
			D3D12_STATE_SUBOBJECT& localRootSubObject = SubObjects[subObjectIndex++];
			localRootSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
			localRootSubObject.pDesc = &association.RootSignature;

			association.ExportAssociation.pExports = association.ShaderExportNamesRef.Data();
			association.ExportAssociation.NumExports = association.ShaderExportNamesRef.Size();
			association.ExportAssociation.pSubobjectToAssociate = &SubObjects[subObjectIndex - 1];

			D3D12_STATE_SUBOBJECT& subObject = SubObjects[subObjectIndex++];
			subObject.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
			subObject.pDesc = &association.ExportAssociation;
		}

		D3D12_STATE_SUBOBJECT& globalRootSubObject = SubObjects[subObjectIndex++];
		globalRootSubObject.Type  = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
		globalRootSubObject.pDesc = &GlobalRootSignature;

		D3D12_STATE_SUBOBJECT& pipelineConfigSubObject = SubObjects[subObjectIndex++];
		pipelineConfigSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
		pipelineConfigSubObject.pDesc = &PipelineConfig;

		D3D12_STATE_SUBOBJECT& shaderConfigSubObject = SubObjects[subObjectIndex++];
		shaderConfigSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
		shaderConfigSubObject.pDesc = &ShaderConfig;

		PayLoadExportNamesRef.Resize(PayLoadExportNames.Size());
		for (uint32 i = 0; i < PayLoadExportNames.Size(); i++) {
			PayLoadExportNamesRef[i] = PayLoadExportNames[i].c_str();
		}

		ShaderConfigAssociation.pExports = PayLoadExportNamesRef.Data();
		ShaderConfigAssociation.NumExports = PayLoadExportNamesRef.Size();
		ShaderConfigAssociation.pSubobjectToAssociate = &SubObjects[subObjectIndex - 1];

		D3D12_STATE_SUBOBJECT& shaderConfigAssociationSubObject = SubObjects[subObjectIndex++];
		shaderConfigAssociationSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
		shaderConfigAssociationSubObject.pDesc = &ShaderConfigAssociation;
	}

	CEArray<CEDXLibrary> Libraries;
	CEArray<CEDXHitGroup> HitGroups;
	CEArray<CEDXRootSignatureAssociation> RootSignatureAssociations;

	D3D12_RAYTRACING_PIPELINE_CONFIG PipelineConfig;
	D3D12_RAYTRACING_SHADER_CONFIG ShaderConfig;
	D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION ShaderConfigAssociation;

	CEArray<std::wstring> PayLoadExportNames;
	CEArray<LPCWSTR> PayLoadExportNamesRef;

	ID3D12RootSignature* GlobalRootSignature;
	CEArray<D3D12_STATE_SUBOBJECT> SubObjects;
};

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingPipelineState::CEDXRayTracingPipelineState(
	CEDXDevice* device): CEDXDeviceElement(device), StateObject(nullptr) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingPipelineState::Create(
	const CERayTracingPipelineStateCreateInfo& createInfo) {
	return false;
}

void* ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXRayTracingPipelineState::GetShaderIdentifier(
	const std::string& exportName) {
	auto mapItem = ShaderIdentifiers.find(exportName);
	if (mapItem == ShaderIdentifiers.end()) {
		std::wstring wName = ConvertToWString(exportName);

		void* result = StateObjectProperties->GetShaderIdentifier(wName.c_str());
		if (!result) {
			return nullptr;
		}

		CERayTracingShaderIdentifier identifier;
		Memory::CEMemory::Memcpy(identifier.ShaderIdentifier, result, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

		auto newIdentifier = ShaderIdentifiers.insert(std::make_pair(exportName, identifier));
		return newIdentifier.first->second.ShaderIdentifier;
	} else {
		return mapItem->second.ShaderIdentifier;
	}
}
