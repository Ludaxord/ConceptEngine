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

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {

	static XMFLOAT4X4 MatrixIdentity4X4() {
		return XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
		                  0.0f, 1.0f, 0.0f, 0.0f,
		                  0.0f, 0.0f, 1.0f, 0.0f,
		                  0.0f, 0.0f, 0.0f, 1.0f);
	}

	struct CEVertex {
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};

	struct CEObjectConstants {
		XMFLOAT4X4 WorldViewProjection = MatrixIdentity4X4();
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

	struct MaterialConstants {
		XMFLOAT4 DiffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
		XMFLOAT3 FresnelR0 = {0.01f, 0.01f, 0.01f};
		float Roughness = 0.25f;

		// Used in texture mapping.
		XMFLOAT4X4 MatTransform = MatrixIdentity4X4();
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
}
