#include "CERTXPlayground.h"


#include <iomanip>
#include <sstream>
#include <spdlog/spdlog.h>


#include "CECommandList.h"
#include "CECommandQueue.h"
#include "CEDevice.h"
#include "CEGUI.h"
#include "CERootSignature.h"
#include "CESwapChain.h"
#include "d3dx12.h"
#include "DirectXRaytracingHelper.h"
#include "Raytracing.hlsl.h"

using namespace Concept::GameEngine::Playground;
using namespace Concept::GraphicsEngine::Direct3D;
using namespace DirectX;

#define SizeOfInUint32(obj) ((sizeof(obj) - 1) / sizeof(UINT32) + 1)

// Shader entry points.
const wchar_t* CERTXPlayground::c_raygenShaderName = L"MyRaygenShader";
const wchar_t* CERTXPlayground::c_intersectionShaderNames[] =
{
	L"MyIntersectionShader_AnalyticPrimitive",
	L"MyIntersectionShader_VolumetricPrimitive",
	L"MyIntersectionShader_SignedDistancePrimitive",
};
const wchar_t* CERTXPlayground::c_closestHitShaderNames[] =
{
	L"MyClosestHitShader_Triangle",
	L"MyClosestHitShader_AABB",
};
const wchar_t* CERTXPlayground::c_missShaderNames[] =
{
	L"MyMissShader", L"MyMissShader_ShadowRay"
};
// Hit groups.
const wchar_t* CERTXPlayground::c_hitGroupNames_TriangleGeometry[] =
{
	L"MyHitGroup_Triangle", L"MyHitGroup_Triangle_ShadowRay"
};
const wchar_t* CERTXPlayground::c_hitGroupNames_AABBGeometry[][RayType::Count] =
{
	{L"MyHitGroup_AABB_AnalyticPrimitive", L"MyHitGroup_AABB_AnalyticPrimitive_ShadowRay"},
	{L"MyHitGroup_AABB_VolumetricPrimitive", L"MyHitGroup_AABB_VolumetricPrimitive_ShadowRay"},
	{L"MyHitGroup_AABB_SignedDistancePrimitive", L"MyHitGroup_AABB_SignedDistancePrimitive_ShadowRay"},
};


