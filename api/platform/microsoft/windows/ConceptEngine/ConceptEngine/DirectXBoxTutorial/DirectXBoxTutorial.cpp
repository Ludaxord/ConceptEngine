#include "DirectXBoxTutorial.h"

#include "../DirectXFrameworkTutorial/ConceptEngineRunner.h"
#include "../DirectXFrameworkTutorial/DirectXHelper.h"

const wchar_t* DirectXBoxTutorial::c_hitGroupName = L"BoxHitGroup";
const wchar_t* DirectXBoxTutorial::c_rayGenShaderName = L"BoxRayGenShader";
const wchar_t* DirectXBoxTutorial::c_closestHitShaderName = L"BoxClosestHitShader";
const wchar_t* DirectXBoxTutorial::c_missShaderName = L"BoxMissShader";

DirectXBoxTutorial::DirectXBoxTutorial(UINT width, UINT height) : Tutorial(width, height, L"Box Tutorial"),
                                                                  m_rayTracingOutputResourceUAVDescriptorHeapIndex(
	                                                                  UINT_MAX) {
	m_rayGenCB.viewport = {-1.0f, -1.0f, 1.0f, 1.0f};
	UpdateForSizeChange(width, height);
}

void DirectXBoxTutorial::OnInit() {
	m_deviceResources = std::make_unique<DirectXResources>(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_UNKNOWN,
		FrameCount,
		D3D_FEATURE_LEVEL_11_0,
		DirectXResources::c_requireTearingSupport,
		m_adapterID
	);

	m_deviceResources->RegisterDeviceNotify(this);
	m_deviceResources->SetWindow(ConceptEngineRunner::GetHWnd(), m_width, m_height);
	m_deviceResources->InitializeDXGIAdapter();
	
	ThrowIfFalse(DirectXResources::IsDirectXRayTracingSupported(m_deviceResources->GetAdapter()),
	             L"Error: DirectX Ray Tracing is not supported in this Machine");
	
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void DirectXBoxTutorial::OnUpdate() {
}

void DirectXBoxTutorial::OnRender() {
}

void DirectXBoxTutorial::OnSizeChanged(UINT width, UINT height, bool minimized) {
}

void DirectXBoxTutorial::OnKeyDown(KeyCode key) {
}

void DirectXBoxTutorial::OnKeyUp(KeyCode key) {
}

void DirectXBoxTutorial::OnWindowMoved(int x, int y) {
}

void DirectXBoxTutorial::OnMouseMove(UINT x, UINT y) {
}

void DirectXBoxTutorial::OnMouseButtonDown(KeyCode key, UINT x, UINT y) {
}

void DirectXBoxTutorial::OnMouseButtonUp(KeyCode key, UINT x, UINT y) {
}

void DirectXBoxTutorial::OnDisplayChanged() {
}

//Create resources that depend on device
void DirectXBoxTutorial::CreateDeviceDependentResources() {
}

void DirectXBoxTutorial::CreateWindowSizeDependentResources() {
}

void DirectXBoxTutorial::ReleaseDeviceDependentResources() {
}

void DirectXBoxTutorial::ReleaseWindowSizeDependentResources() {
}

void DirectXBoxTutorial::ReCreateDirect3D() {
}

void DirectXBoxTutorial::MakeRayTracing() {
}

void DirectXBoxTutorial::CreateRayTracingInterfaces() {
}

void DirectXBoxTutorial::SerializeAndCreateRayTracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc,
                                                                   wrl::ComPtr<ID3D12RootSignature>* rootSig) {
}

void DirectXBoxTutorial::CreateRootSignatures() {
}

void DirectXBoxTutorial::CreateLocalRootSignatureSubObjects(CD3DX12_STATE_OBJECT_DESC* rayTracingPipelineState) {
}

void DirectXBoxTutorial::CreateRayTracingPipelineStateObject() {
}

void DirectXBoxTutorial::CreateDescriptorHeap() {
}

void DirectXBoxTutorial::CreateRayTracingOutputResource() {
}

void DirectXBoxTutorial::BuildGeometry() {
}

void DirectXBoxTutorial::BuildAccelerationStructures() {
}

void DirectXBoxTutorial::BuildShaderTables() {
}

void DirectXBoxTutorial::CopyRayTracingOutputToBackBuffer() {
}

void DirectXBoxTutorial::CalculateFrameStats() {
}

UINT DirectXBoxTutorial::AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse) {
	return 0;
}
