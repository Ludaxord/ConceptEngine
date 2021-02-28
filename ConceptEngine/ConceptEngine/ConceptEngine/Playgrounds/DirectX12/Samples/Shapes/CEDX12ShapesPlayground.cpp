#include "CEDX12ShapesPlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Resources/CEFrameResource.h"
#include "../../../../../ConceptEngineFramework/Tools/CEUtils.h"

using namespace ConceptEngine::Playgrounds::DirectX12;
using namespace ConceptEngineFramework::Game;

CEDX12ShapesPlayground::CEDX12ShapesPlayground(): CEDX12Playground() {
}

void CEDX12ShapesPlayground::Create() {
	CEDX12Playground::Create();

	m_dx12manager->ResetCommandList();

	//Build root signature
	CD3DX12_DESCRIPTOR_RANGE cbvTable0;
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE cbvTable1;
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	// Create root CBVs.
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(2,
	                                              slotRootParameter,
	                                              0,
	                                              nullptr,
	                                              D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	//Create root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	m_rootSignature = m_dx12manager->CreateRootSignature(&rootSignatureDesc);

	//Build shaders and input layout
	BuildShadersAndInputLayout();

	//build geometry
	BuildShapesGeometry();

	//build render items
	BuildRenderItems();

	//build frame resources
	BuildFrameResources();

	//build descriptor heaps
	BuildDescriptorHeaps();

	//build constant buffer views
	BuildConstantBufferViews();

	//build pipeline state object
	BuildPSOs(m_rootSignature);

	//execute command list
	auto commandQueue = m_dx12manager->GetD3D12CommandQueue();
	auto commandList = m_dx12manager->GetD3D12CommandList();

	ThrowIfFailed(commandList->Close());
	ID3D12CommandList* commandLists[] = {commandList.Get()};
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
}

void CEDX12ShapesPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
	UpdateCamera(gt);

	//Cycle through circular frame resource array.
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	auto fence = m_dx12manager->GetD3D12Fence();
	//Has GPU finished processing commands of current frame resource
	//If not, wait until GPU has completed commands up to this fence point
	if (mCurrFrameResource->Fence != 0 && fence->GetCompletedValue() < mCurrFrameResource->
		Fence) {
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ThrowIfFailed(fence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateObjectCBs(gt);
	UpdateMainPassCB(gt);
}

void CEDX12ShapesPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);

	auto mCommandList = m_dx12manager->GetD3D12CommandList();

	auto mScreenViewport = m_dx12manager->GetViewPort();
	auto mScissorRect = m_dx12manager->GetScissorRect();

	auto cmdListAlloc = mCurrFrameResource->commandAllocator;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	if (mIsWireframe) {
		ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque_wireframe"].Get()));
	}
	else {
		ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));
	}

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	auto trRt = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &trRt);

	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(m_dx12manager->CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(m_dx12manager->DepthStencilView(),
	                                    D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0,
	                                    0, nullptr);

	auto currBackBuffView = m_dx12manager->CurrentBackBufferView();
	auto currDepthStencilView = m_dx12manager->DepthStencilView();
	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &currBackBuffView, true, &currDepthStencilView);

	ID3D12DescriptorHeap* descriptorHeaps[] = {m_dx12manager->GetCBVDescriptorHeap().Get()};
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mCommandList->SetGraphicsRootSignature(m_rootSignature.Get());

	int passCbvIndex = mPassCbvOffset + mCurrFrameResourceIndex;
	auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		m_dx12manager->GetCBVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	passCbvHandle.Offset(passCbvIndex, m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	mCommandList->SetGraphicsRootDescriptorTable(1, passCbvHandle);

	DrawRenderItems(mCommandList.Get(), mOpaqueRitems);

	// Indicate a state transition on the resource usage.
	auto trPr = CD3DX12_RESOURCE_BARRIER::Transition(m_dx12manager->CurrentBackBuffer(),
	                                                 D3D12_RESOURCE_STATE_RENDER_TARGET,
	                                                 D3D12_RESOURCE_STATE_PRESENT);
	mCommandList->ResourceBarrier(1, &trPr);

	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};

	auto commandQueue = m_dx12manager->GetD3D12CommandQueue();
	commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Swap the back and front buffers
	auto swapChain = m_dx12manager->GetDXGISwapChain();
	ThrowIfFailed(swapChain->Present(0, 0));

	auto currentBackBufferIndex = m_dx12manager->GetCurrentBackBufferIndex();
	m_dx12manager->SetCurrentBackBufferIndex((currentBackBufferIndex + 1) % CEDX12Manager::GetBackBufferCount());

	m_dx12manager->FlushCommandQueue();
}

void CEDX12ShapesPlayground::Resize() {
	CEDX12Playground::Resize();

	static const float Pi = 3.1415926535f;
	//Window resizes so update aspect ration and recompute projection matrix
	XMMATRIX p = XMMatrixPerspectiveFovLH(0.25f * Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, p);
}