void PrintStateObjectDesc(const D3D12_STATE_OBJECT_DESC* desc) {
	std::wstringstream wstr;
	wstr << L"\n";
	wstr << L"--------------------------------------------------------------------\n";
	wstr << L"| D3D12 State Object 0x" << static_cast<const void*>(desc) << L": ";
	if (desc->Type == D3D12_STATE_OBJECT_TYPE_COLLECTION) wstr << L"Collection\n";
	if (desc->Type == D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE) wstr << L"Raytracing Pipeline\n";

	auto ExportTree = [](UINT depth, UINT numExports, const D3D12_EXPORT_DESC* exports) {
		std::wostringstream woss;
		for (UINT i = 0; i < numExports; i++) {
			woss << L"|";
			if (depth > 0) {
				for (UINT j = 0; j < 2 * depth - 1; j++) woss << L" ";
			}
			woss << L" [" << i << L"]: ";
			if (exports[i].ExportToRename) woss << exports[i].ExportToRename << L" --> ";
			woss << exports[i].Name << L"\n";
		}
		return woss.str();
	};

	for (UINT i = 0; i < desc->NumSubobjects; i++) {
		wstr << L"| [" << i << L"]: ";
		switch (desc->pSubobjects[i].Type) {
		case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
			wstr << L"Global Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
			break;
		case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
			wstr << L"Local Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
			break;
		case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK:
			wstr << L"Node Mask: 0x" << std::hex << std::setfill(L'0') << std::setw(8) << *static_cast<const UINT*>(desc
				->pSubobjects[i].pDesc) << std::setw(0) << std::dec << L"\n";
			break;
		case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY: {
			wstr << L"DXIL Library 0x";
			auto lib = static_cast<const D3D12_DXIL_LIBRARY_DESC*>(desc->pSubobjects[i].pDesc);
			wstr << lib->DXILLibrary.pShaderBytecode << L", " << lib->DXILLibrary.BytecodeLength << L" bytes\n";
			wstr << ExportTree(1, lib->NumExports, lib->pExports);
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION: {
			wstr << L"Existing Library 0x";
			auto collection = static_cast<const D3D12_EXISTING_COLLECTION_DESC*>(desc->pSubobjects[i].pDesc);
			wstr << collection->pExistingCollection << L"\n";
			wstr << ExportTree(1, collection->NumExports, collection->pExports);
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION: {
			wstr << L"Subobject to Exports Association (Subobject [";
			auto association = static_cast<const D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
			UINT index = static_cast<UINT>(association->pSubobjectToAssociate - desc->pSubobjects);
			wstr << index << L"])\n";
			for (UINT j = 0; j < association->NumExports; j++) {
				wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
			}
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION: {
			wstr << L"DXIL Subobjects to Exports Association (";
			auto association = static_cast<const D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].
				pDesc);
			wstr << association->SubobjectToAssociate << L")\n";
			for (UINT j = 0; j < association->NumExports; j++) {
				wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
			}
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG: {
			wstr << L"Raytracing Shader Config\n";
			auto config = static_cast<const D3D12_RAYTRACING_SHADER_CONFIG*>(desc->pSubobjects[i].pDesc);
			wstr << L"|  [0]: Max Payload Size: " << config->MaxPayloadSizeInBytes << L" bytes\n";
			wstr << L"|  [1]: Max Attribute Size: " << config->MaxAttributeSizeInBytes << L" bytes\n";
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG: {
			wstr << L"Raytracing Pipeline Config\n";
			auto config = static_cast<const D3D12_RAYTRACING_PIPELINE_CONFIG*>(desc->pSubobjects[i].pDesc);
			wstr << L"|  [0]: Max Recursion Depth: " << config->MaxTraceRecursionDepth << L"\n";
			break;
		}
		case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP: {
			wstr << L"Hit Group (";
			auto hitGroup = static_cast<const D3D12_HIT_GROUP_DESC*>(desc->pSubobjects[i].pDesc);
			wstr << (hitGroup->HitGroupExport ? hitGroup->HitGroupExport : L"[none]") << L")\n";
			wstr << L"|  [0]: Any Hit Import: " << (hitGroup->AnyHitShaderImport
				                                        ? hitGroup->AnyHitShaderImport
				                                        : L"[none]") << L"\n";
			wstr << L"|  [1]: Closest Hit Import: " << (hitGroup->ClosestHitShaderImport
				                                            ? hitGroup->ClosestHitShaderImport
				                                            : L"[none]") << L"\n";
			wstr << L"|  [2]: Intersection Import: " << (hitGroup->IntersectionShaderImport
				                                             ? hitGroup->IntersectionShaderImport
				                                             : L"[none]") << L"\n";
			break;
		}
		}
		wstr << L"|--------------------------------------------------------------------\n";
	}
	wstr << L"\n";
	OutputDebugStringW(wstr.str().c_str());
	// spdlog::info(wstr.str().c_str());
}

void AllocateUploadBuffer(wrl::ComPtr<ID3D12Device2> pDevice, void* pData, UINT64 datasize, ID3D12Resource** ppResource,
                          const wchar_t* resourceName = nullptr) {
	auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(datasize);
	ThrowIfFailed(pDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(ppResource)));
	if (resourceName) {
		(*ppResource)->SetName(resourceName);
	}
	void* pMappedData;
	(*ppResource)->Map(0, nullptr, &pMappedData);
	memcpy(pMappedData, pData, datasize);
	(*ppResource)->Unmap(0, nullptr);
}

CERTXPlayground::CERTXPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync) : CEPlayground(
		name, width, height, vSync),
	m_viewPort(CD3DX12_VIEWPORT(
			0.0f,
			0.0f,
			static_cast<float>(width),
			static_cast<float>(height))
	),
	m_scissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX)), m_RenderScale(1.0f) {

	DirectX::XMVECTOR cameraPos = DirectX::XMVectorSet(0, 5, -20, 1);
	DirectX::XMVECTOR cameraTarget = DirectX::XMVectorSet(0, 5, 0, 1);
	DirectX::XMVECTOR cameraUp = DirectX::XMVectorSet(0, 1, 0, 0);

	m_camera.set_LookAt(cameraPos, cameraTarget, cameraUp);
	m_camera.set_Projection(45.0f, width / (float)height, 0.1f, 100.0f);

	m_pAlignedCameraData = (CameraData*)_aligned_malloc(sizeof(CameraData), 16);

	m_pAlignedCameraData->m_initialCamPos = m_camera.get_Translation();
	m_pAlignedCameraData->m_initialCamRot = m_camera.get_Rotation();
	m_pAlignedCameraData->m_InitialFov = m_camera.get_FoV();
}

CERTXPlayground::~CERTXPlayground() {
}

