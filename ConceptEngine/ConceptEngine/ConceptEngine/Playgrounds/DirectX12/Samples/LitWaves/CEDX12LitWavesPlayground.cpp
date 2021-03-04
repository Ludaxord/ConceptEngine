#include "CEDX12LitWavesPlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12LitWavesPlayground::CEDX12LitWavesPlayground(): CEDX12Playground() {
}

void CEDX12LitWavesPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	//Create basic waves
	m_waves = std::make_unique<Resources::CEWaves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	//Build root signature
	CD3DX12_ROOT_PARAMETER slotRootParameter[3];

	//Root parameter can be a table, root descriptor or root constants
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstantBufferView(1);
	slotRootParameter[2].InitAsConstantBufferView(2);

	//Root signature is an array of root parameters
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(3,
	                                              slotRootParameter,
	                                              0,
	                                              nullptr,
	                                              D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	m_rootSignature = m_dx12manager->CreateRootSignature(&rootSignatureDesc);

	BuildNormalShadersAndInputLayout("CELitVertexShader.hlsl", "CELitPixelShader.hlsl");
	BuildLandGeometry();
	BuildWavesGeometryBuffers();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
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

void CEDX12LitWavesPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
	UpdateCamera(gt);

	//Cycle through te circular frame resource array
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

	AnimateMaterials(gt);
	UpdateObjectCBs(gt);
	UpdateMaterialCBs(gt);
	UpdateMainPassCB(gt);
	UpdateWaves(gt);
}

void CEDX12LitWavesPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);
	auto mScreenViewport = m_dx12manager->GetViewPort();
	auto mScissorRect = m_dx12manager->GetScissorRect();

	auto cmdListAlloc = mCurrFrameResource->commandAllocator;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Reset(cmdListAlloc.Get(),
	                                                          mPSOs[(mIsWireframe ? "opaque_wireframe" : "opaque")].
	                                                          Get()));

	m_dx12manager->GetD3D12CommandList()->RSSetViewports(1, &mScreenViewport);
	m_dx12manager->GetD3D12CommandList()->RSSetScissorRects(1, &mScissorRect);

	auto trRt = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Indicate a state transition on the resource usage.
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trRt);

	// Clear the back buffer and depth buffer.
	m_dx12manager->GetD3D12CommandList()->ClearRenderTargetView(m_dx12manager->CurrentBackBufferView(),
	                                                            Colors::LightSteelBlue, 0, nullptr);
	m_dx12manager->GetD3D12CommandList()->ClearDepthStencilView(m_dx12manager->DepthStencilView(),
	                                                            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f,
	                                                            0,
	                                                            0, nullptr);

	auto currBackBuffView = m_dx12manager->CurrentBackBufferView();
	auto currDepthStencilView = m_dx12manager->DepthStencilView();
	// Specify the buffers we are going to render to.
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(1, &currBackBuffView, true, &currDepthStencilView);

	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mRitemLayer[(int)Resources::RenderLayer::Opaque]);

	// Indicate a state transition on the resource usage.
	auto trPr = CD3DX12_RESOURCE_BARRIER::Transition(m_dx12manager->CurrentBackBuffer(),
	                                                 D3D12_RESOURCE_STATE_RENDER_TARGET,
	                                                 D3D12_RESOURCE_STATE_PRESENT);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trPr);

	// Done recording commands.
	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Close());

	m_dx12manager->ExecuteCommandLists();

	// Swap the back and front buffers
	auto swapChain = m_dx12manager->GetDXGISwapChain();
	ThrowIfFailed(swapChain->Present(0, 0));

	auto currentBackBufferIndex = m_dx12manager->GetCurrentBackBufferIndex();
	m_dx12manager->SetCurrentBackBufferIndex((currentBackBufferIndex + 1) % CEDX12Manager::GetBackBufferCount());

	m_dx12manager->FlushCommandQueue();
}

void CEDX12LitWavesPlayground::Resize() {
	CEDX12Playground::Resize();
	static const float Pi = 3.1415926535f;
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void CEDX12LitWavesPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_dx12manager->GetWindowHandle());
}

void CEDX12LitWavesPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);

	ReleaseCapture();
}

void CEDX12LitWavesPlayground::OnMouseMove(KeyCode key, int x, int y) {
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

void CEDX12LitWavesPlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyUp(key, keyChar, gt);
}

