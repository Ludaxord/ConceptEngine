#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>


#include "CED3DCamera.h"
#include "CEPlayground.h"
#include "DirectXRaytracingHelper.h"
#include "DXSampleHelper.h"
#include "RaytracingSceneDefines.h"

namespace DX {
	class DeviceResources;
}


// Shader tables
static const wchar_t* c_hitGroupNames_TriangleGeometry[RayType::Count];
static const wchar_t* c_hitGroupNames_AABBGeometry[IntersectionShaderType::Count][RayType::Count];
static const wchar_t* c_raygenShaderName;
static const wchar_t* c_intersectionShaderNames[IntersectionShaderType::Count];
static const wchar_t* c_closestHitShaderNames[GeometryType::Count];
static const wchar_t* c_missShaderNames[RayType::Count];

namespace Concept {
	namespace GraphicsEngine::Direct3D {
		class CEIndexBuffer;
		class CEVertexBuffer;
		class CEStateObject;

		class CERenderTarget;
		class CECommandList;
		class CEPipelineStateObject;
		class CERootSignature;
		class CETexture;
		class CEScene;
		class CESwapChain;
		class CEGUI;
		class CEDevice;
	}

	namespace GameEngine::Playground {
		// Shader tables
		static const wchar_t* c_hitGroupNames_TriangleGeometry[RayType::Count];
		static const wchar_t* c_hitGroupNames_AABBGeometry[IntersectionShaderType::Count][RayType::Count];
		static const wchar_t* c_raygenShaderName;
		static const wchar_t* c_intersectionShaderNames[IntersectionShaderType::Count];
		static const wchar_t* c_closestHitShaderNames[GeometryType::Count];
		static const wchar_t* c_missShaderNames[RayType::Count];

		class CERayTracingPlayground : public CEPlayground {
		public:
			CERayTracingPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync = false);

			bool LoadContent() override;
			void UnloadContent() override;
		protected:
			void OnUpdate(UpdateEventArgs& e) override;
			void OnRender() override;
			void OnKeyPressed(KeyEventArgs& e) override;
			void OnKeyReleased(KeyEventArgs& e) override;
			void OnMouseMoved(MouseMotionEventArgs& e) override;
			void OnMouseWheel(MouseWheelEventArgs& e) override;
			void OnResize(ResizeEventArgs& e) override;
			void OnDPIScaleChanged(DPIScaleEventArgs& e) override;

			void OnGUI(const std::shared_ptr<GraphicsEngine::Direct3D::CECommandList>& commandList,
			           const GraphicsEngine::Direct3D::CERenderTarget& renderTarget) const;

			void InitializeScene();
			void CreateDeviceDependentResources();
			void CreateWindowSizeDependentResources();
			void UpdateCameraMatrices();

			void CreateRaytracingInterfaces();
			void CreateRootSignatures();
			void CreateRaytracingPipelineStateObject();
			void CreateDescriptorHeap();
			void BuildGeometry();
			void BuildAccelerationStructures();
			void CreateConstantBuffers();
			void CreateAABBPrimitiveAttributesBuffers();
			void BuildShaderTables();
			void CreateRaytracingOutputResource();
			void SerializeAndCreateRaytracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc,
			                                               ComPtr<ID3D12RootSignature>* rootSig);
			void CreateDxilLibrarySubobject(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);
			void CreateHitGroupSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);
			void CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);
			void BuildProceduralGeometryAABBs();
			void BuildPlaneGeometry();
			UINT CreateBufferSRV(D3DBuffer* buffer, UINT numElements, UINT elementSize);
			UINT AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse = UINT_MAX);
			void BuildGeometryDescsForBottomLevelAS(
				std::array<std::vector<D3D12_RAYTRACING_GEOMETRY_DESC>, BottomLevelASType::Count>& geometryDescs);
			template <class InstanceDescType, class BLASPtrType>
			void BuildBotomLevelASInstanceDescs(BLASPtrType* bottomLevelASaddresses,
			                                    ComPtr<ID3D12Resource>* instanceDescsResource);
			AccelerationStructureBuffers BuildBottomLevelAS(
				const std::vector<D3D12_RAYTRACING_GEOMETRY_DESC>& geometryDesc,
				D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags =
					D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE);
			AccelerationStructureBuffers BuildTopLevelAS(
				AccelerationStructureBuffers bottomLevelAS[BottomLevelASType::Count],
				D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags =
					D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE);

		private:

			// Viewport dimensions.
			UINT m_width;
			UINT m_height;
			float m_aspectRatio;

			// Window bounds
			RECT m_windowBounds;

			// Override to be able to start without Dx11on12 UI for PIX. PIX doesn't support 11 on 12. 
			bool m_enableUI;

			// D3D device resources
			UINT m_adapterIDoverride;
			std::unique_ptr<DX::DeviceResources> m_deviceResources;

			static const UINT FrameCount = 3;

			// Constants.
			const UINT NUM_BLAS = 2; // Triangle + AABB bottom-level AS.
			const float c_aabbWidth = 2; // AABB width.
			const float c_aabbDistance = 2; // Distance between AABBs.

			// DirectX Raytracing (DXR) attributes
			ComPtr<ID3D12Device5> m_dxrDevice;
			ComPtr<ID3D12GraphicsCommandList5> m_dxrCommandList;
			ComPtr<ID3D12StateObject> m_dxrStateObject;

			// Root signatures
			ComPtr<ID3D12RootSignature> m_raytracingGlobalRootSignature;
			ComPtr<ID3D12RootSignature> m_raytracingLocalRootSignature[LocalRootSignature::Type::Count];

			// Descriptors
			ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
			UINT m_descriptorsAllocated;
			UINT m_descriptorSize;

			// Raytracing scene
			ConstantBuffer<SceneConstantBuffer> m_sceneCB;
			StructuredBuffer<PrimitiveInstancePerFrameBuffer> m_aabbPrimitiveAttributeBuffer;
			std::vector<D3D12_RAYTRACING_AABB> m_aabbs;

			// Root constants
			PrimitiveConstantBuffer m_planeMaterialCB;
			PrimitiveConstantBuffer m_aabbMaterialCB[IntersectionShaderType::TotalPrimitiveCount];

			// Geometry
			D3DBuffer m_indexBuffer;
			D3DBuffer m_vertexBuffer;
			D3DBuffer m_aabbBuffer;

			// Acceleration structure
			ComPtr<ID3D12Resource> m_bottomLevelAS[BottomLevelASType::Count];
			ComPtr<ID3D12Resource> m_topLevelAS;

			// Raytracing output
			ComPtr<ID3D12Resource> m_raytracingOutput;
			D3D12_GPU_DESCRIPTOR_HANDLE m_raytracingOutputResourceUAVGpuDescriptor;
			UINT m_raytracingOutputResourceUAVDescriptorHeapIndex;


			ComPtr<ID3D12Resource> m_missShaderTable;
			UINT m_missShaderTableStrideInBytes;
			ComPtr<ID3D12Resource> m_hitGroupShaderTable;
			UINT m_hitGroupShaderTableStrideInBytes;
			ComPtr<ID3D12Resource> m_rayGenShaderTable;

			// Application state
			float m_animateGeometryTime;
			bool m_animateGeometry;
			bool m_animateCamera;
			bool m_animateLight;
			XMVECTOR m_eye;
			XMVECTOR m_at;
			XMVECTOR m_up;
		};

	}
}