bool CERTXPlayground::LoadContent() {
	/*
	 * Create devices
	 */
	m_device = CEDevice::Create();
	m_swapChain = m_device->CreateSwapChain(m_window->GetWindowHandle(), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_swapChain->SetVSync(m_vSync);

	m_gui = m_device->CreateGUI(m_window->GetWindowHandle(), m_swapChain->GetRenderTarget());

	CEGame::Get().WndProcHandler += CEWindowProcEvent::slot(&CEGUI::WndProcHandler, m_gui);

	auto& commandQueue = m_device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	auto commandList = commandQueue.GetCommandList();

	/*
	 * Create scene based on Microsoft Tutorial.
	 * TODO: Move attributes to CEMaterial, for now just test rtx
	 */
	{
		auto SetAttributes = [&](
			UINT primitiveIndex,
			const XMFLOAT4& albedo,
			float reflectanceCoef = 0.0f,
			float diffuseCoef = 0.9f,
			float specularCoef = 0.7f,
			float specularPower = 50.0f,
			float stepScale = 1.0f
		) {
			auto& attributes = m_aabbMaterialCB[primitiveIndex];
			attributes.albedo = albedo;
			attributes.reflectanceCoef = reflectanceCoef;
			attributes.diffuseCoef = diffuseCoef;
			attributes.specularCoef = specularCoef;
			attributes.specularPower = specularPower;
			attributes.stepScale = stepScale;
		};

		m_planeMaterialCB = {XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f), 0.25f, 1, 0.4f, 50, 1};

		/*
		 * Albedos
		 */
		XMFLOAT4 green = XMFLOAT4(0.1f, 1.0f, 0.5f, 1.0f);
		XMFLOAT4 red = XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f);
		XMFLOAT4 yellow = XMFLOAT4(1.0f, 1.0f, 0.5f, 1.0f);

		UINT offset = 0;
		/*
		 * Analytic primitives
		 */
		{
			using namespace AnalyticPrimitive;
			SetAttributes(offset + AABB, red);
			SetAttributes(offset + Spheres, ChromiumReflectance, 1);
			offset += AnalyticPrimitive::Count;
		}

		/*
		 * Volumetric primitives
		 */
		{
			using namespace VolumetricPrimitive;
			SetAttributes(offset + Metaballs, ChromiumReflectance, 1);
			offset += VolumetricPrimitive::Count;
		}

		/*
		 * Signed distance primitives
		 */
		{
			using namespace SignedDistancePrimitive;
			using namespace SignedDistancePrimitive;
			SetAttributes(offset + MiniSpheres, green);
			SetAttributes(offset + IntersectedRoundCube, green);
			SetAttributes(offset + SquareTorus, ChromiumReflectance, 1);
			SetAttributes(offset + TwistedTorus, yellow, 0, 1.0f, 0.7f, 50, 0.5f);
			SetAttributes(offset + Cog, yellow, 0, 1.0f, 0.1f, 2);
			SetAttributes(offset + Cylinder, red);
			SetAttributes(offset + FractalPyramid, green, 0, 1, 0.1f, 4, 0.8f);
		}

		/*
		 * Camera setup in constructor
		 */
		{
		}

		/*
		 * Setup lights
		 */
		{
			XMFLOAT4 lightPosition;
			XMFLOAT4 lightAmbientColor;
			XMFLOAT4 lightDiffuseColor;

			lightPosition = XMFLOAT4(0.0f, 18.0f, -20.0f, 0.0f);
			m_sceneCB->lightPosition = XMLoadFloat4(&lightPosition);

			lightAmbientColor = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
			m_sceneCB->lightAmbientColor = XMLoadFloat4(&lightAmbientColor);

			float d = 0.6f;
			lightDiffuseColor = XMFLOAT4(d, d, d, 1.0f);
			m_sceneCB->lightDiffuseColor = XMLoadFloat4(&lightDiffuseColor);
		}
	}

	//TODO: Create Root signatures
	{
		CD3DX12_DESCRIPTOR_RANGE1 descriptorRanges[2];
		descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
		descriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);

		CD3DX12_ROOT_PARAMETER1 rootParameters[GlobalRootSignature::Slot::Count];
		rootParameters[GlobalRootSignature::Slot::OutputView].InitAsDescriptorTable(1, &descriptorRanges[0]);
		rootParameters[GlobalRootSignature::Slot::AccelerationStructure].InitAsShaderResourceView(0);
		rootParameters[GlobalRootSignature::Slot::SceneConstant].InitAsConstantBufferView(0);
		rootParameters[GlobalRootSignature::Slot::AABBattributeBuffer].InitAsShaderResourceView(3);
		rootParameters[GlobalRootSignature::Slot::VertexBuffers].InitAsDescriptorTable(1, &descriptorRanges[1]);

		auto d3dDevice = m_device->GetDevice();

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(ARRAYSIZE(rootParameters), rootParameters);
		m_rayTracingGlobalSignature = m_device->CreateRootSignature(rootSignatureDescription.Desc_1_1);
	}

	//TODO: Create local root signature
	/*
	 * This root signature enable shader to have unique arguments that come from shader tables
	 */
	{
		//Triangle geometry
		{
			namespace RootSignatureSlots = LocalRootSignature::Triangle::Slot;
			CD3DX12_ROOT_PARAMETER1 rootParameters[RootSignatureSlots::Count];
			rootParameters[RootSignatureSlots::MaterialConstant].
				InitAsConstants(SizeOfInUint32(PrimitiveConstantBuffer), 1);
			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC localRootSignatureDescription;
			localRootSignatureDescription.Init_1_1(ARRAYSIZE(rootParameters), rootParameters, 0, nullptr,
			                                       D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);
			m_triangleRayTracingLocalSignature = m_device->CreateRootSignature(localRootSignatureDescription.Desc_1_1);
		}

		//AABB geometry
		{
			namespace RootSignatureSlots = LocalRootSignature::AABB::Slot;
			CD3DX12_ROOT_PARAMETER1 rootParameters[RootSignatureSlots::Count];
			rootParameters[RootSignatureSlots::MaterialConstant].
				InitAsConstants(SizeOfInUint32(PrimitiveConstantBuffer), 1);
			rootParameters[RootSignatureSlots::GeometryIndex].InitAsConstants(
				SizeOfInUint32(PrimitiveInstanceConstantBuffer), 2);

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC localRootSignatureDescription;
			localRootSignatureDescription.Init_1_1(ARRAYSIZE(rootParameters), rootParameters, 0, nullptr,
			                                       D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);
			m_AABBRayTracingLocalSignature = m_device->CreateRootSignature(localRootSignatureDescription.Desc_1_1);
		}
	}

	//TODO: Create Ray Tracing Pipeline State Object
	//TODO: Create similar object as PanoToCubemap or Mips_CS
	/*
	 * Create ray tracing state object RTPSO;
	 * An RTPSO represent full set of shaders reachable by DispatchRays() call,
	 * with all configuration options resolved, such as local signatures and other state.
	 */
	{
		/*
		 * Create 18 subobjects that combine int a RTPSO:
		 * Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations.
		 * Default association applies to every exported shader entry point that does not have any of same type of subobject associated with it.
		 * This simple sample utilizes default shader association expect for loacl root signature subobject
		 * which has an explicit association specified purely for demonstration purposes.
		 *
		 * 1 - DXIL Library
		 * 8 - Hit group types - 4 geometries (1 triangle, 3 aabb) x 2 ray types (ray, shadowRay)
		 * 1 - Shader config
		 * 6 - 3 x Local root signature and association
		 * 1 - Global root signature
		 * 1 - Pipeline config
		 */
		CD3DX12_STATE_OBJECT_DESC rayTracingPipeline(D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE);

		/*
		 * DXIL library
		 */
		{
			auto lib = rayTracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
			D3D12_SHADER_BYTECODE libDXIL = CD3DX12_SHADER_BYTECODE((void*)g_pRaytracing, ARRAYSIZE(g_pRaytracing));
			lib->SetDXILLibrary(&libDXIL);
			//Use default shader exports for DXIL library/collection subobject ~ surface all shaders
		}

		/*
		 * Hit groups
		 */
		/*
		 * Hit groups
		 * hit group specifies closest hit, any hit and intersection shaders
		 * to be executed when ray intersects geometry.
		 */
		{
			for (UINT rayType = 0; rayType < RayType::Count; rayType++) {
				auto hitGroup = rayTracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
				if (rayType == RayType::Radiance) {
					hitGroup->SetClosestHitShaderImport(c_closestHitShaderNames[GeometryType::Triangle]);
				}
				hitGroup->SetHitGroupExport(c_hitGroupNames_TriangleGeometry[rayType]);
				hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE);
			}
		}

		/*
		 * Shader config
		 * Defines maximum sizes in bytes for ray rayPayload and attribute structure
		 */
		auto shaderConfig = rayTracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
		UINT payloadSize = max(sizeof(RayPayload), sizeof(ShadowRayPayload));
		UINT attributeSize = sizeof(struct ProceduralPrimitiveAttributes);
		shaderConfig->Config(payloadSize, attributeSize);

		//Local root signature and shader association
		//This is root signature that enables a shader to have unique arguments that come from shader tables;
		// Ray gen and miss shaders in this sample are not using a local root signature and thus one is not associated with them
		{
			//Hit groups
			//Triangle geometry
			{
				auto localRTRootSignature = m_triangleRayTracingLocalSignature->GetD3D12RootSignature();
				auto localRootSignature = rayTracingPipeline.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
				localRootSignature->SetRootSignature(localRTRootSignature.Get());
				//Shader association
				auto rootSignatureAssociation = rayTracingPipeline.CreateSubobject<
					CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
				rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
				rootSignatureAssociation->AddExports(c_hitGroupNames_TriangleGeometry);
			}

			//AABB geometry
			{
				auto localRTRootSignature = m_AABBRayTracingLocalSignature->GetD3D12RootSignature();
				auto localRootSignature = rayTracingPipeline.CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
				localRootSignature->SetRootSignature(localRTRootSignature.Get());
				//Shader association
				auto rootSignatureAssociation = rayTracingPipeline.CreateSubobject<
					CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
				rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
				for (auto& hitGroupsForIntersectionShaderType : c_hitGroupNames_AABBGeometry) {
					rootSignatureAssociation->AddExports(hitGroupsForIntersectionShaderType);
				}
			}
		}

		//Global root signature
		//This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
		auto globalRTRootSignature = m_rayTracingGlobalSignature->GetD3D12RootSignature();
		auto globalRootSignature = rayTracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
		globalRootSignature->SetRootSignature(globalRTRootSignature.Get());

		//Pipeline config
		//Defines maxiumum TraceRay() recursion depth;
		auto pipelineConfig = rayTracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
		//Performance TIP: Set max recursion depth as low as needed
		//as drives may apply optimization strategies for low recursion depth;
		UINT maxRecursionDepth = MAX_RAY_RECURSION_DEPTH;
		pipelineConfig->Config(maxRecursionDepth);

		PrintStateObjectDesc(rayTracingPipeline);

		/*
		 * Create state object
		 */
		auto rtxDevice = m_device->GetRayTracingDevice();
		ThrowIfFailed(rtxDevice->CreateStateObject(rayTracingPipeline, IID_PPV_ARGS(&m_dxrStateObject)));
	}

	// Create heap for descriptors;
	//TODO: Use CEDescriptorAllocatorPage in future
	{
		auto device = m_device->GetDevice();
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		//Allocate heap for 6 descriptors:
		//2 - vertex and index buffer SRVs
		//1 - ray tracing output texture SRV;
		descriptorHeapDesc.NumDescriptors = CESwapChain::BufferCount;
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;
		device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));
		NAME_D3D12_OBJECT(m_descriptorHeap);

		m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//Build geometry to be used in sample
	{
		// Build AABBs for procedural geometry within a bottom-level acceleration structure.
		{
			auto device = m_device->GetDevice();
			// Set up AABBs on a grid
			{
				XMINT3 aabbGrid = XMINT3(4, 1, 4);
				const XMFLOAT3 basePosition = {
					-(aabbGrid.x * c_aabbWidth + (aabbGrid.x - 1) * c_aabbDistance) / 2.0f,
					-(aabbGrid.y * c_aabbWidth + (aabbGrid.y - 1) * c_aabbDistance) / 2.0f,
					-(aabbGrid.z * c_aabbWidth + (aabbGrid.z - 1) * c_aabbDistance) / 2.0f,
				};

				XMFLOAT3 stride = XMFLOAT3(c_aabbWidth + c_aabbDistance, c_aabbWidth + c_aabbDistance,
				                           c_aabbWidth + c_aabbDistance);
				auto InitializeAABB = [&](auto& offsetIndex, auto& size) {
					return D3D12_RAYTRACING_AABB{
						basePosition.x + offsetIndex.x * stride.x,
						basePosition.y + offsetIndex.y * stride.y,
						basePosition.z + offsetIndex.z * stride.z,
						basePosition.x + offsetIndex.x * stride.x + size.x,
						basePosition.y + offsetIndex.y * stride.y + size.y,
						basePosition.z + offsetIndex.z * stride.z + size.z,
					};
				};
				m_aabbs.resize(IntersectionShaderType::TotalPrimitiveCount);
				UINT offset = 0;

				//Analytic primitives
				{
					using namespace AnalyticPrimitive;
					m_aabbs[offset + AABB] = InitializeAABB(XMINT3(3, 0, 0), XMFLOAT3(2, 3, 2));
					m_aabbs[offset + Spheres] = InitializeAABB(XMFLOAT3(2.25f, 0, 0.75f), XMFLOAT3(3, 3, 3));
					offset += AnalyticPrimitive::Count;
				}

				//Volumetric primitives
				{
					using namespace VolumetricPrimitive;
					m_aabbs[offset + Metaballs] = InitializeAABB(XMINT3(0, 0, 0), XMFLOAT3(3, 3, 3));
					offset += VolumetricPrimitive::Count;
				}

				//Signed distance primitives
				{
					using namespace SignedDistancePrimitive;
					m_aabbs[offset + MiniSpheres] = InitializeAABB(XMINT3(2, 0, 0), XMFLOAT3(2, 2, 2));
					m_aabbs[offset + TwistedTorus] = InitializeAABB(XMINT3(0, 0, 1), XMFLOAT3(2, 2, 2));
					m_aabbs[offset + IntersectedRoundCube] = InitializeAABB(XMINT3(0, 0, 2), XMFLOAT3(2, 2, 2));
					m_aabbs[offset + SquareTorus] = InitializeAABB(XMFLOAT3(0.75f, -0.1f, 2.25f), XMFLOAT3(3, 3, 3));
					m_aabbs[offset + Cog] = InitializeAABB(XMINT3(1, 0, 0), XMFLOAT3(2, 2, 2));
					m_aabbs[offset + Cylinder] = InitializeAABB(XMINT3(0, 0, 3), XMFLOAT3(2, 3, 2));
					m_aabbs[offset + FractalPyramid] = InitializeAABB(XMINT3(2, 0, 2), XMFLOAT3(6, 6, 6));
				}

				AllocateUploadBuffer(device, m_aabbs.data(), m_aabbs.size() * sizeof(m_aabbs[0]),
				                     &m_aabbBuffer.resource);
			}
		}
		//Build Plane Geometry
		{
			auto device = m_device->GetDevice();
			//Plane indices
			Index indices[] = {
				3, 1, 0,
				2, 1, 3
			};

			// Cube vertices positions and corresponding triangle normals
			Vertex vertices[] = {
				{XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
				{XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
				{XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
				{XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			};

			AllocateUploadBuffer(device, indices, sizeof(indices), &m_indexBuffer.resource);
			AllocateUploadBuffer(device, vertices, sizeof(vertices), &m_indexBuffer.resource);
			UINT descriptorIndexIB;
			UINT descriptorIndexVB;
			//Vertex Buffer is passed to shader along with index buffer as a descriptor range;
			{
				auto numElements = sizeof(indices) / 4;
				auto elementSize = 0;
				//SRV
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Buffer.NumElements = numElements;
				if (elementSize == 0) {
					srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
					srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
					srvDesc.Buffer.StructureByteStride = 0;
				}
				else {
					srvDesc.Format = DXGI_FORMAT_UNKNOWN;
					srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
					srvDesc.Buffer.StructureByteStride = elementSize;
				}
				UINT descriptorIndexToUse = UINT_MAX;
				D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor = &m_indexBuffer.cpuDescriptorHandle;
				auto descriptorHeapCpuBase = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
				if (descriptorIndexToUse >= m_descriptorHeap->GetDesc().NumDescriptors) {
					if (m_descriptorsAllocated < m_descriptorHeap->GetDesc().NumDescriptors) {
						descriptorIndexToUse = m_descriptorsAllocated++;
					}
					else {
						throw std::exception(
							"ERROR: m_descriptorsAllocated < m_descriptorHeap->GetDesc().NumDescriptors");
					}
				}
				*cpuDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeapCpuBase, descriptorIndexToUse,
				                                               m_descriptorSize);
				UINT descriptorIndex = descriptorIndexToUse;
				device->CreateShaderResourceView(m_indexBuffer.resource.Get(), &srvDesc,
				                                 m_indexBuffer.cpuDescriptorHandle);
				m_indexBuffer.gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
					m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), descriptorIndex, m_descriptorSize);
				descriptorIndexIB = descriptorIndex;
			}

			{
				auto numElements = ARRAYSIZE(vertices);
				auto elementSize = sizeof(vertices[0]);
				//SRV
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Buffer.NumElements = numElements;
				if (elementSize == 0) {
					srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
					srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
					srvDesc.Buffer.StructureByteStride = 0;
				}
				else {
					srvDesc.Format = DXGI_FORMAT_UNKNOWN;
					srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
					srvDesc.Buffer.StructureByteStride = elementSize;
				}
				UINT descriptorIndexToUse = UINT_MAX;
				D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor = &m_vertexBuffer.cpuDescriptorHandle;
				auto descriptorHeapCpuBase = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
				if (descriptorIndexToUse >= m_descriptorHeap->GetDesc().NumDescriptors) {
					if (m_descriptorsAllocated < m_descriptorHeap->GetDesc().NumDescriptors) {
						descriptorIndexToUse = m_descriptorsAllocated++;
					}
					else {
						throw std::exception(
							"ERROR: m_descriptorsAllocated < m_descriptorHeap->GetDesc().NumDescriptors");
					}
				}
				*cpuDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeapCpuBase, descriptorIndexToUse,
				                                               m_descriptorSize);
				UINT descriptorIndex = descriptorIndexToUse;
				device->CreateShaderResourceView(m_vertexBuffer.resource.Get(), &srvDesc,
				                                 m_vertexBuffer.cpuDescriptorHandle);
				m_vertexBuffer.gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
					m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), descriptorIndex, m_descriptorSize);
				descriptorIndexVB = descriptorIndex;
			}

			if (descriptorIndexVB != descriptorIndexIB + 1) {
				throw std::exception("Error: VertexBuffer Desc is not equal to IndexBuffer Desc");
			}
		}
	}

	//Build rayTracing acceleration structures from generated geometry
	{
		auto device = m_device->GetDevice();
		auto& commandQueue = m_device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		auto commandList = commandQueue.GetCommandList();
		auto d3dCommandList = commandList->GetCommandList();
		auto commandAllocator = commandList->GetCommandAllocator();

		//Reset the command list for acceleration structure construction.
		d3dCommandList->Reset(commandAllocator.Get(), nullptr);

		//Build bottom-level AS.
		AccelerationStructureBuffers bottomLevelAS[BottomLevelASType::Count];
		std::array<std::vector<D3D12_RAYTRACING_GEOMETRY_DESC>, BottomLevelASType::Count> geometryDescs;
		{
			//Build Geometry Descriptors for botrtom levels AS.
			{
				//Mark geometry as opaque.
				//PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing optimizations.
				//NOTE: When rays encouter opaque geometry an any hit shader will not be executable whenether it is present or not.
				D3D12_RAYTRACING_GEOMETRY_FLAGS geometryFlags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

				//Triangle geometry desc.
				{
					//Triangle bottom-level AS contains a single plane geometry;
					geometryDescs[BottomLevelASType::Triangle].resize(1);

					//Plane geometry
					auto& geometryDesc = geometryDescs[BottomLevelASType::Triangle][0];
					geometryDesc = {};
					geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
					geometryDesc.Triangles.IndexBuffer = m_indexBuffer.resource->GetGPUVirtualAddress();
					geometryDesc.Triangles.IndexCount = static_cast<UINT>(m_indexBuffer.resource->GetDesc().Width) /
						sizeof(Index);
					geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
					geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
					geometryDesc.Triangles.VertexCount = static_cast<UINT>(m_vertexBuffer.resource->GetDesc().Width) /
						sizeof(Vertex);
					geometryDesc.Triangles.VertexBuffer.StartAddress = m_vertexBuffer.resource->GetGPUVirtualAddress();
					geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);
					geometryDesc.Flags = geometryFlags;
				}

				//AABB geometry desc
				{
					D3D12_RAYTRACING_GEOMETRY_DESC aabbDescTemplate = {};
					aabbDescTemplate.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS;
					aabbDescTemplate.AABBs.AABBCount = 1;
					aabbDescTemplate.AABBs.AABBs.StrideInBytes = sizeof(D3D12_RAYTRACING_AABB);
					aabbDescTemplate.Flags = geometryFlags;

					//One AABB primitive per geometry.
					geometryDescs[BottomLevelASType::AABB].resize(IntersectionShaderType::TotalPrimitiveCount,
					                                              aabbDescTemplate);

					//Create AABB geometries.
					//Having separate geometries allows of separate shader record binding per geometry.
					for (UINT i = 0; i < IntersectionShaderType::TotalPrimitiveCount; i++) {
						auto& geometryDesc = geometryDescs[BottomLevelASType::AABB][i];
					}
				}
			}
			//Build all bottom-level AS.
			auto rtxDevice = m_device->GetRayTracingDevice();
			auto rtxCommandList = commandList->GetRayTracingCommandList();

			for (UINT i = 0; i < BottomLevelASType::Count; i++) {
				wrl::ComPtr<ID3D12Resource> scratch;
				wrl::ComPtr<ID3D12Resource> bottomLevelASRes;

				//Get size requirements for scratch and AS buffers
				D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
				D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& bottomLevelInputs = bottomLevelBuildDesc.Inputs;
				bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
				bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
				bottomLevelInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
				bottomLevelInputs.NumDescs = static_cast<UINT>(geometryDescs[i].size());
				bottomLevelInputs.pGeometryDescs = geometryDescs[i].data();

				D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPreBuildInfo = {};
				rtxDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPreBuildInfo);
				if (bottomLevelPreBuildInfo.ResultDataMaxSizeInBytes <= 0) {
					//Create scratch buffer.
					AllocateUAVBuffer(device.Get(), bottomLevelPreBuildInfo.ScratchDataSizeInBytes, &scratch,
					                  D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");
					//Allocate resources for acceleration structures.
					//Acceleration structures can only be placed in resources that are created in default heap (or custom heap equivalent).
					//Default heap is OK since the application does not need CPU read/write access to them.
					//Resources that will contain acceleration structures must be created in state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
					//And must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. ALLOW_UNORDERED_ACCESS requirement simply acknowledges both:
					// - system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
					// - pointy of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
					{
						D3D12_RESOURCE_STATES initialResourceState =
							D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
						AllocateUAVBuffer(device.Get(), bottomLevelPreBuildInfo.ResultDataMaxSizeInBytes,
						                  &bottomLevelASRes, initialResourceState, L"BottomLevelAccelerationStructure");
					}

					//Bottom-level AS desc.
					{
						bottomLevelBuildDesc.ScratchAccelerationStructureData = scratch->GetGPUVirtualAddress();
						bottomLevelBuildDesc.DestAccelerationStructureData = bottomLevelASRes->GetGPUVirtualAddress();
					}

					//Build acceleration structure
					rtxCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);

					AccelerationStructureBuffers bottomLevelASBuffers;
					bottomLevelASBuffers.accelerationStructure = bottomLevelASRes;
					bottomLevelASBuffers.scratch = scratch;
					bottomLevelASBuffers.ResultDataMaxSizeInBytes = bottomLevelPreBuildInfo.ResultDataMaxSizeInBytes;
					bottomLevelAS[i] = bottomLevelASBuffers;
				}
			}
		}

		// Batch all resource barriers for bottom-level AS builds.
		D3D12_RESOURCE_BARRIER resourceBarriers[BottomLevelASType::Count];
		for (UINT i = 0; i < BottomLevelASType::Count; i++) {
			resourceBarriers[i] = CD3DX12_RESOURCE_BARRIER::UAV(bottomLevelAS[i].accelerationStructure.Get());
		}
		d3dCommandList->ResourceBarrier(BottomLevelASType::Count, resourceBarriers);

		//Build top-level AS.
		AccelerationStructureBuffers topLevelAS;
		{
			auto rtxDevice = m_device->GetRayTracingDevice();
			auto rtxCommandList = commandList->GetRayTracingCommandList();

			wrl::ComPtr<ID3D12Resource> scratch;
			wrl::ComPtr<ID3D12Resource> topLevelASRes;

			//Get size requirements for scratch and AS buffers
			D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
			D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& topLevelInputs = topLevelBuildDesc.Inputs;
			topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
			topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
			topLevelInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
			topLevelInputs.NumDescs = NUM_BLAS;

			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPreBuildInfo = {};
			rtxDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPreBuildInfo);
			if (topLevelPreBuildInfo.ResultDataMaxSizeInBytes <= 0) {
				//Create scratch buffer.
				AllocateUAVBuffer(device.Get(), topLevelPreBuildInfo.ScratchDataSizeInBytes, &scratch,
				                  D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");
				//Allocate resources for acceleration structures.
				//Acceleration structures can only be placed in resources that are created in default heap (or custom heap equivalent).
				//Default heap is OK since the application does not need CPU read/write access to them.
				//Resources that will contain acceleration structures must be created in state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
				//And must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. ALLOW_UNORDERED_ACCESS requirement simply acknowledges both:
				// - system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
				// - pointy of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
				{
					D3D12_RESOURCE_STATES initialResourceState =
						D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
					AllocateUAVBuffer(device.Get(), topLevelPreBuildInfo.ResultDataMaxSizeInBytes,
					                  &topLevelASRes, initialResourceState, L"TopLevelAccelerationStructure");
				}

				// Create instance descs for the bottom-level acceleration structures.
				ComPtr<ID3D12Resource> instanceDescsResource;
				{
					D3D12_RAYTRACING_INSTANCE_DESC instanceDescs[BottomLevelASType::Count] = {};
					D3D12_GPU_VIRTUAL_ADDRESS bottomLevelASaddresses[BottomLevelASType::Count] = {
						bottomLevelAS[0].accelerationStructure->GetGPUVirtualAddress(),
						bottomLevelAS[1].accelerationStructure->GetGPUVirtualAddress()
					};
					BuildBottomLevelASInstanceDescs<D3D12_RAYTRACING_INSTANCE_DESC>(
						bottomLevelASaddresses, &instanceDescsResource);

					//TOP-level AS desc
					{
						topLevelBuildDesc.DestAccelerationStructureData = topLevelASRes->GetGPUVirtualAddress();
						topLevelInputs.InstanceDescs = instanceDescsResource->GetGPUVirtualAddress();
						topLevelBuildDesc.ScratchAccelerationStructureData = scratch->GetGPUVirtualAddress();
					}

					//Build acceleration structure
					rtxCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);

					AccelerationStructureBuffers topLevelASBuffers;
					topLevelASBuffers.accelerationStructure = topLevelASRes;
					topLevelASBuffers.instanceDesc = instanceDescsResource;
					topLevelASBuffers.scratch = scratch;
					topLevelASBuffers.ResultDataMaxSizeInBytes = topLevelASBuffers.ResultDataMaxSizeInBytes;
					topLevelAS = topLevelASBuffers;
				}
			}
		}

		//Kick off acceleration structure construion.
		//Wait for GPU to finish as locally created temporary GPU resource will get released once we go out of scope.
		commandQueue.ExecuteCommandList(commandList);

		//Store AS buffers. Rest of buffers will be released once we exit function.
		for (UINT i = 0; i < BottomLevelASType::Count; i++) {
			m_bottomLevelAS[i] = bottomLevelAS[i].accelerationStructure;
		}

		m_topLevelAS = topLevelAS.accelerationStructure;


		//Create constant buffers for geometry and scene
		{
			auto frameCount = m_swapChain->BufferCount;

			m_sceneCB.Create(device.Get(), frameCount, L"Scene Constant Buffer");
		}

		//Create AABB primitive attribute buffers
		{
		}

		//Build shader tables, which define shaders and their local root arguments
		{
		}

		//Create an output 2D texture to store rayTracing result to.
		{
		}
	}

	return true;
}

