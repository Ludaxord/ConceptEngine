#include "CEDX12FPPCameraPlayground.h"

#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/CEDX12Manager.h"
#include "../../../../../ConceptEngineFramework/Graphics/DirectX12/Libraries/GeometryGenerator.h"

using namespace ConceptEngine::Playgrounds::DirectX12;

CEDX12FPPCameraPlayground::CEDX12FPPCameraPlayground(): CEDX12Playground() {
}

void CEDX12FPPCameraPlayground::Create() {
	CEDX12Playground::Create();

	//Reset command list to prepare for initialization commands
	m_dx12manager->ResetCommandList();

	m_camera.SetPosition(0.0f, 2.0f, -15.0f);

	LoadTextures();
	// BuildRootSignature
	{
		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0, 0);

		// Root parameter can be a table, root descriptor or root constants.
		CD3DX12_ROOT_PARAMETER slotRootParameter[4];

		// Perfomance TIP: Order from most frequent to least frequent.
		slotRootParameter[0].InitAsConstantBufferView(0);
		slotRootParameter[1].InitAsConstantBufferView(1);
		slotRootParameter[2].InitAsShaderResourceView(0, 1);
		slotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);


		auto staticSamplers = m_dx12manager->GetStaticSamplers();

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		                                        (UINT)staticSamplers.size(), staticSamplers.data(),
		                                        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		m_rootSignature = m_dx12manager->CreateRootSignature(&rootSigDesc);
	}
	BuildDescriptorHeaps();
	// BuildShaders
	{
		m_shadersMap["standardVS"] = m_dx12manager->CompileShaders("CEIndexingVertexShader.hlsl",
		                                                           nullptr,
		                                                           "VS",
		                                                           // "vs_6_3"
		                                                           "vs_5_1"
		);
		m_shadersMap["opaquePS"] = m_dx12manager->CompileShaders("CEIndexingPixelShader.hlsl",
		                                                         nullptr,
		                                                         "PS",
		                                                         // "ps_6_3"
		                                                         "ps_5_1"
		);
	}
	BuildInputLayout();
	BuildShapesGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs(m_rootSignature);

	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Close());
	m_dx12manager->ExecuteCommandLists();

	//Wait until initialization is complete
	m_dx12manager->FlushCommandQueue();
}

void CEDX12FPPCameraPlayground::Update(const CETimer& gt) {
	CEDX12Playground::Update(gt);
	m_camera.UpdateViewMatrix();

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
}