void CEDX12ShapesPlayground::UpdateCamera(const CETimer& gt) {
	CEDX12Playground::UpdateCamera(gt);
	// Convert Spherical to Cartesian coordinates.
	mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
	mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
	mEyePos.y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

void CEDX12ShapesPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_dx12manager->GetWindowHandle());
}

void CEDX12ShapesPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
	ReleaseCapture();
}

void CEDX12ShapesPlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
	static const float Pi = 3.1415926535f;
	if (key == KeyCode::LButton) {
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = m_dx12manager->Clamp(mPhi, 0.1f, Pi - 0.1f);
	}
	else if (key == KeyCode::RButton) {
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = 0.2f * static_cast<float>(x - mLastMousePos.x);
		float dy = 0.2f * static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = m_dx12manager->Clamp(mRadius, 5.0f, 150.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void CEDX12ShapesPlayground::OnKeyUp(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyUp(key, keyChar);
}

void CEDX12ShapesPlayground::OnKeyDown(KeyCode key, char keyChar) {
	CEDX12Playground::OnKeyDown(key, keyChar);
}

void CEDX12ShapesPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12ShapesPlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);
	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& ri : mAllRitems) {
		Resources::ShapesRenderItem* e = static_cast<Resources::ShapesRenderItem*>(ri.get());

		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0) {
			XMMATRIX world = XMLoadFloat4x4(&e->World);

			Resources::CEObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.WorldViewProjection, XMMatrixTranspose(world));

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

void CEDX12ShapesPlayground::UpdateMainPassCB(const CETimer& gt) {
	CEDX12Playground::UpdateMainPassCB(gt);
	auto width = m_dx12manager->GetWindowWidth();
	auto height = m_dx12manager->GetWindowHeight();

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	auto determinantView = XMMatrixDeterminant(view);
	XMMATRIX invView = XMMatrixInverse(&determinantView, view);

	auto determinantProj = XMMatrixDeterminant(proj);
	XMMATRIX invProj = XMMatrixInverse(&determinantProj, proj);

	auto determinantViewProj = XMMatrixDeterminant(viewProj);
	XMMATRIX invViewProj = XMMatrixInverse(&determinantViewProj, viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = mEyePos;
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)width, (float)height);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / width, 1.0f / height);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void CEDX12ShapesPlayground::BuildShapesGeometry() {
	GeometryGenerator geometryGen;
	GeometryGenerator::MeshData box = geometryGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
	GeometryGenerator::MeshData grid = geometryGen.CreateGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geometryGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geometryGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	/*
	 * We are concatenating all geometry into one big vertex index buffer. So define regions in buffer each submesh covers
	 */

	//Cache vertex offsets to each object in concatenated vertex buffers
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

	//Cache starting index for each object in concatenated index buffers
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

	/*
	 * Define SubMeshGeometry that cover different regions of vertex/index buffers
	 */

	Resources::SubMeshGeometry boxSubMesh;
	boxSubMesh.IndexCount = (UINT)box.Indices32.size();
	boxSubMesh.StartIndexLocation = boxIndexOffset;
	boxSubMesh.BaseVertexLocation = boxVertexOffset;

	Resources::SubMeshGeometry gridSubMesh;
	gridSubMesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubMesh.StartIndexLocation = gridIndexOffset;
	gridSubMesh.BaseVertexLocation = gridVertexOffset;

	Resources::SubMeshGeometry sphereSubMesh;
	sphereSubMesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubMesh.StartIndexLocation = sphereIndexOffset;
	sphereSubMesh.BaseVertexLocation = sphereVertexOffset;

	Resources::SubMeshGeometry cylinderSubMesh;
	cylinderSubMesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubMesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubMesh.BaseVertexLocation = cylinderVertexOffset;

	/*
	 * Extract vertex elements we are interested in and pack vertices of all meshes into one vertex buffer
	 */

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	std::vector<Resources::CEVertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(Colors::DarkGreen);
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(Colors::AliceBlue);
	}
	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(Colors::Crimson);
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(Colors::SteelBlue);
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CEVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "shapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CEVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubMesh;
	geo->DrawArgs["grid"] = gridSubMesh;
	geo->DrawArgs["sphere"] = sphereSubMesh;
	geo->DrawArgs["cylinder"] = cylinderSubMesh;

	mGeometries[geo->Name] = std::move(geo);
}

