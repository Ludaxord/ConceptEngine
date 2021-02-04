#include "CERTXPlayground.h"


#include <iomanip>
#include <sstream>
#include <spdlog/spdlog.h>


#include "CECommandQueue.h"
#include "CEDevice.h"
#include "CEGUI.h"
#include "CERootSignature.h"
#include "CESwapChain.h"
#include "d3dx12.h"
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
	spdlog::info(wstr.str().c_str());
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
	{
	}

	//Build geometry to be used in sample
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

	//Build rayTracing acceleration structures from generated geometry
	{
	}

	//Create constant buffers for geometry and scene
	{
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
