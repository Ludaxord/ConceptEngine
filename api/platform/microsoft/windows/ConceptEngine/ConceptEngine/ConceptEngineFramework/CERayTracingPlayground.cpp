#include "CERayTracingPlayground.h"


#include "CECommandList.h"
#include "CECommandQueue.h"
#include "CEDevice.h"
#include "CEDXILLibrary.h"
#include "CEExportAssociation.h"
#include "CEGUI.h"
#include "CEHitGroup.h"
#include "CERootSignature.h"
#include "CEScene.h"
#include "CESwapChain.h"
#include "CETools.h"
#include "d3dx12.h"


using namespace Concept::GameEngine::Playground;
using namespace Concept::GraphicsEngine::Direct3D;
using namespace DirectX;

// An enum for root signature parameters.
// I'm not using scoped enums to avoid the explicit cast that would be required
// to use these as root indices in the root signature.
enum RootParameters {
	Buffer,
	AccelerationStructure,
	GeometryInputs,
	GlobalConstants,
	NumRootParameters
};

enum HitGroupParameters {
	Constants,
	NumParameters
};

CERayTracingPlayground::CERayTracingPlayground(const std::wstring& name,
                                               uint32_t width, uint32_t height, bool vSync): CEPlayground(name, width,
		height, vSync),
	m_viewPort(CD3DX12_VIEWPORT(
			0.0f,
			0.0f,
			static_cast<float>(width),
			static_cast<float>(height))
	),
	m_scissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX)),
	m_RenderScale(1.0f) {

	XMVECTOR cameraPos = XMVectorSet(0, 5, -20, 1);
	auto cameraTarget = XMVectorSet(0, 5, 0, 1);
	XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

	m_camera.set_LookAt(cameraPos, cameraTarget, cameraUp);
	m_camera.set_Projection(45.0f, width / (float)height, 0.1f, 100.0f);

	m_pAlignedCameraData = (CameraData*)_aligned_malloc(sizeof(CameraData), 16);

	m_pAlignedCameraData->m_initialCamPos = m_camera.get_Translation();
	m_pAlignedCameraData->m_initialCamRot = m_camera.get_Rotation();
	m_pAlignedCameraData->m_InitialFov = m_camera.get_FoV();
}