void CEDX12FPPCameraPlayground::Render(const CETimer& gt) {
	CEDX12Playground::Render(gt);

	auto mScreenViewport = m_dx12manager->GetViewPort();
	auto mScissorRect = m_dx12manager->GetScissorRect();

	auto cmdListAlloc = mCurrFrameResource->commandAllocator;

	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	ThrowIfFailed(cmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	ThrowIfFailed(m_dx12manager->GetD3D12CommandList()->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));

	m_dx12manager->GetD3D12CommandList()->RSSetViewports(1, &mScreenViewport);
	m_dx12manager->GetD3D12CommandList()->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	auto trPRT = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trPRT);

	// Clear the back buffer and depth buffer.
	m_dx12manager->GetD3D12CommandList()->ClearRenderTargetView(m_dx12manager->CurrentBackBufferView(),
	                                                            Colors::LightSteelBlue, 0, nullptr);
	m_dx12manager->GetD3D12CommandList()->ClearDepthStencilView(m_dx12manager->DepthStencilView(),
	                                                            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f,
	                                                            0, 0, nullptr);

	// Specify the buffers we are going to render to.
	auto rtCBBV = m_dx12manager->CurrentBackBufferView();
	auto rtDsv = m_dx12manager->DepthStencilView();
	m_dx12manager->GetD3D12CommandList()->OMSetRenderTargets(1, &rtCBBV, true,
	                                                         &rtDsv);

	ID3D12DescriptorHeap* descriptorHeaps[] = {m_dx12manager->GetSRVDescriptorHeap().Get()};
	m_dx12manager->GetD3D12CommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootSignature(m_rootSignature.Get());

	auto passCB = mCurrFrameResource->PassCB->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

	// Bind all the materials used in this scene.  For structured buffers, we can bypass the heap and 
	// set as a root descriptor.
	auto matBuffer = mCurrFrameResource->MaterialBuffer->Resource();
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootShaderResourceView(2, matBuffer->GetGPUVirtualAddress());

	// Bind all the textures used in this scene.  Observe
	// that we only have to specify the first descriptor in the table.  
	// The root signature knows how many descriptors are expected in the table.
	m_dx12manager->GetD3D12CommandList()->SetGraphicsRootDescriptorTable(
		3, m_dx12manager->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

	DrawRenderItems(m_dx12manager->GetD3D12CommandList().Get(), mOpaqueRitems);

	// Indicate a state transition on the resource usage.
	auto trRTP = CD3DX12_RESOURCE_BARRIER::Transition(
		m_dx12manager->CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	m_dx12manager->GetD3D12CommandList()->ResourceBarrier(1, &trRTP);

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

void CEDX12FPPCameraPlayground::Resize() {
	CEDX12Playground::Resize();
	m_camera.SetLens(0.25f * Resources::Pi, m_dx12manager->GetAspectRatio(), 1.0f, 1000.0f);
}

void CEDX12FPPCameraPlayground::OnMouseDown(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseDown(key, x, y);
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_dx12manager->GetWindowHandle());
}

void CEDX12FPPCameraPlayground::OnMouseUp(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseUp(key, x, y);
	ReleaseCapture();
}

void CEDX12FPPCameraPlayground::OnMouseMove(KeyCode key, int x, int y) {
	CEDX12Playground::OnMouseMove(key, x, y);
	if (key == KeyCode::LButton) {
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		m_camera.Pitch(dy);
		m_camera.RotateY(dx);
	}
	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void CEDX12FPPCameraPlayground::OnKeyUp(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyUp(key, keyChar, gt);
}

void CEDX12FPPCameraPlayground::OnKeyDown(KeyCode key, char keyChar, const CETimer& gt) {
	CEDX12Playground::OnKeyDown(key, keyChar, gt);
	const float dt = gt.DeltaTime();
	switch (key) {
	case KeyCode::A:
		m_camera.Strafe(-10.0f * dt);
		break;
	case KeyCode::D:
		m_camera.Strafe(10.0f * dt);
		break;
	case KeyCode::W:
		m_camera.Walk(10.0f * dt);
		break;
	case KeyCode::S:
		m_camera.Walk(-10.0f * dt);
		break;
	}
}

void CEDX12FPPCameraPlayground::OnMouseWheel(KeyCode key, float wheelDelta, int x, int y) {
	CEDX12Playground::OnMouseWheel(key, wheelDelta, x, y);
}

void CEDX12FPPCameraPlayground::UpdateObjectCBs(const CETimer& gt) {
	CEDX12Playground::UpdateObjectCBs(gt);
	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& ri : mAllRitems) {
		Resources::LitShapesRenderItem* e = static_cast<Resources::LitShapesRenderItem*>(ri.get());
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (e->NumFramesDirty > 0) {
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			XMMATRIX texTransform = XMLoadFloat4x4(&e->TexTransform);

			Resources::ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.WorldViewProjection, XMMatrixTranspose(world));
			XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));
			objConstants.MaterialIndex = e->Mat->MatCBIndex;

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			e->NumFramesDirty--;
		}
	}
}

void CEDX12FPPCameraPlayground::UpdateMainPassCB(const CETimer& gt) {
	CEDX12Playground::UpdateMainPassCB(gt);
	XMMATRIX view = m_camera.GetView();
	XMMATRIX proj = m_camera.GetProj();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	auto detView = XMMatrixDeterminant(view);
	XMMATRIX invView = XMMatrixInverse(&detView, view);

	auto detProj = XMMatrixDeterminant(proj);
	XMMATRIX invProj = XMMatrixInverse(&detProj, proj);

	auto detViewProj = XMMatrixDeterminant(viewProj);
	XMMATRIX invViewProj = XMMatrixInverse(&detViewProj, viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = m_camera.GetPosition3f();
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)m_dx12manager->GetWindowWidth(),
	                                        (float)m_dx12manager->GetWindowHeight());
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / m_dx12manager->GetWindowWidth(),
	                                           1.0f / m_dx12manager->GetWindowHeight());
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
	mMainPassCB.AmbientLight = {0.25f, 0.25f, 0.35f, 1.0f};
	mMainPassCB.Lights[0].Direction = {0.57735f, -0.57735f, 0.57735f};
	mMainPassCB.Lights[0].Strength = {0.8f, 0.8f, 0.8f};
	mMainPassCB.Lights[1].Direction = {-0.57735f, -0.57735f, 0.57735f};
	mMainPassCB.Lights[1].Strength = {0.4f, 0.4f, 0.4f};
	mMainPassCB.Lights[2].Direction = {0.0f, -0.707f, -0.707f};
	mMainPassCB.Lights[2].Strength = {0.2f, 0.2f, 0.2f};

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void CEDX12FPPCameraPlayground::UpdateMaterialCBs(const CETimer& gt) {
	CEDX12Playground::UpdateMaterialCBs(gt);
	auto currMaterialBuffer = mCurrFrameResource->MaterialBuffer.get();
	for (auto& e : mMaterials) {
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		Resources::Material* mat = e.second.get();
		if (mat->NumFramesDirty > 0) {
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			Resources::MaterialData matData;
			matData.DiffuseAlbedo = mat->DiffuseAlbedo;
			matData.FresnelR0 = mat->FresnelR0;
			matData.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;

			currMaterialBuffer->CopyData(mat->MatCBIndex, matData);

			// Next FrameResource need to be updated too.
			mat->NumFramesDirty--;
		}
	}
}