void CERTXPlayground::UnloadContent() {
}

void CERTXPlayground::OnUpdate(UpdateEventArgs& e) {
}

void CERTXPlayground::OnRender() {
}

void CERTXPlayground::OnKeyPressed(KeyEventArgs& e) {
}

void CERTXPlayground::OnKeyReleased(KeyEventArgs& e) {
}

void CERTXPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
}

void CERTXPlayground::OnMouseWheel(MouseWheelEventArgs& e) {
}

void CERTXPlayground::OnResize(ResizeEventArgs& e) {
}

void CERTXPlayground::OnDPIScaleChanged(DPIScaleEventArgs& e) {
}

void CERTXPlayground::OnGUI(const std::shared_ptr<GraphicsEngine::Direct3D::CECommandList>& commandList,
                            const GraphicsEngine::Direct3D::CERenderTarget& renderTarget) {
}

void CERTXPlayground::RescaleHDRRenderTarget(float scale) {
}


template <class InstanceDescType, class BLASPtrType>
void CERTXPlayground::BuildBottomLevelASInstanceDescs(BLASPtrType* bottomLevelASaddresses,
                                                      ComPtr<ID3D12Resource>* instanceDescsResource) {
	auto device = m_deviceResources->GetD3DDevice();

	vector<InstanceDescType> instanceDescs;
	instanceDescs.resize(NUM_BLAS);

	// Width of a bottom-level AS geometry.
	// Make the plane a little larger than the actual number of primitives in each dimension.
	const XMUINT3 NUM_AABB = XMUINT3(700, 1, 700);
	const XMFLOAT3 fWidth = XMFLOAT3(
		NUM_AABB.x * c_aabbWidth + (NUM_AABB.x - 1) * c_aabbDistance,
		NUM_AABB.y * c_aabbWidth + (NUM_AABB.y - 1) * c_aabbDistance,
		NUM_AABB.z * c_aabbWidth + (NUM_AABB.z - 1) * c_aabbDistance);
	const XMVECTOR vWidth = XMLoadFloat3(&fWidth);


	// Bottom-level AS with a single plane.
	{
		auto& instanceDesc = instanceDescs[BottomLevelASType::Triangle];
		instanceDesc = {};
		instanceDesc.InstanceMask = 1;
		instanceDesc.InstanceContributionToHitGroupIndex = 0;
		instanceDesc.AccelerationStructure = bottomLevelASaddresses[BottomLevelASType::Triangle];

		// Calculate transformation matrix.
		const XMVECTOR vBasePosition = vWidth * XMLoadFloat3(&XMFLOAT3(-0.35f, 0.0f, -0.35f));

		// Scale in XZ dimensions.
		XMMATRIX mScale = XMMatrixScaling(fWidth.x, fWidth.y, fWidth.z);
		XMMATRIX mTranslation = XMMatrixTranslationFromVector(vBasePosition);
		XMMATRIX mTransform = mScale * mTranslation;
		XMStoreFloat3x4(reinterpret_cast<XMFLOAT3X4*>(instanceDesc.Transform), mTransform);
	}

	// Create instanced bottom-level AS with procedural geometry AABBs.
	// Instances share all the data, except for a transform.
	{
		auto& instanceDesc = instanceDescs[BottomLevelASType::AABB];
		instanceDesc = {};
		instanceDesc.InstanceMask = 1;

		// Set hit group offset to beyond the shader records for the triangle AABB.
		instanceDesc.InstanceContributionToHitGroupIndex = BottomLevelASType::AABB * RayType::Count;
		instanceDesc.AccelerationStructure = bottomLevelASaddresses[BottomLevelASType::AABB];

		// Move all AABBS above the ground plane.
		XMMATRIX mTranslation = XMMatrixTranslationFromVector(XMLoadFloat3(&XMFLOAT3(0, c_aabbWidth / 2, 0)));
		XMStoreFloat3x4(reinterpret_cast<XMFLOAT3X4*>(instanceDesc.Transform), mTranslation);
	}
	UINT64 bufferSize = static_cast<UINT64>(instanceDescs.size() * sizeof(instanceDescs[0]));
	AllocateUploadBuffer(device, instanceDescs.data(), bufferSize, &(*instanceDescsResource), L"InstanceDescs");
};