void CEDX12LitWavesPlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyDown(key, keyChar, gt);
	const float dt = gt.DeltaTime();
	switch (key) {
	case KeyCode::Left:
		mSunTheta -= 1.0f * dt;
		break;
	case KeyCode::Right:
		mSunTheta += 1.0f * dt;
		break;
	case KeyCode::Up:
		mSunPhi -= 1.0f * dt;
		break;
	case KeyCode::Down:
		mSunPhi += 1.0f * dt;
		break;
	}

	mSunPhi = m_dx12manager->Clamp(mSunPhi, 0.1f, XM_PIDIV2);
}

void CEDX12LitWavesPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12LitWavesPlayground::UpdateCamera(const CETimer& gt) {
	CEDX12Playground::UpdateCamera(gt);
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

void CEDX12LitWavesPlayground::AnimateMaterials(const CETimer& gt) {
	CEDX12Playground::AnimateMaterials(gt);
}

void CEDX12LitWavesPlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);

	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& ri : mAllRitems) {
		Resources::LitShapesRenderItem* e = static_cast<Resources::LitShapesRenderItem*>(ri.get());
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0) {
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

			Resources::CEObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.WorldViewProjection, XMMatrixTranspose(world));

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

void CEDX12LitWavesPlayground::UpdateMaterialCBs(const CETimer& gt) {
	CEDX12Playground::UpdateMaterialCBs(gt);
	auto currMaterialCB = mCurrFrameResource->MaterialCB.get();

	for (auto& e : mMaterials) {
		//Only update cbuffer data if constants have changed. If cbuffer data changes, it needs to be updated for each FrameResource
		Resources::Material* mat = e.second.get();

		if (mat->NumFramesDirty > 0) {
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			Resources::MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			//Next FrameResource need to be updated too
			mat->NumFramesDirty--;
		}
	}
}

void CEDX12LitWavesPlayground::UpdateMainPassCB(const CETimer& gt) {
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
	mMainPassCB.AmbientLight = {0.25f, 0.25f, 0.35f, 1.0f};

	XMVECTOR lightDirection = -m_dx12manager->SphericalToCartesian(1.0f, mSunTheta, mSunPhi);

	XMStoreFloat3(&mMainPassCB.Lights[0].Direction, lightDirection);
	mMainPassCB.Lights[0].Strength = { 1.0f, 1.0f, 0.9f };

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void CEDX12LitWavesPlayground::BuildLandGeometry() {
	GeometryGenerator geometry;
	GeometryGenerator::MeshData grid = geometry.CreateGrid(160.0f, 160.0f, 50, 50);

	/*
	 * Extract vertex elements we are interested and apply height function to each vertex
	 * In addition, color vertices based on their height so we have sandy looking beaches, grassy low hills, and snow mountain peaks
	 */

	std::vector<Resources::CENormalVertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); ++i) {
		auto& p = grid.Vertices[i].Position;
		vertices[i].Pos = p;
		vertices[i].Pos.y = GetHillsHeight(p.x, p.z);
		vertices[i].Normal = GetHillsNormal(p.x, p.z);
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CENormalVertex);

	std::vector<std::uint16_t> indices = grid.GetIndices16();
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "landscapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CENormalVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry subMesh;
	subMesh.IndexCount = (UINT)indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = subMesh;

	mGeometries["landscapeGeo"] = std::move(geo);
}

void CEDX12LitWavesPlayground::BuildWavesGeometryBuffers() {
	std::vector<std::uint16_t> indices(3 * m_waves->TriangleCount()); // 3 indices per face
	assert(m_waves->VertexCount() < 0x0000ffff);

	// Iterate over each quad.
	int m = m_waves->RowCount();
	int n = m_waves->ColumnCount();
	int k = 0;
	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6; // next quad
		}
	}

	UINT vbByteSize = m_waves->VertexCount() * sizeof(Resources::CENormalVertex);
	UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "waterGeo";

	//Set dynamically
	geo->VertexBufferCPU = nullptr;
	geo->VertexBufferGPU = nullptr;

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CENormalVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	Resources::SubMeshGeometry subMesh;
	subMesh.IndexCount = (UINT)indices.size();
	subMesh.StartIndexLocation = 0;
	subMesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = subMesh;
	mGeometries["waterGeo"] = std::move(geo);
}

