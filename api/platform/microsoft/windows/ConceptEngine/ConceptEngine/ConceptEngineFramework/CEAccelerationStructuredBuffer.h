#pragma once
#include "CEBuffer.h"

namespace ConceptFramework::GraphicsEngine::Direct3D {
	class CEDevice;

	/*
	 * Note: Similar class to CEUploadBuffer
	 * TODO: Create implementations based on DirectX Ray Tracing Sample class D3D12RaytracingProceduralGeometry
	 */
	class CEAccelerationStructuredBuffer {
	public:
	protected:
		CEAccelerationStructuredBuffer(CEDevice& device,
		                               const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& structDesc);
		CEAccelerationStructuredBuffer(CEDevice& device, wrl::ComPtr<ID3D12Resource> scratchResource,
		                               wrl::ComPtr<ID3D12Resource> bottomLevelAccelerationStructuredResource);
		virtual ~CEAccelerationStructuredBuffer() = default;

		void CreateAccelerationStructuredBufferView();
	private:
	};
}
