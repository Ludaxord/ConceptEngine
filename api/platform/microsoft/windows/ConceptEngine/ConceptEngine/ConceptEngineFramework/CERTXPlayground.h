#pragma once
#include "CED3DCamera.h"
#include "CEPlayground.h"
#include "CERenderTarget.h"


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

		private:
			/*
			 * DX12 Device
			 */
			std::shared_ptr<GraphicsEngine::Direct3D::CEDevice> m_device;
			std::shared_ptr<GraphicsEngine::Direct3D::CESwapChain> m_swapChain;
			std::shared_ptr<GraphicsEngine::Direct3D::CEGUI> m_gui;

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
			 * TODO: Move to subclasses for now try to start
			 */


			struct ProceduralPrimitiveAttributes {
				DirectX::XMFLOAT3 normal;
			};

			struct RayPayload {
				DirectX::XMFLOAT4 color;
				UINT recursionDepth;
			};

			struct ShadowRayPayload {
				bool hit;
			};

			struct SceneConstantBuffer {
				DirectX::XMMATRIX projectionToWorld;
				DirectX::XMVECTOR cameraPosition;
				DirectX::XMVECTOR lightPosition;
				DirectX::XMVECTOR lightAmbientColor;
				DirectX::XMVECTOR lightDiffuseColor;
				float reflectance;
				float elapsedTime; // Elapsed application time.
			};

			// Attributes per primitive type.
			struct PrimitiveConstantBuffer {
				DirectX::XMFLOAT4 albedo;
				float reflectanceCoef;
				float diffuseCoef;
				float specularCoef;
				float specularPower;
				float stepScale; // Step scale for ray marching of signed distance primitives. 
				// - Some object transformations don't preserve the distances and 
				//   thus require shorter steps.
				DirectX::XMFLOAT3 padding;
			};

			// Attributes per primitive instance.
			struct PrimitiveInstanceConstantBuffer {
				UINT instanceIndex;
				UINT primitiveType; // Procedural primitive type
			};

			// Dynamic attributes per primitive instance.
			struct PrimitiveInstancePerFrameBuffer {
				DirectX::XMMATRIX localSpaceToBottomLevelAS;
				// Matrix from local primitive space to bottom-level object space.
				DirectX::XMMATRIX bottomLevelASToLocalSpace;
				// Matrix from bottom-level object space to local primitive space.
			};

			struct Vertex {
				DirectX::XMFLOAT3 position;
				DirectX::XMFLOAT3 normal;
			};

		};
	}

}


// Ray types traced in this sample.
namespace RayType {
	enum Enum {
		Radiance = 0,
		// ~ Primary, reflected camera/view rays calculating color for each hit.
		Shadow,
		// ~ Shadow/visibility rays, only testing for occlusion
		Count
	};
}

namespace TraceRayParameters {
	static const UINT InstanceMask = ~0; // Everything is visible.
	namespace HitGroup {
		static const UINT Offset[RayType::Count] =
		{
			0, // Radiance ray
			1 // Shadow ray
		};
		static const UINT GeometryStride = RayType::Count;
	}

	namespace MissShader {
		static const UINT Offset[RayType::Count] =
		{
			0, // Radiance ray
			1 // Shadow ray
		};
	}
}


// From: http://blog.selfshadow.com/publications/s2015-shading-course/hoffman/s2015_pbs_physics_math_slides.pdf
static const DirectX::XMFLOAT4 ChromiumReflectance = DirectX::XMFLOAT4(0.549f, 0.556f, 0.554f, 1.0f);

static const DirectX::XMFLOAT4 BackgroundColor = DirectX::XMFLOAT4(0.8f, 0.9f, 1.0f, 1.0f);
static const float InShadowRadiance = 0.35f;

namespace AnalyticPrimitive {
	enum Enum {
		AABB = 0,
		Spheres,
		Count
	};
}

namespace VolumetricPrimitive {
	enum Enum {
		Metaballs = 0,
		Count
	};
}

namespace SignedDistancePrimitive {
	enum Enum {
		MiniSpheres = 0,
		IntersectedRoundCube,
		SquareTorus,
		TwistedTorus,
		Cog,
		Cylinder,
		FractalPyramid,
		Count
	};
}