void CEDX12FPPCameraPlayground::BuildPSOs(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature) {
	auto d3dDevice = m_dx12manager->GetD3D12Device();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	/*
	 * PSO for opaque objects
	 */
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = {m_inputLayout.data(), (UINT)m_inputLayout.size()};
	opaquePsoDesc.pRootSignature = rootSignature.Get();
	opaquePsoDesc.VS = {
		reinterpret_cast<BYTE*>(m_shadersMap["standardVS"]->GetBufferPointer()),
		m_shadersMap["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS = {
		reinterpret_cast<BYTE*>(m_shadersMap["opaquePS"]->GetBufferPointer()),
		m_shadersMap["opaquePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = m_dx12manager->GetBackBufferFormat();
	opaquePsoDesc.SampleDesc.Count = m_dx12manager->GetM4XMSAAState() ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m_dx12manager->GetM4XMSAAState() ? (m_dx12manager->GetM4XMSAAQuality() - 1) : 0;
	opaquePsoDesc.DSVFormat = m_dx12manager->GetDepthStencilFormat();
	ThrowIfFailed(d3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));
}

void CEDX12FPPCameraPlayground::LoadTextures() {
	auto bricksTex = m_dx12manager->LoadTextureFromFile("bricks.dds", "bricksTex");
	auto stoneTex = m_dx12manager->LoadTextureFromFile("stone.dds", "stoneTex");
	auto tileTex = m_dx12manager->LoadTextureFromFile("tile.dds", "tileTex");
	auto crateTex = m_dx12manager->LoadTextureFromFile("WoodCrate01.dds", "crateTex");

	mTextures[bricksTex->Name] = std::move(bricksTex);
	mTextures[stoneTex->Name] = std::move(stoneTex);
	mTextures[tileTex->Name] = std::move(tileTex);
	mTextures[crateTex->Name] = std::move(crateTex);
}

void CEDX12FPPCameraPlayground::BuildDescriptorHeaps() {
	m_dx12manager->CreateSRVDescriptorHeap(4);

	auto srvSize = m_dx12manager->GetDescriptorSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
		m_dx12manager->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

	auto d3dDevice = m_dx12manager->GetD3D12Device();

	auto bricksTex = mTextures["bricksTex"]->Resource;
	auto stoneTex = mTextures["stoneTex"]->Resource;
	auto tileTex = mTextures["tileTex"]->Resource;
	auto crateTex = mTextures["crateTex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = bricksTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = bricksTex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	d3dDevice->CreateShaderResourceView(bricksTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvSize);

	srvDesc.Format = stoneTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = stoneTex->GetDesc().MipLevels;
	d3dDevice->CreateShaderResourceView(stoneTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvSize);

	srvDesc.Format = tileTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = tileTex->GetDesc().MipLevels;
	d3dDevice->CreateShaderResourceView(tileTex.Get(), &srvDesc, hDescriptor);

	// next descriptor
	hDescriptor.Offset(1, srvSize);

	srvDesc.Format = crateTex->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = crateTex->GetDesc().MipLevels;
	d3dDevice->CreateShaderResourceView(crateTex.Get(), &srvDesc, hDescriptor);
}

void CEDX12FPPCameraPlayground::BuildShapesGeometry() {
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	//
	// We are concatenating all the geometry into one big vertex/index buffer.  So
	// define the regions in the buffer each submesh covers.
	//

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

	Resources::SubMeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	Resources::SubMeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	Resources::SubMeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	Resources::SubMeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	std::vector<Resources::CENormalTextureVertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].TexCoord = box.Vertices[i].TexC;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].TexCoord = grid.Vertices[i].TexC;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].TexCoord = sphere.Vertices[i].TexC;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k) {
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].TexCoord = cylinder.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Resources::CENormalTextureVertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<Resources::MeshGeometry>();
	geo->Name = "shapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = m_dx12manager->CreateDefaultBuffer(vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = m_dx12manager->CreateDefaultBuffer(indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Resources::CENormalTextureVertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

void CEDX12FPPCameraPlayground::BuildFrameResources() {
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(std::make_unique<Resources::CEFrameResource>(m_dx12manager->GetD3D12Device().Get(),
		                                                                       1,
		                                                                       (UINT)mAllRitems.size(),
		                                                                       (UINT)mMaterials.size(),
		                                                                       0,
		                                                                       Resources::WavesNormalTextureVertex,
		                                                                       false));
	}
}