bool CERayTracingPlayground::LoadContent() {
	/*
	 * Create devices
	 */
	m_device = CEDevice::Create();
	auto description = m_device->GetDescription();
	m_logger->info("Device Created: {}", description);
	m_swapChain = m_device->CreateSwapChain(m_window->GetWindowHandle(), DXGI_FORMAT_R8G8B8A8_UNORM);
	m_swapChain->SetVSync(m_vSync);

	m_gui = m_device->CreateGUI(m_window->GetWindowHandle(), m_swapChain->GetRenderTarget());

	CEGame::Get().WndProcHandler += CEWindowProcEvent::slot(&CEGUI::WndProcHandler, m_gui);


	auto& commandQueue = m_device->GetCommandQueue(
		D3D12_COMMAND_LIST_TYPE_DIRECT /* D3D12_COMMAND_LIST_TYPE_COPY/D3D12_COMMAND_LIST_TYPE_DIRECT */);
	auto commandList = commandQueue.GetCommandList();

	m_cube = commandList->CreateCube();

	commandList->CreateAccelerationStructures(m_cube);

	commandQueue.ExecuteCommandList(commandList);


	/**
	 * Create State Object based on passed template PipelineStateStream
	 * To create basic Ray Tracing Pipeline State
	 * Need 16 subobjects:
	* 1 for DXIL library
	* 3 for the hit-groups (triangle hit group, plane hit-group, shadow-hit group)
	* 2 for RayGen root-signature (root-signature and the subobject association)
	* 2 for triangle hit-program root-signature (root-signature and the subobject association)
	* 2 for the plane-hit root-signature (root-signature and the subobject association)
	* 2 for shadow-program and miss root-signature (root-signature and the subobject association)
	* 2 for shader config (shared between all programs. 1 for the config, 1 for association)
	* 1 for pipeline config
	* 1 for the global root signature
	 */
	{
		//Global flags for root signatures
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		WCHAR assetsPath[512];
		CETools::GetAssetsPath(assetsPath, _countof(assetsPath));
		std::wstring m_assetsPath = assetsPath;

		std::vector<D3D12_STATE_SUBOBJECT> subobjects;

		const auto* kRayGen = L"rayGen";
		const auto* kMiss = L"miss";
		const auto* kPlaneChs = L"planeChs";
		const auto* kTriangleChs = L"triangleChs";
		const auto* kShadowMiss = L"shadowMiss";
		const auto* kShadowChs = L"shadowChs";
		const auto* kTriangleHitGroup = L"triangleHitGroup";
		const auto* kShadowHitGroup = L"shadowHitGroup";
		const auto* kPlaneHitGroup = L"shadowHitGroup";

		const WCHAR* entryPoints[] = {kRayGen, kMiss, kPlaneChs, kTriangleChs, kShadowMiss, kShadowChs};

		//Create DXIL Library
		auto dxilLibrary = m_device->LoadDXILLibrary(m_assetsPath + L"CEGERayTracingVertexShader.hlsl", entryPoints);
		subobjects.push_back(dxilLibrary->operator()()); // 0 - Library Shader StateSubObject

		//TODO: Change triangle and plane shadow to Cube;

		//Create triangle HitGroup
		auto triangleHitGroup = m_device->CreateHitGroup(nullptr, kTriangleChs, kTriangleHitGroup);
		subobjects.push_back(triangleHitGroup->operator()()); // 1 Triangle hit group

		//Create shadow-ray hit group
		auto planeHitGroup = m_device->CreateHitGroup(nullptr, kPlaneChs, kPlaneHitGroup);
		subobjects.push_back(planeHitGroup->operator()()); // 2 Plane hit group

		//Create shadow-ray hit group
		auto shadowHitGroup = m_device->CreateHitGroup(nullptr, kShadowChs, kShadowHitGroup);
		subobjects.push_back(shadowHitGroup->operator()()); // 3 Shadow Hit Group

		//Create ray-gen root-signature and association
		D3D12_STATE_SUBOBJECT rayGenSubObject = {};

		//TODO: Change to CD3DX12 library, make code cleaner!
		{
			D3D12_ROOT_SIGNATURE_DESC1 desc = {};
			std::vector<D3D12_DESCRIPTOR_RANGE1> range;
			std::vector<D3D12_ROOT_PARAMETER1> rootParams;

			range.resize(2);
			// gOutput
			range[0].BaseShaderRegister = 0;
			range[0].NumDescriptors = 1;
			range[0].RegisterSpace = 0;
			range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
			range[0].OffsetInDescriptorsFromTableStart = 0;

			// gRtScene
			range[1].BaseShaderRegister = 0;
			range[1].NumDescriptors = 1;
			range[1].RegisterSpace = 0;
			range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range[1].OffsetInDescriptorsFromTableStart = 1;

			rootParams.resize(1);
			rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParams[0].DescriptorTable.NumDescriptorRanges = 2;
			rootParams[0].DescriptorTable.pDescriptorRanges = range.data();

			// Create the desc
			desc.NumParameters = 1;
			desc.pParameters = rootParams.data();
			desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

			m_rayGenRootSignature = m_device->CreateRootSignature(desc);

			rayGenSubObject.pDesc = m_rayGenRootSignature->GetD3D12RootSignature().Get();
			rayGenSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
		}

		subobjects.push_back(rayGenSubObject); // 4 Ray Gen Root Signature

		auto rayGenRootAssociation = m_device->CreateExportAssociation(&kRayGen, &rayGenSubObject);
		subobjects.push_back(rayGenRootAssociation->operator()()); // 5 Associate Root Signature to Ray Gen Shader

		// Create the tri hit root-signature and association
		D3D12_STATE_SUBOBJECT triangleHitSubObject = {};
		{
			D3D12_ROOT_SIGNATURE_DESC1 desc = {};
			std::vector<D3D12_DESCRIPTOR_RANGE1> range;
			std::vector<D3D12_ROOT_PARAMETER1> rootParams;

			range.resize(2);

			rootParams.resize(1);
			rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParams[0].Descriptor.RegisterSpace = 0;
			rootParams[0].Descriptor.ShaderRegister = 0;

			desc.NumParameters = 1;
			desc.pParameters = rootParams.data();
			desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

			m_triangleHitRootSignature = m_device->CreateRootSignature(desc);

			triangleHitSubObject.pDesc = m_triangleHitRootSignature->GetD3D12RootSignature().Get();
			triangleHitSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
		}
		subobjects.push_back(triangleHitSubObject); // 6 Triangle Hit Root Signature

		auto triangleHitAssociation = m_device->CreateExportAssociation(&kTriangleChs, &triangleHitSubObject);
		subobjects.push_back(triangleHitAssociation->operator()());
		// 7 Associate Triangle Root Sig to Triangle Hit Group

		// Create the plane hit root-signature and association
		D3D12_STATE_SUBOBJECT planeHitSubObject = {};
		{
			D3D12_ROOT_SIGNATURE_DESC1 desc = {};
			std::vector<D3D12_DESCRIPTOR_RANGE1> range;
			std::vector<D3D12_ROOT_PARAMETER1> rootParams;

			range.resize(1);
			range[0].BaseShaderRegister = 0;
			range[0].NumDescriptors = 1;
			range[0].RegisterSpace = 0;
			range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range[0].OffsetInDescriptorsFromTableStart = 0;

			rootParams.resize(1);
			rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParams[0].DescriptorTable.NumDescriptorRanges = 1;
			rootParams[0].DescriptorTable.pDescriptorRanges = range.data();

			desc.NumParameters = 1;
			desc.pParameters = rootParams.data();
			desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;

			m_planeHitRootSignature = m_device->CreateRootSignature(desc);

			planeHitSubObject.pDesc = m_planeHitRootSignature->GetD3D12RootSignature().Get();
			planeHitSubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
		}
		subobjects.push_back(planeHitSubObject); // 8 Plane Hit Root Sig

		auto planeHitAssociation = m_device->CreateExportAssociation(&kPlaneHitGroup, &planeHitSubObject);
		subobjects.push_back(planeHitAssociation->operator()()); // 9 Associate Plane Hit Root Sig to Plane Hit Group

		// Create the empty root-signature and associate it with the primary miss-shader and the shadow programs
		D3D12_STATE_SUBOBJECT emptySubObject = {};
		D3D12_ROOT_SIGNATURE_DESC1 emptyDesc = {};
		emptyDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
		m_planeHitRootSignature = m_device->CreateRootSignature(emptyDesc);

		emptySubObject.pDesc = m_planeHitRootSignature->GetD3D12RootSignature().Get();
		emptySubObject.Type = D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE;
		subobjects.push_back(planeHitAssociation->operator()()); // 10 Empty Root Sig for Plane Hit Group and Miss

		const WCHAR* emptyRootExport[] = {kMiss, kShadowChs, kShadowMiss};
		auto emptyRootAssociation = m_device->CreateExportAssociation(emptyRootExport, &emptySubObject);
		subobjects.push_back(emptyRootAssociation->operator()());
		// 11 Associate empty root sig to Plane Hit Group and Miss shader

		// TODO:
		// Bind the payload size to all programs

		//TODO:
		// Create the pipeline config

		//TODO:
		// Create the global root signature and store the empty signature

		//TODO: After implementing above objects create pipeline state object
		// Create the state
		// m_rtPipelineState = m_device->CreateStateObject(subobjects);
		
	}

	commandQueue.Flush();

	return true;
}

void CERayTracingPlayground::UnloadContent() {
	m_gui.reset();
	m_swapChain.reset();
	m_device.reset();
}

void CERayTracingPlayground::OnUpdate(UpdateEventArgs& e) {
}

void CERayTracingPlayground::OnRender() {
}

void CERayTracingPlayground::OnKeyPressed(KeyEventArgs& e) {
}

void CERayTracingPlayground::OnKeyReleased(KeyEventArgs& e) {
}

void CERayTracingPlayground::OnMouseMoved(MouseMotionEventArgs& e) {
}

void CERayTracingPlayground::OnMouseWheel(MouseWheelEventArgs& e) {
}

void CERayTracingPlayground::OnResize(ResizeEventArgs& e) {
}

void CERayTracingPlayground::OnDPIScaleChanged(DPIScaleEventArgs& e) {
}

void CERayTracingPlayground::OnGUI(const std::shared_ptr<CECommandList>& commandList,
                                   const CERenderTarget& renderTarget) const {
	m_gui->NewFrame();

	static bool showDemoWindow = false;
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	m_gui->Render(commandList, renderTarget);
}
