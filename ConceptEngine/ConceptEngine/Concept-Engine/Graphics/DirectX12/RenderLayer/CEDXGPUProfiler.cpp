#include "CEDXGPUProfiler.h"

#include "CEDXCommandContext.h"
#include "CEDXCommandList.h"

#include "../../../Core/Debug/CEDebug.h"

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::CEDXGPUProfiler(CEDXDevice* device):
	CEDXDeviceElement(device), CEGPUProfiler(), QueryHeap(nullptr), WriteResource(nullptr), ReadResources(),
	TimeQueries(), Frequency(0) {
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::GetTimeQuery(TimeQuery& query,
	uint32 index) const {
	if (index >= TimeQueries.Size()) {
		query.Begin = 0;
		query.End = 0;
	}
	else {
		query = TimeQueries[index];
	}
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::BeginQuery(
	ID3D12GraphicsCommandList* commandList, uint32 index) {
	Assert(index < D3D12_DEFAULT_QUERY_COUNT);
	Assert(commandList != nullptr);

	commandList->EndQuery(QueryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, (index * 2));

	uint32 queryCount = index + 1;
	if (queryCount >= TimeQueries.Size()) {
		TimeQueries.Resize(queryCount);
	}
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::EndQuery(ID3D12GraphicsCommandList* commandList,
	uint32 index) {
	Assert(commandList != nullptr);
	Assert(index < TimeQueries.Size());

	commandList->EndQuery(QueryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, (index * 2) + 1);
}

void ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::ResolveQueries(
	CEDXCommandContext& commandContext) {
	CEDXCommandListHandle commandList = commandContext.GetCommandList();
	ID3D12CommandQueue* commandQueue = commandContext.GetCommandQueue().GetQueue();
	ID3D12GraphicsCommandList* graphicsCommandList = commandList.GetGraphicsCommandList();

	Assert(commandQueue != nullptr);
	Assert(graphicsCommandList != nullptr);

	uint32 readIndex = commandContext.GetCurrentEpochValue();
	if (readIndex >= ReadResources.Size()) {
		if (!AllocateReadResources()) {
			CEDebug::DebugBreak();
			return;
		}
	}

	CEDXResource* currentReadResource = ReadResources[readIndex].Get();
	void* data = currentReadResource->Map(0, nullptr);
	if (data) {
		const uint32 sizeInBytes = TimeQueries.SizeInBytes();

		Memory::CEMemory::Memcpy(TimeQueries.Data(), data, sizeInBytes);
		currentReadResource->Unmap(0, nullptr);
	}

	graphicsCommandList->ResolveQueryData(QueryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, 0, D3D12_DEFAULT_QUERY_COUNT,
	                                      WriteResource->GetResource(), 0);

	commandList.TransitionBarrier(WriteResource->GetResource(), D3D12_RESOURCE_STATE_COPY_DEST,
	                              D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
	graphicsCommandList->CopyResource(currentReadResource->GetResource(), WriteResource->GetResource());
	commandList.TransitionBarrier(WriteResource->GetResource(), D3D12_RESOURCE_STATE_COPY_SOURCE,
	                              D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

	HRESULT result = commandQueue->GetTimestampFrequency(&Frequency);
	if (FAILED(result)) {
		CE_LOG_ERROR("[CEDXGPUProfiler]: Failed to Query Clock Calibration");
	}
}

ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler* ConceptEngine::Graphics::DirectX12::RenderLayer::
CEDXGPUProfiler::Create(CEDXDevice* device) {
	CERef<CEDXGPUProfiler> profiler = new CEDXGPUProfiler(device);

	ID3D12Device* dxDevice = device->GetDevice();

	D3D12_QUERY_HEAP_DESC queryHeap;
	Memory::CEMemory::Memzero(&queryHeap);

	queryHeap.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
	queryHeap.Count = D3D12_DEFAULT_QUERY_COUNT * 2;
	queryHeap.NodeMask = 0;

	Microsoft::WRL::ComPtr<ID3D12QueryHeap> heap;

	HRESULT result = dxDevice->CreateQueryHeap(&queryHeap, IID_PPV_ARGS(&heap));
	if (FAILED(result)) {
		CE_LOG_ERROR("[CEDXGPUProfiler]: FAILED to create Query Heap");
		return nullptr;
	}

	D3D12_RESOURCE_DESC desc;
	Memory::CEMemory::Memzero(&desc);

	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Width = D3D12_DEFAULT_QUERY_COUNT * sizeof(TimeQuery);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Alignment = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	CERef<CEDXResource> writeResource = new CEDXResource(device, desc, D3D12_HEAP_TYPE_DEFAULT);
	if (!writeResource->Create(D3D12_RESOURCE_STATE_COMMON, nullptr)) {
		return nullptr;
	}

	writeResource->SetName("Query Write Resource");

	for (uint32 i = 0; i < 3; i++) {
		if (!profiler->AllocateReadResources()) {
			return nullptr;
		}
	}

	profiler->QueryHeap = heap;
	profiler->WriteResource = writeResource;

	return profiler.ReleaseOwnership();
}

bool ConceptEngine::Graphics::DirectX12::RenderLayer::CEDXGPUProfiler::AllocateReadResources() {

	D3D12_RESOURCE_DESC Desc;
	Memory::CEMemory::Memzero(&Desc);

	Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	Desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	Desc.Format = DXGI_FORMAT_UNKNOWN;
	Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Desc.Width = D3D12_DEFAULT_QUERY_COUNT * sizeof(TimeQuery);
	Desc.Height = 1;
	Desc.DepthOrArraySize = 1;
	Desc.MipLevels = 1;
	Desc.Alignment = 0;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;

	CERef<CEDXResource> readResource = new CEDXResource(GetDevice(), Desc, D3D12_HEAP_TYPE_READBACK);
	if (readResource->Create(D3D12_RESOURCE_STATE_COPY_DEST, nullptr)) {
		readResource->SetName("Query Read Back Resource");
		ReadResources.EmplaceBack(readResource);
	}
	else {
		return false;
	}

	return true;
}