void CEDX12FPPCameraPlayground::BuildMaterials() {
	auto bricks0 = std::make_unique<Resources::Material>();
	bricks0->Name = "bricks0";
	bricks0->MatCBIndex = 0;
	bricks0->DiffuseSrvHeapIndex = 0;
	bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks0->Roughness = 0.1f;

	auto stone0 = std::make_unique<Resources::Material>();
	stone0->Name = "stone0";
	stone0->MatCBIndex = 1;
	stone0->DiffuseSrvHeapIndex = 1;
	stone0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	stone0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	stone0->Roughness = 0.3f;

	auto tile0 = std::make_unique<Resources::Material>();
	tile0->Name = "tile0";
	tile0->MatCBIndex = 2;
	tile0->DiffuseSrvHeapIndex = 2;
	tile0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	tile0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	tile0->Roughness = 0.3f;

	auto crate0 = std::make_unique<Resources::Material>();
	crate0->Name = "crate0";
	crate0->MatCBIndex = 3;
	crate0->DiffuseSrvHeapIndex = 3;
	crate0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	crate0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	crate0->Roughness = 0.2f;

	mMaterials["bricks0"] = std::move(bricks0);
	mMaterials["stone0"] = std::move(stone0);
	mMaterials["tile0"] = std::move(tile0);
	mMaterials["crate0"] = std::move(crate0);
}

void CEDX12FPPCameraPlayground::BuildRenderItems() {
	auto boxRitem = std::make_unique<Resources::LitShapesRenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 1.0f, 0.0f));
	XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	boxRitem->ObjCBIndex = 0;
	boxRitem->Mat = mMaterials["crate0"].get();
	boxRitem->Geo = mGeometries["shapeGeo"].get();
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	mAllRitems.push_back(std::move(boxRitem));

	auto gridRitem = std::make_unique<Resources::LitShapesRenderItem>();
	gridRitem->World = Resources::MatrixIdentity4X4();
	XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	gridRitem->ObjCBIndex = 1;
	gridRitem->Mat = mMaterials["tile0"].get();
	gridRitem->Geo = mGeometries["shapeGeo"].get();
	gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	mAllRitems.push_back(std::move(gridRitem));

	XMMATRIX brickTexTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	UINT objCBIndex = 2;
	for (int i = 0; i < 5; ++i) {
		auto leftCylRitem = std::make_unique<Resources::LitShapesRenderItem>();
		auto rightCylRitem = std::make_unique<Resources::LitShapesRenderItem>();
		auto leftSphereRitem = std::make_unique<Resources::LitShapesRenderItem>();
		auto rightSphereRitem = std::make_unique<Resources::LitShapesRenderItem>();

		XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
		XMStoreFloat4x4(&leftCylRitem->TexTransform, brickTexTransform);
		leftCylRitem->ObjCBIndex = objCBIndex++;
		leftCylRitem->Mat = mMaterials["bricks0"].get();
		leftCylRitem->Geo = mGeometries["shapeGeo"].get();
		leftCylRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
		XMStoreFloat4x4(&rightCylRitem->TexTransform, brickTexTransform);
		rightCylRitem->ObjCBIndex = objCBIndex++;
		rightCylRitem->Mat = mMaterials["bricks0"].get();
		rightCylRitem->Geo = mGeometries["shapeGeo"].get();
		rightCylRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
		leftSphereRitem->TexTransform = Resources::MatrixIdentity4X4();
		leftSphereRitem->ObjCBIndex = objCBIndex++;
		leftSphereRitem->Mat = mMaterials["stone0"].get();
		leftSphereRitem->Geo = mGeometries["shapeGeo"].get();
		leftSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
		rightSphereRitem->TexTransform = Resources::MatrixIdentity4X4();
		rightSphereRitem->ObjCBIndex = objCBIndex++;
		rightSphereRitem->Mat = mMaterials["stone0"].get();
		rightSphereRitem->Geo = mGeometries["shapeGeo"].get();
		rightSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
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

void CEDX12FPPCameraPlayground::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
                                                std::vector<Resources::RenderItem*>& ritems) const {
	UINT objCBByteSize = (sizeof(Resources::ObjectConstants) + 255) & ~255;

	auto objectCB = mCurrFrameResource->ObjectCB->Resource();

	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i) {
		auto ri = static_cast<Resources::LitShapesRenderItem*>(ritems[i]);

		auto vbv = ri->Geo->VertexBufferView();
		auto ibv = ri->Geo->IndexBufferView();

		cmdList->IASetVertexBuffers(0, 1, &vbv);
		cmdList->IASetIndexBuffer(&ibv);
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;

		// CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		// tex.Offset(ri->Mat->DiffuseSrvHeapIndex, mCbvSrvDescriptorSize);

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}
