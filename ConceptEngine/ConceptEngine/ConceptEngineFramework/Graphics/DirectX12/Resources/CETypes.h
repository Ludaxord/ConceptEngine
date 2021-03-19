#pragma once
#include <d3d12.h>
#include <d3dcommon.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <unordered_map>
#include <wrl.h>

using namespace DirectX;

extern const int gNumFrameResources;

extern const UINT CubeMapSize;

#define MaxLights 16

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {

	static const float Pi = 3.1415926535f;
	const float Infinity = FLT_MAX;

	static XMFLOAT4X4 MatrixIdentity4X4() {
		return XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
		                  0.0f, 1.0f, 0.0f, 0.0f,
		                  0.0f, 0.0f, 1.0f, 0.0f,
		                  0.0f, 0.0f, 0.0f, 1.0f);
	}

	static UINT CalcConstantBufferByteSize(UINT byteSize) {
		// Constant buffers must be a multiple of the minimum hardware
		// allocation size (usually 256 bytes).  So round up to nearest
		// multiple of 256.  We do this by adding 255 and then masking off
		// the lower 2 bytes which store all bits < 256.
		// Example: Suppose byteSize = 300.
		// (300 + 255) & ~255
		// 555 & ~255
		// 0x022B & ~0x00ff
		// 0x022B & 0xff00
		// 0x0200
		// 512
		return (byteSize + 255) & ~255;
	}

	struct CEVertex {
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	struct CENormalVertex {
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
	};

	struct CENormalTextureVertex {
		CENormalTextureVertex() = default;

		CENormalTextureVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) :
			Pos(x, y, z),
			Normal(nx, ny, nz),
			TexCoord(u, v) {
		}

		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 TexCoord;
	};

	struct CENormalTextureTangentVertex {
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 TexCoord;
		XMFLOAT3 TangentU;
	};

	struct CENode {
		std::vector<CENormalVertex> vertices;
		std::vector<std::uint64_t> indices;
	};

	struct CENode32 {
		std::vector<CENormalVertex> vertices;
		std::vector<std::int32_t> indices;
	};

	struct InstanceData {
		DirectX::XMFLOAT4X4 World = MatrixIdentity4X4();
		DirectX::XMFLOAT4X4 TexTransform = MatrixIdentity4X4();
		UINT MaterialIndex;
		UINT InstancePad0;
		UINT InstancePad1;
		UINT InstancePad2;
	};

	struct ObjectConstants {
		XMFLOAT4X4 WorldViewProjection = MatrixIdentity4X4();
		XMFLOAT4X4 TexTransform = MatrixIdentity4X4();
		XMFLOAT2 DisplacementMapTexelSize = {1.0f, 1.0f};
		float GridSpatialStep = 1.0f;
		UINT MaterialIndex;
		UINT ObjPad0;
		UINT ObjPad1;
		UINT ObjPad2;
	};

	struct StructuredObjectConstants {
		XMFLOAT4X4 WorldViewProjection = MatrixIdentity4X4();
		XMFLOAT4X4 TexTransform = MatrixIdentity4X4();
		UINT MaterialIndex;
		UINT ObjPad0;
		UINT ObjPad1;
		UINT ObjPad2;
	};

	//Defines subrange of geometry in MeshGeometry.
	//This is for when multiple geometries are stored in one vertex and index buffers.
	//It provides offsets and data needed to draw subset of geometry stores in vertex and index buffers.
	struct SubMeshGeometry {
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		UINT BaseVertexLocation = 0;

		//BoundingBox of geometry defined by submesh.
		DirectX::BoundingBox Bounds;
	};

	struct MeshGeometry {
		//Set name to find Mesh Geometry by name
		std::string Name;

		//System memory copies. Use Blobs because vertex/index format can be generic
		//It is up to client to cast appropriately
		Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

		//Buffer data;
		UINT VertexByteStride = 0;
		UINT VertexBufferByteSize = 0;
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		UINT IndexBufferByteSize = 0;

		//MeshGeometry may store multiple geometries in one vertex/index buffer/
		//Use container to define Submesh geometries so we can draw
		//Submeshes individually
		std::unordered_map<std::string, SubMeshGeometry> DrawArgs;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const {
			D3D12_VERTEX_BUFFER_VIEW vbv;
			vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
			vbv.StrideInBytes = VertexByteStride;
			vbv.SizeInBytes = VertexBufferByteSize;

			return vbv;
		}

		D3D12_INDEX_BUFFER_VIEW IndexBufferView() const {
			D3D12_INDEX_BUFFER_VIEW ibv;
			ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
			ibv.Format = IndexFormat;
			ibv.SizeInBytes = IndexBufferByteSize;

			return ibv;
		}

		// We can free memory after we finish upload to GPU;
		void DisposeUploaders() {
			VertexBufferUploader = nullptr;
			IndexBufferUploader = nullptr;
		}
	};

	struct Light {
		XMFLOAT3 Strength = {0.5f, 0.5f, 0.5f};
		float FalloffStart = 1.0f; // point/spot light only
		XMFLOAT3 Direction = {0.0f, -1.0f, 0.0f}; // directional/spot light only
		float FalloffEnd = 10.0f; // point/spot light only
		XMFLOAT3 Position = {0.0f, 0.0f, 0.0f}; // point/spot light only
		float SpotPower = 64.0f; // spot light only
	};

	struct MaterialData {
		DirectX::XMFLOAT4 DiffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
		DirectX::XMFLOAT3 FresnelR0 = {0.01f, 0.01f, 0.01f};
		// float Roughness = 64.0f;
		float Roughness = 0.5f;

		// Used in texture mapping.
		DirectX::XMFLOAT4X4 MatTransform = MatrixIdentity4X4();

		UINT DiffuseMapIndex = 0;
		UINT MaterialPad0;
		UINT MaterialPad1;
		UINT MaterialPad2;
	};

	struct MaterialIndexData {
		DirectX::XMFLOAT4 DiffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
		DirectX::XMFLOAT3 FresnelR0 = {0.01f, 0.01f, 0.01f};
		// float Roughness = 64.0f;
		float Roughness = 0.5f;

		// Used in texture mapping.
		DirectX::XMFLOAT4X4 MatTransform = MatrixIdentity4X4();

		UINT DiffuseMapIndex = 0;
		UINT NormalMapIndex = 0;
		UINT MaterialPad1;
		UINT MaterialPad2;
	};

	struct MaterialConstants {
		XMFLOAT4 DiffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
		XMFLOAT3 FresnelR0 = {0.01f, 0.01f, 0.01f};
		float Roughness = 0.25f;

		// Used in texture mapping.
		XMFLOAT4X4 MatTransform = MatrixIdentity4X4();

		UINT DiffuseMapIndex = 0;
		UINT MaterialPad0;
		UINT MaterialPad1;
		UINT MaterialPad2;
	};

	struct Material {
		// Unique material name for lookup.
		std::string Name;

		// Index into constant buffer corresponding to this material.
		int MatCBIndex = -1;

		// Index into SRV heap for diffuse texture.
		int DiffuseSrvHeapIndex = -1;

		// Index into SRV heap for normal texture.
		int NormalSrvHeapIndex = -1;

		// Dirty flag indicating the material has changed and we need to update the constant buffer.
		// Because we have a material constant buffer for each FrameResource, we have to apply the
		// update to each FrameResource.  Thus, when we modify a material we should set 
		// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
		int NumFramesDirty = gNumFrameResources;

		// Material constant buffer data used for shading.
		XMFLOAT4 DiffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
		XMFLOAT3 FresnelR0 = {0.01f, 0.01f, 0.01f};
		float Roughness = .25f;
		XMFLOAT4X4 MatTransform = MatrixIdentity4X4();
	};

	struct Texture {
		// Unique material name for lookup.
		std::string Name;

		std::wstring Filename;

		Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
	};

	struct PassConstants {
		XMFLOAT4X4 View = MatrixIdentity4X4();
		XMFLOAT4X4 InvView = MatrixIdentity4X4();
		XMFLOAT4X4 Proj = MatrixIdentity4X4();
		XMFLOAT4X4 InvProj = MatrixIdentity4X4();
		XMFLOAT4X4 ViewProj = MatrixIdentity4X4();
		XMFLOAT4X4 InvViewProj = MatrixIdentity4X4();
		XMFLOAT3 EyePosW = {0.0f, 0.0f, 0.0f};
		float cbPerObjectPad1 = 0.0f;
		XMFLOAT2 RenderTargetSize = {0.0f, 0.0f};
		XMFLOAT2 InvRenderTargetSize = {0.0f, 0.0f};
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;

		XMFLOAT4 AmbientLight = {0.0f, 0.0f, 0.0f, 0.0f};

		//NOTE: Might not work with previous Shader files
		XMFLOAT4 FogColor = {0.7f, 0.7f, 0.7f, 1.0f};
		float gFogStart = 5.0f;
		float gFogRange = 150.0f;
		XMFLOAT2 cbPerObjectPad2;
		//NOTE END

		Light Lights[MaxLights];
	};

	struct PassStructuredConstants {
		XMFLOAT4X4 View = MatrixIdentity4X4();
		XMFLOAT4X4 InvView = MatrixIdentity4X4();
		XMFLOAT4X4 Proj = MatrixIdentity4X4();
		XMFLOAT4X4 InvProj = MatrixIdentity4X4();
		XMFLOAT4X4 ViewProj = MatrixIdentity4X4();
		XMFLOAT4X4 InvViewProj = MatrixIdentity4X4();
		XMFLOAT3 EyePosW = {0.0f, 0.0f, 0.0f};
		float cbPerObjectPad1 = 0.0f;
		XMFLOAT2 RenderTargetSize = {0.0f, 0.0f};
		XMFLOAT2 InvRenderTargetSize = {0.0f, 0.0f};
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;

		XMFLOAT4 AmbientLight = {0.0f, 0.0f, 0.0f, 0.0f};

		Light Lights[MaxLights];
	};

	struct RenderItem {
		RenderItem() = default;
	};

	enum class RenderLayer : int {
		Opaque = 0,
		Mirrors,
		Reflected,
		Transparent,
		Alpha,
		AlphaSprites,
		Shadow,
		GpuWaves,
		Highlight,
		OpaqueDynamicReflectors,
		Debug,
		Sky,
		Count
	};

	//Lightweight structure stores parameters to draw shape.
	struct LandscapeRenderItem : RenderItem {
		LandscapeRenderItem() = default;

		//World matrix of shape that describes object's local space.
		//Relative to world space, which defines position, orientation and scale of object in world
		XMFLOAT4X4 World = MatrixIdentity4X4();

		//Dirty flag indicating object data has changed and we need to update constant buffer
		//Because we have an object cbuffer for each resource we have to apply update to each resource
		//Thus, when we modify object data we should set NumFramesDirty = gNumFrameResources, so that each frame resource gets update.
		int NumFramesDirty = gNumFrameResources;

		//Index into GPU constant buffer corresponding to ObjectCB for render item
		UINT ObjCBIndex = -1;

		MeshGeometry* Geo = nullptr;

		//Primitive Topology
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		//DrawIndexedInstanced parameters
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};

	struct ShapesRenderItem : RenderItem {
		ShapesRenderItem() = default;

		//World matrix of shape that describes object's local space.
		//Relative to world space, which defines position, orientation and scale of object in world
		XMFLOAT4X4 World = MatrixIdentity4X4();

		//Dirty flag indicating object data has changed and we need to update constant buffer
		//Because we have an object cbuffer for each resource we have to apply update to each resource
		//Thus, when we modify object data we should set NumFramesDirty = gNumFrameResources, so that each frame resource gets update.
		int NumFramesDirty = gNumFrameResources;

		//Index into GPU constant buffer corresponding to ObjectCB for render item
		UINT ObjCBIndex = -1;

		MeshGeometry* Geo = nullptr;

		//Primitive Topology
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		//DrawIndexedInstanced parameters
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};

	struct LitShapesRenderItem : RenderItem {
		LitShapesRenderItem() = default;

		bool Visible = true;
		
		std::string Name;

		//World matrix of shape that describes object's local space.
		//Relative to world space, which defines position, orientation and scale of object in world
		XMFLOAT4X4 World = MatrixIdentity4X4();

		XMFLOAT4X4 TexTransform = MatrixIdentity4X4();

		// Used for GPU waves render items.
		DirectX::XMFLOAT2 DisplacementMapTexelSize = {1.0f, 1.0f};
		float GridSpatialStep = 1.0f;

		//Dirty flag indicating object data has changed and we need to update constant buffer
		//Because we have an object cbuffer for each resource we have to apply update to each resource
		//Thus, when we modify object data we should set NumFramesDirty = gNumFrameResources, so that each frame resource gets update.
		int NumFramesDirty = gNumFrameResources;

		//Index into GPU constant buffer corresponding to ObjectCB for render item
		UINT ObjCBIndex = -1;

		Material* Mat = nullptr;
		MeshGeometry* Geo = nullptr;

		//Primitive Topology
		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		BoundingBox Bounds;
		std::vector<InstanceData> Instances;
		//DrawIndexedInstanced parameters
		UINT IndexCount = 0;
		UINT InstanceCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation = 0;
	};

	interface CEObject {
	public:
		virtual ~CEObject() = default;
		virtual int RowCount() const = 0;
		virtual int ColumnCount() const = 0;
		virtual int VertexCount() const = 0;
		virtual int TriangleCount() const = 0;
		virtual float Width() const = 0;
		virtual float Depth() const = 0;

		// Returns the solution at the ith grid point.
		virtual const DirectX::XMFLOAT3& Position(int i) const { return mCurrSolution[i]; }

		// Returns the solution normal at the ith grid point.
		virtual const DirectX::XMFLOAT3& Normal(int i) const { return mNormals[i]; }

		// Returns the unit tangent vector at the ith grid point in the local x-axis direction.
		virtual const DirectX::XMFLOAT3& TangentX(int i) const { return mTangentX[i]; }

		virtual void Update(float dt) = 0;
		virtual void Disturb(int i, int j, float magnitude) = 0;

	protected:
		int m_numRows = 0;
		int m_numCols = 0;

		int mVertexCount = 0;
		int mTriangleCount = 0;

		// Simulation constants we can precompute.
		float mK1 = 0.0f;
		float mK2 = 0.0f;
		float mK3 = 0.0f;

		float mTimeStep = 0.0f;
		float mSpatialStep = 0.0f;

		std::vector<XMFLOAT3> mPrevSolution;
		std::vector<XMFLOAT3> mCurrSolution;
		std::vector<XMFLOAT3> mNormals;
		std::vector<XMFLOAT3> mTangentX;

	private:
	};
}
