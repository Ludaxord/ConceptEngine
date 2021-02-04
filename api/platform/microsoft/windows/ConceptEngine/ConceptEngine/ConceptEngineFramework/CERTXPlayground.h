#pragma once
#include "CED3DCamera.h"
#include "CEPlayground.h"
#include "CERenderTarget.h"
#include "DXSampleHelper.h"
#include "RayTracingHlslCompat.h"
#include "RaytracingSceneDefines.h"

/*
 * TODO: Above structs just to test
 */


namespace Concept {

	namespace GraphicsEngine::Direct3D {

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
		class CERTXPlayground : public CEPlayground {

		public:
			CERTXPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync = false);
			virtual ~CERTXPlayground();

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
			           const GraphicsEngine::Direct3D::CERenderTarget& renderTarget);

			void RescaleHDRRenderTarget(float scale);
			
			template <class InstanceDescType, class BLASPtrType>
			void BuildBottomLevelASInstanceDescs(BLASPtrType* bottomLevelASaddresses, ComPtr<ID3D12Resource>* instanceDescsResource);

		private:
			/*
			 * DX12 Device
			 */
			std::shared_ptr<GraphicsEngine::Direct3D::CEDevice> m_device;
			std::shared_ptr<GraphicsEngine::Direct3D::CESwapChain> m_swapChain;
			std::shared_ptr<GraphicsEngine::Direct3D::CEGUI> m_gui;

			std::shared_ptr<GraphicsEngine::Direct3D::CERootSignature> m_rayTracingGlobalSignature;
			std::shared_ptr<GraphicsEngine::Direct3D::CERootSignature> m_triangleRayTracingLocalSignature;
			std::shared_ptr<GraphicsEngine::Direct3D::CERootSignature> m_AABBRayTracingLocalSignature;

			D3D12_VIEWPORT m_viewPort;
			D3D12_RECT m_scissorRect;

			// Scale the HDR render target to a fraction of the window size.
			float m_RenderScale;

			GraphicsEngine::Direct3D::Camera::CED3DCamera m_camera;

			struct alignas(16) CameraData {
				DirectX::XMVECTOR m_initialCamPos;
				DirectX::XMVECTOR m_initialCamRot;
				float m_InitialFov;
			};

			CameraData* m_pAlignedCameraData;

			/*
			 * TODO: Move to subclasses for now try to start with ray tracing
			 */
			// Constants.
			const UINT NUM_BLAS = 2; // Triangle + AABB bottom-level AS.
			const float c_aabbWidth = 2; // AABB width.
			const float c_aabbDistance = 2; // Distance between AABBs.

			// Geometry
			D3DBuffer m_indexBuffer;
			D3DBuffer m_vertexBuffer;
			D3DBuffer m_aabbBuffer;

			ComPtr<ID3D12StateObject> m_dxrStateObject;

			// Raytracing scene
			ConstantBuffer<SceneConstantBuffer> m_sceneCB;
			StructuredBuffer<PrimitiveInstancePerFrameBuffer> m_aabbPrimitiveAttributeBuffer;
			std::vector<D3D12_RAYTRACING_AABB> m_aabbs;

			// Descriptors
			ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
			UINT m_descriptorsAllocated;
			UINT m_descriptorSize;

			// Shader tables
			static const wchar_t* c_hitGroupNames_TriangleGeometry[RayType::Count];
			static const wchar_t* c_hitGroupNames_AABBGeometry[IntersectionShaderType::Count][RayType::Count];
			static const wchar_t* c_raygenShaderName;
			static const wchar_t* c_intersectionShaderNames[IntersectionShaderType::Count];
			static const wchar_t* c_closestHitShaderNames[GeometryType::Count];
			static const wchar_t* c_missShaderNames[RayType::Count];

			ComPtr<ID3D12Resource> m_missShaderTable;
			UINT m_missShaderTableStrideInBytes;
			ComPtr<ID3D12Resource> m_hitGroupShaderTable;
			UINT m_hitGroupShaderTableStrideInBytes;
			ComPtr<ID3D12Resource> m_rayGenShaderTable;
			
			// Root constants
			PrimitiveConstantBuffer m_planeMaterialCB;
			PrimitiveConstantBuffer m_aabbMaterialCB[IntersectionShaderType::TotalPrimitiveCount];
			
			// Acceleration structure
			ComPtr<ID3D12Resource> m_bottomLevelAS[BottomLevelASType::Count];
			ComPtr<ID3D12Resource> m_topLevelAS;
		};
	}

}