void CEDX12ShapesPlayground::BuildRenderItems() {
	auto boxRitem = std::make_unique<Resources::ShapesRenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRitem->ObjCBIndex = 0;
	boxRitem->Geo = mGeometries["shapeGeo"].get();
	boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	mAllRitems.push_back(std::move(boxRitem));

	auto gridRitem = std::make_unique<Resources::ShapesRenderItem>();
	gridRitem->World = Resources::MatrixIdentity4X4();
	gridRitem->ObjCBIndex = 1;
	gridRitem->Geo = mGeometries["shapeGeo"].get();
	gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	mAllRitems.push_back(std::move(gridRitem));

	UINT objCBIndex = 2;
	for (int i = 0; i < 5; ++i) {
		auto leftCylRitem = std::make_unique<Resources::ShapesRenderItem>();
		auto rightCylRitem = std::make_unique<Resources::ShapesRenderItem>();
		auto leftSphereRitem = std::make_unique<Resources::ShapesRenderItem>();
		auto rightSphereRitem = std::make_unique<Resources::ShapesRenderItem>();

		XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
		leftCylRitem->ObjCBIndex = objCBIndex++;
		leftCylRitem->Geo = mGeometries["shapeGeo"].get();
		leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
		rightCylRitem->ObjCBIndex = objCBIndex++;
		rightCylRitem->Geo = mGeometries["shapeGeo"].get();
		rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
		leftSphereRitem->ObjCBIndex = objCBIndex++;
		leftSphereRitem->Geo = mGeometries["shapeGeo"].get();
		leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
		rightSphereRitem->ObjCBIndex = objCBIndex++;
		rightSphereRitem->Geo = mGeometries["shapeGeo"].get();
		rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		mAllRitems.push_back(std::move(leftCylRitem));
		mAllRitems.push_back(std::move(rightCylRitem));
		mAllRitems.push_back(std::move(leftSphereRitem));
		mAllRitems.push_back(std::move(rightSphereRitem));
	}

	// All the render items are opaque.
	for (auto& e : mAllRitems)
		mOpaqueRitems.push_back(e.get());
}

void CEDX12ShapesPlayground::BuildFrameResources() {
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(
			std::make_unique<Resources::CEFrameResource>(
				m_dx12manager->GetD3D12Device().Get(),
				1,
				(UINT)mAllRitems.size()
			)
		);
	}
}

void CEDX12ShapesPlayground::BuildDescriptorHeaps() {
	UINT objCount = (UINT)mOpaqueRitems.size();

	//Need a CBV descriptor for each object for each frame resource, +1 for perPass CBV for each frame resource
	UINT numDescriptors = (objCount + 1) * gNumFrameResources;

	//Save an offset to start of pass CBVs. These are last 3 descriptors
	mPassCbvOffset = objCount * gNumFrameResources;

	m_dx12manager->CreateCSVDescriptorHeap(numDescriptors);
}

void CEDX12ShapesPlayground::BuildConstantBufferViews() {
	UINT objCBByteSize = (sizeof(Resources::CEObjectConstants) + 255) & ~255;

	UINT objCount = (UINT)mOpaqueRitems.size();

	//Need a CBV descriptor for each object for each frame resource
	for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex) {
		auto objectCB = mFrameResources[frameIndex]->ObjectCB->Resource();
		for (UINT i = 0; i < objCount; ++i) {
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();

			//Offset to ith object constant buffer in buffer
			cbAddress += i * objCBByteSize;

			//Offset to object cbv in descriptor heap
			auto cpuDescriptorHandleForHeap = m_dx12manager->GetCBVDescriptorHeap()->
			                                                 GetCPUDescriptorHandleForHeapStart();
			int heapIndex = frameIndex * objCount + 1;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuDescriptorHandleForHeap);
			handle.Offset(heapIndex, m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

			m_dx12manager->CreateConstantBuffers(cbAddress, objCBByteSize, &handle);
		}
	}

	UINT passCBByteSize = (sizeof(Resources::PassConstants) + 255) & ~255;

	//Last three descriptors are pass CBVs for each frame resource
	for (int frameIndex = 0; frameIndex < gNumFrameResources; ++frameIndex) {
		auto passCB = mFrameResources[frameIndex]->PassCB->Resource();

		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

		//Offset to pass cbv in descriptor heap
		auto cpuDescriptorHandleForHeap = m_dx12manager->GetCBVDescriptorHeap()->
		                                                 GetCPUDescriptorHandleForHeapStart();
		int heapIndex = mPassCbvOffset + frameIndex;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuDescriptorHandleForHeap);
		handle.Offset(heapIndex, m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

		m_dx12manager->CreateConstantBuffers(cbAddress, passCBByteSize, &handle);
	}
}

void CEDX12ShapesPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
                                             std::vector<Resources::RenderItem*>& ritems) const {

	//For each render item...
	for (size_t i = 0; i < ritems.size(); ++i) {
		auto ri = static_cast<Resources::ShapesRenderItem*>(ritems[i]);

		auto riVertexBufferView = ri->Geo->VertexBufferView();
		auto riIndexBufferView = ri->Geo->IndexBufferView();

		cmdList->IASetVertexBuffers(0, 1, &riVertexBufferView);
		cmdList->IASetIndexBuffer(&riIndexBufferView);
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		auto constantBuffer = m_dx12manager->GetCBVDescriptorHeap();
		auto descSize = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Offset to the CBV in the descriptor heap for this object and for this frame resource.
		UINT cbvIndex = mCurrFrameResourceIndex * (UINT)mOpaqueRitems.size() + ri->ObjCBIndex;
		auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(constantBuffer->GetGPUDescriptorHandleForHeapStart());
		cbvHandle.Offset(cbvIndex, descSize);

		cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);
		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}