void CEDX12LitWavesPlayground::BuildMaterials() {
	auto grass = std::make_unique<Resources::Material>();
	grass->Name = "grass";
	grass->MatCBIndex = 0;
	grass->DiffuseAlbedo = XMFLOAT4(0.2f, 0.6f, 0.2f, 1.0f);
	grass->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	grass->Roughness = 0.125f;

	auto water = std::make_unique<Resources::Material>();
	water->Name = "water";
	water->MatCBIndex = 1;
	water->DiffuseAlbedo = XMFLOAT4(0.0f, 0.2f, 0.6f, 1.0f);
	water->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	water->Roughness = 0.0f;

	mMaterials["grass"] = std::move(grass);
	mMaterials["water"] = std::move(water);
}

void CEDX12LitWavesPlayground::BuildRenderItems() {
	auto wavesRitem = std::make_unique<Resources::LitShapesRenderItem>();
	wavesRitem->World = Resources::MatrixIdentity4X4();
	wavesRitem->ObjCBIndex = 0;
	wavesRitem->Mat = mMaterials["water"].get();
	wavesRitem->Geo = mGeometries["waterGeo"].get();
	wavesRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	wavesRitem->IndexCount = wavesRitem->Geo->DrawArgs["grid"].IndexCount;
	wavesRitem->BaseVertexLocation = wavesRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mWavesRitem = wavesRitem.get();

	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(wavesRitem.get());

	auto gridRitem = std::make_unique<Resources::LitShapesRenderItem>();
	gridRitem->World = Resources::MatrixIdentity4X4();
	gridRitem->ObjCBIndex = 1;
	gridRitem->Mat = mMaterials["grass"].get();
	gridRitem->Geo = mGeometries["landscapeGeo"].get();
	gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;

	mRitemLayer[(int)Resources::RenderLayer::Opaque].push_back(gridRitem.get());

	mAllRitems.push_back(std::move(wavesRitem));
	mAllRitems.push_back(std::move(gridRitem));
}

void CEDX12LitWavesPlayground::BuildFrameResources() {
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(
			std::make_unique<Resources::CEFrameResource>(
				m_dx12manager->GetD3D12Device().Get(),
				1,
				(UINT)mAllRitems.size(),
				(UINT)mMaterials.size(),
				m_waves->VertexCount()
			)
		);
	}
}

void CEDX12LitWavesPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
                                               std::vector<Resources::RenderItem*>& ritems) const {
	UINT objCBByteSize = (sizeof(Resources::CEObjectConstants) + 255) & ~255;
	UINT matCBByteSize = (sizeof(Resources::MaterialConstants) + 255) & ~255;

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();
	auto matCB = mCurrFrameResource->MaterialCB->Resource();

	for (size_t i = 0; i < ritems.size(); ++i) {
		auto ri = static_cast<Resources::LitShapesRenderItem*>(ritems[i]);
		std::stringstream ss;
		ss << "RenderItem name: " << ri->Geo->Name << std::endl;
		OutputDebugStringA(ss.str().c_str());

		auto vertexBuffer = ri->Geo->VertexBufferView();
		auto indexBuffer = ri->Geo->IndexBufferView();
		cmdList->IASetVertexBuffers(0, 1, &vertexBuffer);
		cmdList->IASetIndexBuffer(&indexBuffer);
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);
		cmdList->SetGraphicsRootConstantBufferView(1, matCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

float CEDX12LitWavesPlayground::GetHillsHeight(float x, float z) const {
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

XMFLOAT3 CEDX12LitWavesPlayground::GetHillsNormal(float x, float z) const {
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

void CEDX12LitWavesPlayground::UpdateWaves(const CETimer& gt) {
	// Every quarter second, generate a random wave.
	static float t_base = 0.0f;
	if ((gt.TotalTime() - t_base) >= 0.25f) {
		t_base += 0.25f;

		int i = m_dx12manager->Rand(4, m_waves->RowCount() - 5);
		int j = m_dx12manager->Rand(4, m_waves->ColumnCount() - 5);

		float r = m_dx12manager->RandF(0.2f, 0.5f);

		m_waves->Disturb(i, j, r);
	}

	// Update the wave simulation.
	m_waves->Update(gt.DeltaTime());

	// Update the wave vertex buffer with the new solution.
	auto currWavesVB = mCurrFrameResource->NormalWavesVB.get();
	for (int i = 0; i < m_waves->VertexCount(); ++i) {
		Resources::CENormalVertex v;

		v.Pos = m_waves->Position(i);
		v.Normal = m_waves->Normal(i);

		currWavesVB->CopyData(i, v);
	}

	// Set the dynamic VB of the wave renderitem to the current frame VB.
	static_cast<Resources::LitShapesRenderItem*>(mWavesRitem)->Geo->VertexBufferGPU = currWavesVB->Resource();
}
