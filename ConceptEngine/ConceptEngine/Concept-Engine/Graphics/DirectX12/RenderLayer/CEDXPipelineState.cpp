#include "CEDXPipelineState.h"

#include "../../../Core/Debug/CEDebug.h"

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
 if (!dxInputLayoutState)
    {
        inputLayoutDesc.pInputElementDescs = nullptr;
        inputLayoutDesc.NumElements = 0;
    }
    else
    {
        inputLayoutDesc = dxInputLayoutState->GetDesc();
    }

    CEArray<CEDXBaseShader*> ShadersWithRootSignature;
    CEArray<CEDXBaseShader*> BaseShaders;

    // VertexShader
    CEDXVertexShader* DxVertexShader = static_cast<CEDXVertexShader*>(createInfo.ShaderState.VertexShader);
    Assert(DxVertexShader != nullptr);

    if (DxVertexShader->HasRootSignature())
    {
        ShadersWithRootSignature.EmplaceBack(DxVertexShader);
    }

    D3D12_SHADER_BYTECODE& VertexShader = pipelineStream.VertexShader;
    VertexShader = DxVertexShader->GetByteCode();
    BaseShaders.EmplaceBack(DxVertexShader);

    // PixelShader
    CEDXPixelShader* DxPixelShader = static_cast<CEDXPixelShader*>(createInfo.ShaderState.PixelShader);

    D3D12_SHADER_BYTECODE& PixelShader = pipelineStream.PixelShader;
    if (DxPixelShader)
    {
        PixelShader = DxPixelShader->GetByteCode();
        BaseShaders.EmplaceBack(DxPixelShader);

        if (DxPixelShader->HasRootSignature())
        {
            ShadersWithRootSignature.EmplaceBack(DxPixelShader);
        }
    }
    else
    {
        PixelShader.pShaderBytecode = nullptr;
        PixelShader.BytecodeLength  = 0;
    }

    // RenderTarget
    const uint32 NumRenderTargets = createInfo.PipelineFormats.NumRenderTargets;

    D3D12_RT_FORMAT_ARRAY& RenderTargetInfo = pipelineStream.RenderTargetInfo;
    RenderTargetInfo.NumRenderTargets = NumRenderTargets;
    for (uint32 Index = 0; Index < NumRenderTargets; Index++)
    {
        RenderTargetInfo.RTFormats[Index] = ConvertFormat(createInfo.PipelineFormats.RenderTargetFormats[Index]);
    }

    // DepthStencil
    pipelineStream.DepthBufferFormat = ConvertFormat(createInfo.PipelineFormats.DepthStencilFormat);

    // RasterizerState
    CEDXRasterizerState* DxRasterizerState = static_cast<CEDXRasterizerState*>(createInfo.RasterizerState);
    Assert(DxRasterizerState != nullptr);

    D3D12_RASTERIZER_DESC& RasterizerDesc = pipelineStream.RasterizerDesc;
    RasterizerDesc = DxRasterizerState->GetDesc();

    // DepthStencilState
    CEDXDepthStencilState* DxDepthStencilState = static_cast<CEDXDepthStencilState*>(createInfo.DepthStencilState);
    Assert(DxDepthStencilState != nullptr);

    D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc = pipelineStream.DepthStencilDesc;
    DepthStencilDesc = DxDepthStencilState->GetDesc();

    // BlendState
    CEDXBlendState* DxBlendState = static_cast<CEDXBlendState*>(createInfo.BlendState);
    Assert(DxBlendState != nullptr);

    D3D12_BLEND_DESC& BlendStateDesc = pipelineStream.BlendStateDesc;
    BlendStateDesc = DxBlendState->GetDesc();

    // Topology
    pipelineStream.PrimitiveTopologyType = ConvertPrimitiveTopologyType(createInfo.PrimitiveTopologyType);

    // MSAA
    DXGI_SAMPLE_DESC& SamplerDesc = pipelineStream.SampleDesc;
    SamplerDesc.Count   = createInfo.SampleCount;
    SamplerDesc.Quality = createInfo.SampleQuality;

    // RootSignature
    if (ShadersWithRootSignature.IsEmpty())
    {
        CEDXRootSignatureResourceCount ResourceCounts;
        ResourceCounts.Type                = CERootSignatureType::Graphics;
        // TODO: Check if any shader actually uses the input assembler
        ResourceCounts.AllowInputAssembler = true;

        // NOTE: For now all constants are put in visibility_all
        uint32 Num32BitConstants = 0;
        for (CEDXBaseShader* DxShader : BaseShaders)
        {
            uint32 Index = DxShader->GetShaderVisibility();
            ResourceCounts.ResourceCounts[Index] = DxShader->GetResourceCount();
            Num32BitConstants = Math::CEMath::Max<uint32>(Num32BitConstants, ResourceCounts.ResourceCounts[Index].Num32BitConstants);
            ResourceCounts.ResourceCounts[Index].Num32BitConstants = 0;
        }

        ResourceCounts.ResourceCounts[ShaderVisibility_All].Num32BitConstants = Num32BitConstants;

        RootSignature = CEDXRootSignatureCache::Get().GetRootSignature(ResourceCounts);
    }
    else
    {
        // TODO: Maybe use all shaders and create one that fits all
        D3D12_SHADER_BYTECODE ByteCode = ShadersWithRootSignature.Front()->GetByteCode();

        RootSignature = new CEDXRootSignature(GetDevice());
        if (!RootSignature->Create(ByteCode.pShaderBytecode, ByteCode.BytecodeLength))
        {
            return false;
        }
        else
        {
            RootSignature->SetName("Custom Graphics RootSignature");
        }
    }

    Assert(RootSignature != nullptr);

    pipelineStream.RootSignature = RootSignature->GetRootSignature();
    
    // Create PipelineState
    D3D12_PIPELINE_STATE_STREAM_DESC PipelineStreamDesc;
    Memory::CEMemory::Memzero(&PipelineStreamDesc);

    PipelineStreamDesc.pPipelineStateSubobjectStream = &pipelineStream;
    PipelineStreamDesc.SizeInBytes                   = sizeof(GraphicsPipelineStream);

    Microsoft::WRL::ComPtr<ID3D12PipelineState> NewPipelineState;
    HRESULT Result = GetDevice()->CreatePipelineState(&PipelineStreamDesc, IID_PPV_ARGS(&NewPipelineState));
    if (FAILED(Result))
    {
        CE_LOG_ERROR("[CEDXGraphicsPipelineState]: FAILED to Create GraphicsPipelineState");
        return false;
    }

    PipelineState = NewPipelineState;
	
	return true;
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXComputePipelineState::CEDXComputePipelineState(CEDXDevice* device,
	const CERef<CEDXComputeShader>& shader): CEComputePipelineState(), CEDXDeviceElement(device),
	                                         PipelineState(nullptr), Shader(shader), RootSignature(nullptr) {
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXComputePipelineState::Create() {
	
    struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT) ComputePipelineStream
    {
        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type0 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE;
            ID3D12RootSignature* RootSignature = nullptr;
        };

        struct alignas(D3D12_PIPELINE_STATE_STREAM_ALIGNMENT)
        {
            D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type1 = D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS;
            D3D12_SHADER_BYTECODE ComputeShader = { };
        };
    } PipelineStream;

	PipelineStream.ComputeShader = Shader->GetByteCode();

	if (!Shader->HasRootSignature()) {
		CE_LOG_INFO("[CEDXComputePipelineState]: Shader Has Root Signature");
		CEDXRootSignatureResourceCount resourceCounts;
		resourceCounts.Type = CERootSignatureType::Compute;
		resourceCounts.AllowInputAssembler = false;
		resourceCounts.ResourceCounts[ShaderVisibility_All]= Shader->GetResourceCount();

		RootSignature = CEDXRootSignatureCache::Get().GetRootSignature(resourceCounts);
	} else {
		CE_LOG_INFO("[CEDXComputePipelineState]: Shader Does not Have Root Signature");
		D3D12_SHADER_BYTECODE byteCode = Shader->GetByteCode();
		
		RootSignature = new CEDXRootSignature(GetDevice());
		if (!RootSignature->Create(byteCode.pShaderBytecode, byteCode.BytecodeLength)) {
			return false;
		}

		RootSignature->SetName("Custom Compute Root Signature");
	}

	Assert(RootSignature != nullptr);

	PipelineStream.RootSignature = RootSignature->GetRootSignature();

	D3D12_PIPELINE_STATE_STREAM_DESC PipelineStreamDesc;
	ConceptEngine::Memory::CEMemory::Memzero(&PipelineStreamDesc, sizeof(D3D12_PIPELINE_STATE_STREAM_DESC));

	PipelineStreamDesc.pPipelineStateSubobjectStream = &PipelineStream;
	PipelineStreamDesc.SizeInBytes = sizeof(ComputePipelineStream);

	HRESULT result = GetDevice()->CreatePipelineState(&PipelineStreamDesc, IID_PPV_ARGS(&PipelineState));
	if (FAILED(result)) {
		CE_LOG_ERROR("[CEDXComputePipelineState]: Failed to Create Compute Pipeline State");
		return false;
	}
	
	return true;
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
	CEDXRayTracingPipelineStateStream PipelineStateStream;

	CEArray<CEDXBaseShader*> Shaders;
	CEDXRayGenShader* RayGen = static_cast<CEDXRayGenShader*>(createInfo.RayGen);
	Shaders.EmplaceBack(RayGen);

	CEDXRootSignatureResourceCount RayGenLocalResourceCounts;
	RayGenLocalResourceCounts.Type = CERootSignatureType::RayTracingLocal;
	RayGenLocalResourceCounts.AllowInputAssembler = false;
	RayGenLocalResourceCounts.ResourceCounts[ShaderVisibility_All] = RayGen->GetRTLocalResourceCount();

	RayGenLocalRootSignature = MakeSharedRef<CEDXRootSignature>(CEDXRootSignatureCache::Get().GetRootSignature(RayGenLocalResourceCounts));
	if (!RayGenLocalRootSignature) {
		return false;
	}

	std::wstring rayGenIdentifier = ConvertToWString(RayGen->GetIdentifier());
	PipelineStateStream.AddLibrary(RayGen->GetByteCode(), {rayGenIdentifier});
	PipelineStateStream.AddRootSignatureAssociation(RayGenLocalRootSignature->GetRootSignature(), {rayGenIdentifier});
	PipelineStateStream.PayLoadExportNames.EmplaceBack(rayGenIdentifier);

	std::wstring HitGroupName;
	std::wstring ClosestHitName;
	std::wstring AnyHitName;

	for (const CERayTracingHitGroup& hitGroup : createInfo.HitGroups) {
		CEDXRayAnyHitShader * DXAnyHit = static_cast<CEDXRayAnyHitShader*>(hitGroup.AnyHit);
		CEDXRayClosestHitShader* DXClosestHit = static_cast<CEDXRayClosestHitShader*>(hitGroup.ClosestHit);

		Assert(DXClosestHit != nullptr);

		HitGroupName = ConvertToWString(hitGroup.Name);
		ClosestHitName = ConvertToWString(DXClosestHit->GetIdentifier());
		AnyHitName = DXAnyHit ? ConvertToWString(DXAnyHit->GetIdentifier()) : L"";

		PipelineStateStream.AddHitGroup(HitGroupName, ClosestHitName, AnyHitName, L"");
	}

	for (CERayAnyHitShader* hit : createInfo.AnyHitShaders) {
		CEDXRayAnyHitShader* dxAnyHit = static_cast<CEDXRayAnyHitShader*>(hit);
		Shaders.EmplaceBack(dxAnyHit);

		CEDXRootSignatureResourceCount anyHitLocalResourceCounts;
		anyHitLocalResourceCounts.Type = CERootSignatureType::RayTracingLocal;
		anyHitLocalResourceCounts.AllowInputAssembler = false;
		anyHitLocalResourceCounts.ResourceCounts[ShaderVisibility_All] = dxAnyHit->GetRTLocalResourceCount();

		HitLocalRootSignature = MakeSharedRef<CEDXRootSignature>(CEDXRootSignatureCache::Get().GetRootSignature(anyHitLocalResourceCounts));
		if (!HitLocalRootSignature) {
			return false;
		}

		std::wstring anyHitIdentifier = ConvertToWString(dxAnyHit->GetIdentifier());
		PipelineStateStream.AddLibrary(dxAnyHit->GetByteCode(), {anyHitIdentifier});
		PipelineStateStream.AddRootSignatureAssociation(HitLocalRootSignature->GetRootSignature(), {anyHitIdentifier});
		PipelineStateStream.PayLoadExportNames.EmplaceBack(anyHitIdentifier);
	}

	for (CERayClosestHitShader* closestHit : createInfo.ClosestHitShaders) {
		CEDXRayClosestHitShader* dxClosestHit = static_cast<CEDXRayClosestHitShader*>(closestHit);
		Shaders.EmplaceBack(dxClosestHit);

		CEDXRootSignatureResourceCount closestHitLocalResourceCounts;
		closestHitLocalResourceCounts.Type = CERootSignatureType::RayTracingLocal;
		closestHitLocalResourceCounts.AllowInputAssembler = false;
		closestHitLocalResourceCounts.ResourceCounts[ShaderVisibility_All] = dxClosestHit->GetRTLocalResourceCount();
		HitLocalRootSignature = MakeSharedRef<CEDXRootSignature>(CEDXRootSignatureCache::Get().GetRootSignature(closestHitLocalResourceCounts));
		if (!HitLocalRootSignature) {
			return false;
		}

		std::wstring closestHitIdentifier = ConvertToWString(dxClosestHit->GetIdentifier());
		PipelineStateStream.AddLibrary(dxClosestHit->GetByteCode(), {closestHitIdentifier});
		PipelineStateStream.AddRootSignatureAssociation(HitLocalRootSignature->GetRootSignature(), {closestHitIdentifier});
		PipelineStateStream.PayLoadExportNames.EmplaceBack(closestHitIdentifier);
	}

	for (CERayMissShader* Miss : createInfo.MissShaders) {
		CEDXRayMissShader* dxMiss = static_cast<CEDXRayMissShader*>(Miss);
		Shaders.EmplaceBack(dxMiss);

		CEDXRootSignatureResourceCount missLocalResourceCounts;
		missLocalResourceCounts.Type = CERootSignatureType::RayTracingLocal;
		missLocalResourceCounts.AllowInputAssembler = false;
		missLocalResourceCounts.ResourceCounts[ShaderVisibility_All] = dxMiss->GetRTLocalResourceCount();

		MissLocalRootSignature = MakeSharedRef<CEDXRootSignature>(CEDXRootSignatureCache::Get().GetRootSignature(missLocalResourceCounts));
		if (!MissLocalRootSignature) {
			return false;
		}

		std::wstring MissIdentifier = ConvertToWString(dxMiss->GetIdentifier());
		PipelineStateStream.AddLibrary(dxMiss->GetByteCode(), {MissIdentifier});
		PipelineStateStream.AddRootSignatureAssociation(MissLocalRootSignature->GetRootSignature(), {MissIdentifier});
		PipelineStateStream.PayLoadExportNames.EmplaceBack(MissIdentifier);
	}

	PipelineStateStream.ShaderConfig.MaxAttributeSizeInBytes = createInfo.MaxAttributeSizeInBytes;
	PipelineStateStream.ShaderConfig.MaxPayloadSizeInBytes = createInfo.MaxPayloadSizeInBytes;
	PipelineStateStream.PipelineConfig.MaxTraceRecursionDepth  = createInfo.MaxRecursionDepth;

	CEShaderResourceCount combinedResourceCount;
	for (CEDXBaseShader* shader : Shaders) {
		
	}

	CEDXRootSignatureResourceCount GlobalResourceCounts;
	GlobalResourceCounts.Type = CERootSignatureType::RayTracingGlobal;
	GlobalResourceCounts.AllowInputAssembler = false;
	GlobalResourceCounts.ResourceCounts[ShaderVisibility_All] = combinedResourceCount;

	GlobalRootSignature = MakeSharedRef<CEDXRootSignature>(CEDXRootSignatureCache::Get().GetRootSignature(GlobalResourceCounts));
	if (!GlobalRootSignature) {
		return false;
	}

	PipelineStateStream.GlobalRootSignature = GlobalRootSignature->GetRootSignature();

	PipelineStateStream.Generate();

	D3D12_STATE_OBJECT_DESC rayTracingPipeline;
	Memory::CEMemory::Memzero(&rayTracingPipeline);

	rayTracingPipeline.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
	rayTracingPipeline.pSubobjects = PipelineStateStream.SubObjects.Data();
	rayTracingPipeline.NumSubobjects = PipelineStateStream.SubObjects.Size();

	Microsoft::WRL::ComPtr<ID3D12StateObject> TempStateObject;
	HRESULT result = GetDevice()->GetDXRDevice()->CreateStateObject(&rayTracingPipeline, IID_PPV_ARGS(&TempStateObject));
	if (FAILED(result)) {
		CEDebug::DebugBreak();
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> TempStateObjectProperties;
	result = TempStateObject->QueryInterface(IID_PPV_ARGS(&TempStateObjectProperties));
	if (FAILED(result)) {
		CE_LOG_ERROR("[CEDXRayTracingPipelineState]: Failed to retrive ID3D12StateObjectProperties" );
		return false;
	}

	StateObject = TempStateObject;
	StateObjectProperties = TempStateObjectProperties;
		
	return true;
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
