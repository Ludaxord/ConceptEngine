#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <wrl.h>

#include "CEVertexTypes.h"


namespace Concept::GraphicsEngine::Direct3D {

	class CEBuffer;
	class CEAccelerationStructuredBuffer;
	class CEByteAddressBuffer;
	class CEConstantBuffer;
	class CEConstantBufferView;
	class CEDevice;
	class CEDynamicDescriptorHeap;
	class CEGenerateMipsPSO;
	class CEIndexBuffer;
	class CEPanoToCubemapPSO;
	class CEPipelineStateObject;
	class CERenderTarget;
	class CEResource;
	class CEResourceStateTracker;
	class CERootSignature;
	class CEScene;
	class CEMesh;
	class CEShaderResourceView;
	class CEStructuredBuffer;
	class CETexture;
	class CEUnorderedAccessView;
	class CEUploadBuffer;
	class CEVertexBuffer;

	class CECommandList : public std::enable_shared_from_this<CECommandList> {
	public:
		/**
		 * Get the type of command list.
		 */
		D3D12_COMMAND_LIST_TYPE GetCommandListType() const {
			return m_commandListType;
		}

		/**
		 * Get the device that was used to create this command list.
		 */
		CEDevice& GetDevice() const {
			return m_device;
		}

		/**
		 * Get direct access to the ID3D12GraphicsCommandList interface.
		 */
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() const {
			return m_commandList;
		}
		/**
		 * Get direct access to the ID3D12GraphicsCommandList5 interface.
		 */
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> GetRayTracingCommandList() const {
			return m_rtxCommandList;
		}

		/**
		 * Get direct access to the ID3D12CommandAllocator interface.
		 * NOTE: Temporary just for RTXPlayground
		 */
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCommandAllocator() const {
			return m_commandAllocator;
		}

		/**
		 * Transition a resource to a particular state.
		 *
		 * @param resource The resource to transition.
		 * @param stateAfter The state to transition the resource to. The before state is resolved by the resource state
		 * tracker.
		 * @param subresource The subresource to transition. By default, this is D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
		 * which indicates that all subresources are transitioned to the same state.
		 * @param flushBarriers Force flush any barriers. Resource barriers need to be flushed before a command (draw,
		 * dispatch, or copy) that expects the resource to be in a particular state can run.
		 */
		void TransitionBarrier(const std::shared_ptr<CEResource>& resource, D3D12_RESOURCE_STATES stateAfter,
		                       UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flushBarriers = false);
		void TransitionBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES stateAfter,
		                       UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flushBarriers = false);

		/**
		 * Add a UAV barrier to ensure that any writes to a resource have completed
		 * before reading from the resource.
		 *
		 * @param [resource} The resource to add a UAV barrier for (can be null).
		 * @param flushBarriers Force flush any barriers. Resource barriers need to be
		 * flushed before a command (draw, dispatch, or copy) that expects the resource
		 * to be in a particular state can run.
		 */
		void UAVBarrier(const std::shared_ptr<CEResource>& resource = nullptr, bool flushBarriers = false);
		void UAVBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> resource, bool flushBarriers = false);

		/**
		 * Add an aliasing barrier to indicate a transition between usages of two
		 * different resources that occupy the same space in a heap.
		 *
		 * @param [beforeResource] The resource that currently occupies the heap (can be null).
		 * @param [afterResource] The resource that will occupy the space in the heap (can be null).
		 */
		void AliasingBarrier(const std::shared_ptr<CEResource>& = nullptr,
		                     const std::shared_ptr<CEResource>& afterResource = nullptr, bool flushBarriers = false);
		void AliasingBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> beforeResource,
		                     Microsoft::WRL::ComPtr<ID3D12Resource> afterResource, bool flushBarriers = false);

		/**
		 * Flush any barriers that have been pushed to the command list.
		 */
		void FlushResourceBarriers();

		/**
		 * Copy resources.
		 */
		void CopyResource(const std::shared_ptr<CEResource>& dstRes, const std::shared_ptr<CEResource>& srcRes);
		void CopyResource(Microsoft::WRL::ComPtr<ID3D12Resource> dstRes, Microsoft::WRL::ComPtr<ID3D12Resource> srcRes);

		/**
		 * Resolve a multisampled resource into a non-multisampled resource.
		 */
		void ResolveSubResource(const std::shared_ptr<CEResource>&, const std::shared_ptr<CEResource>&,
		                        uint32_t dstSubresource = 0, uint32_t srcSubresource = 0);

		/**
		 * Copy the contents to a vertex buffer in GPU memory.
		 */
		std::shared_ptr<CEVertexBuffer> CopyVertexBuffer(size_t numVertices, size_t vertexStride,
		                                                 const void* vertexBufferData);

		template <typename T>
		std::shared_ptr<CEVertexBuffer> CopyVertexBuffer(const std::vector<T>& vertexBufferData) {
			return CopyVertexBuffer(vertexBufferData.size(), sizeof(T), vertexBufferData.data());
		}

		/**
		 * Copy the contents to a index buffer in GPU memory.
		 */
		std::shared_ptr<CEIndexBuffer> CopyIndexBuffer(size_t numIndicies, DXGI_FORMAT indexFormat,
		                                               const void* indexBufferData);

		template <typename T>
		std::shared_ptr<CEIndexBuffer> CopyIndexBuffer(const std::vector<T>& indexBufferData) {
			assert(sizeof(T) == 2 || sizeof(T) == 4);

			DXGI_FORMAT indexFormat = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			return CopyIndexBuffer(indexBufferData.size(), indexFormat, indexBufferData.data());
		}

		/**
		 * Copy the contents to a constant buffer in GPU memory.
		 */
		std::shared_ptr<CEConstantBuffer> CopyConstantBuffer(size_t bufferSize, const void* bufferData);

		template <typename T>
		std::shared_ptr<CEConstantBuffer> CopyConstantBuffer(const T& data) {
			return CopyConstantBuffer(sizeof(T), &data);
		}

		/**
		 * Copy the contents to a byte address buffer in GPU memory.
		 */
		std::shared_ptr<CEByteAddressBuffer> CopyByteAddressBuffer(size_t bufferSize, const void* bufferData);

		template <typename T>
		std::shared_ptr<CEByteAddressBuffer> CopyByteAddressBuffer(const T& data) {
			return CopyByteAddressBuffer(sizeof(T), &data);
		}

		/**
		 * TODO: Implement
		 * Copy the contents to a byte address buffer in GPU memory.
		 */
		std::shared_ptr<CEAccelerationStructuredBuffer> CopyAccelerationStructuredBuffer(
			size_t bufferSize, const void* bufferData);

		template <typename T>
		std::shared_ptr<CEAccelerationStructuredBuffer> CopyAccelerationStructuredBuffer(const std::vector<T>& data) {
			return CopyAccelerationStructuredBuffer(sizeof(T), data.data());
		}

		/**
		 * Copy the contents to a structured buffer in GPU memory.
		 */
		std::shared_ptr<CEStructuredBuffer> CopyStructuredBuffer(size_t numElements, size_t elementSize,
		                                                         const void* bufferData);

		template <typename T>
		std::shared_ptr<CEStructuredBuffer> CopyStructuredBuffer(const std::vector<T>& bufferData) {
			return CopyStructuredBuffer(bufferData.size(), sizeof(T), bufferData.data());
		}

		/**
		 * Set the current primitive topology for the rendering pipeline.
		 */
		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology);
		
		/*
		 * Load Image from filename
		 */
		int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename,
		                          int& bytesPerRow);

		/**
		 * Load a texture by a filename.
		 */
		std::shared_ptr<CETexture> LoadTextureFromFile(const std::wstring& fileName, bool sRGB = false);

		/**
		 * Load a scene file.
		 *
		 * @param fileName The path to the scene file definition.
		 * @param [loadingProgress] An optional callback function that can be used to report loading progress.
		 */
		std::shared_ptr<CEScene>
		LoadSceneFromFile(const std::wstring& fileName,
		                  const std::function<bool(float)>& loadingProgres = std::function<bool(float)>());

		/**
		 * Load a scene from a string.
		 *
		 * @param sceneString The scene file definition as a string.
		 * @param format The format of the scene file. (for example "NFF")
		 *
		 * @see https://www.fileformat.info/format/nff/egff.htm
		 */
		std::shared_ptr<CEScene> LoadSceneFromString(const std::string& sceneString, const std::string& format);

		/**
		 * Create a cube.
		 *
		 * @param size The size of one side of the cube.
		 * @param reverseWinding Whether to reverse the winding order of the triangles (useful for skyboxes).
		 */
		std::shared_ptr<CEScene> CreateCube(float size = 1.0, bool reverseWinding = false);

		/**
		 * Create a sphere.
		 *
		 * @param radius Radius of the sphere.
		 * @param tessellation Determines how smooth the sphere is.
		 * @param reverseWinding Whether to reverse the winding order of the triangles (useful for sydomes).
		 */
		std::shared_ptr<CEScene> CreateSphere(float radius = 0.5f, uint32_t tessellation = 16,
		                                      bool reversWinding = false);

		/**
		 * Create a Cylinder
		 *
		 * @param radius The radius of the primary axis of the cylinder.
		 * @param hight The height of the cylinder.
		 * @param tessellation How smooth the cylinder will be.
		 * @param reverseWinding Whether to reverse the winding order of the triangles.
		 */
		std::shared_ptr<CEScene> CreateCylinder(float radius = 0.5f, float height = 1.0f, uint32_t tessellation = 32,
		                                        bool reverseWinding = false);

		/**
		 * Create a cone.
		 *
		 * @param radius The radius of the base of the cone.
		 * @param height The height of the cone.
		 * @param tessellation How smooth to make the cone.
		 * @param reverseWinding Whether to reverse the winding order of the triangles.
		 */
		std::shared_ptr<CEScene> CreateCone(float radius = 0.5f, float height = 1.0f, uint32_t tessellation = 32,
		                                    bool reverseWinding = false);

		/**
		 * Create a torus.
		 *
		 * @param radius The radius of the torus.
		 * @param thickness The The thickness of the torus.
		 * @param tessellation The smoothness of the torus.
		 * @param reverseWinding Reverse the winding order of the vertices.
		 */
		std::shared_ptr<CEScene> CreateTorus(float radius = 0.5f, float thickness = 0.333f, uint32_t tessellation = 32,
		                                     bool reverseWinding = false);

		/**
		 * Create a plane.
		 *
		 * @param width The width of the plane.
		 * @param height The height of the plane.
		 * @reverseWinding Whether to reverse the winding order of the plane.
		 */
		std::shared_ptr<CEScene> CreatePlane(float width = 1.0f, float height = 1.0f, bool reverseWinding = false);

		/*
		 * Create Bottom Level Acceleration Structure
		 */
		void CreateBottomLevelAccelerationStructure(std::shared_ptr<CEMesh> mesh);

		/*
		 * Create Top Level AccelerationStructure
		 */
		void CreateTopLevelAccelerationStructure();

		/*
		 * Create Ray Tracing Output
		 */
		void CreateRayTracingOutput();
		
		/**
		 * Clear a texture.
		 */
		void ClearTexture(const std::shared_ptr<CETexture>& texture, const float clearColor[4]);

		/**
		 * Clear depth/stencil texture.
		 */
		void ClearDepthStencilTexture(const std::shared_ptr<CETexture>& texture, D3D12_CLEAR_FLAGS clearFlags,
		                              float depth = 1.0f, uint8_t stencil = 0);

		/**
		 * Generate mips for the texture.
		 * The first subresource is used to generate the mip chain.
		 * Mips are automatically generated for textures loaded from files.
		 */
		void GenerateMips(const std::shared_ptr<CETexture>& texture);

		/**
		 * Generate a cubemap texture from a panoramic (equirectangular) texture.
		 */
		void PanoToCubeMap(const std::shared_ptr<CETexture>& cubemapTexture,
		                   const std::shared_ptr<CETexture>& panoTexture);

		/**
		 * Copy subresource data to a texture.
		 */
		void CopyTextureSubResource(const std::shared_ptr<CETexture>& texture, uint32_t firstSubresource,
		                            uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData);

		/**
		 * Set a dynamic constant buffer data to an inline descriptor in the root
		 * signature.
		 */
		void SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, size_t sizeInBytes, const void* bufferData);

		template <typename T>
		void SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, const T& data) {
			SetGraphicsDynamicConstantBuffer(rootParameterIndex, sizeof(T), &data);
		}

		/**
		 * Set a set of 32-bit constants on the graphics pipeline.
		 */
		void SetGraphics32BitConstants(uint32_t rootParameterIndex, uint32_t numConstants, const void* constants);

		template <typename T>
		void SetGraphics32BitConstants(uint32_t rootParameterIndex, const T& constants) {
			static_assert(sizeof(T) % sizeof(uint32_t) == 0, "Size of type must be a multiple of 4 bytes");
			SetGraphics32BitConstants(rootParameterIndex, sizeof(T) / sizeof(uint32_t), &constants);
		}

		/**
		 * Set a set of 32-bit constants on the compute pipeline.
		 */
		void SetCompute32BitConstants(uint32_t rootParameterIndex, uint32_t numConstants, const void* constants);

		template <typename T>
		void SetCompute32BitConstants(uint32_t rootParameterIndex, const T& constants) {
			static_assert(sizeof(T) % sizeof(uint32_t) == 0, "Size of type must be a multiple of 4 bytes");
			SetCompute32BitConstants(rootParameterIndex, sizeof(T) / sizeof(uint32_t), &constants);
		}

		/**
		 * Set the vertex buffer to the rendering pipeline.
		 *
		 * @param slot The slot to bind the vertex buffer to.
		 * @vertexBuffer The vertex buffer to bind (can be null to remove the vertex buffer from the slot).
		 */
		void SetVertexBuffers(uint32_t startSlot,
		                      const std::vector<std::shared_ptr<CEVertexBuffer>>& vertexBufferViews);
		void SetVertexBuffer(uint32_t slot, const std::shared_ptr<CEVertexBuffer>& vertexBufferView);

		/**
		 * Set dynamic vertex buffer data to the rendering pipeline.
		 */
		void SetDynamicVertexBuffer(uint32_t slot, size_t numVertices, size_t vertexSize, const void* vertexBufferData);

		template <typename T>
		void SetDynamicVertexBuffer(uint32_t slot, const std::vector<T>& vertexBufferData) {
			SetDynamicVertexBuffer(slot, vertexBufferData.size(), sizeof(T), vertexBufferData.data());
		}

		/**
		 * Bind the index buffer to the rendering pipeline.
		 *
		 * @param indexBuffer The index buffer to bind to the rendering pipeline.
		 */
		void SetIndexBuffer(const std::shared_ptr<CEIndexBuffer>& indexBuffer);

		/**
		 * Bind dynamic index buffer data to the rendering pipeline.
		 */
		void SetDynamicIndexBuffer(size_t numIndicies, DXGI_FORMAT indexFormat, const void* indexBufferData);

		template <typename T>
		void SetDynamicIndexBuffer(const std::vector<T>& indexBufferData) {
			static_assert(sizeof(T) == 2 || sizeof(T) == 4);

			DXGI_FORMAT indexFormat = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			SetDynamicIndexBuffer(indexBufferData.size(), indexFormat, indexBufferData.data());
		}

		/**
		 * Set dynamic structured buffer contents.
		 */
		void SetGraphicsDynamicStructuredBuffer(uint32_t slot, size_t numElements, size_t elementSize,
		                                        const void* bufferData);

		template <typename T>
		void SetGraphicsDynamicStructuredBuffer(uint32_t slot, const std::vector<T>& bufferData) {
			SetGraphicsDynamicStructuredBuffer(slot, bufferData.size(), sizeof(T), bufferData.data());
		}

		/**
		 * Set viewports.
		 */
		void SetViewport(const D3D12_VIEWPORT& viewport);
		void SetViewports(const std::vector<D3D12_VIEWPORT>& viewports);

		/**
		 * Set scissor rects.
		 */
		void SetScissorRect(const D3D12_RECT& scissorRect);
		void SetScissorRects(const std::vector<D3D12_RECT>& scissorRects);

		/**
		 * Set the pipeline state object on the command list.
		 */
		void SetPipelineState(const std::shared_ptr<CEPipelineStateObject>& pipelineState);

		/**
		 * Set the current root signature on the command list.
		 */
		void SetGraphicsRootSignature(const std::shared_ptr<CERootSignature>& rootSignature);
		void SetComputeRootSignature(const std::shared_ptr<CERootSignature>& rootSignature);

		/**
		 * Set an inline CBV.
		 *
		 * Note: Only ConstantBuffer's can be used with inline CBV's.
		 */
		void SetConstantBufferView(uint32_t rootParameterIndex, const std::shared_ptr<CEConstantBuffer>& buffer,
		                           D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		                           size_t bufferOffset = 0);

		/**
		 * Set an inline SRV.
		 *
		 * Note: Only Buffer resources can be used with inline SRV's
		 */
		void SetShaderResourceView(uint32_t rootParameterIndex, const std::shared_ptr<CEBuffer>& buffer,
		                           D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
			                           D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		                           size_t bufferOffset = 0);
		/**
		 * Set an inline UAV.
		 *
		 * Note: Only Buffer resoruces can be used with inline UAV's.
		 */
		void SetUnorderedAccessView(uint32_t rootParameterIndex, const std::shared_ptr<CEBuffer>& buffer,
		                            D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		                            size_t bufferOffset = 0);

		/**
		 * Set the CBV on the rendering pipeline.
		 */
		void SetConstantBufferView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
		                           const std::shared_ptr<CEConstantBufferView>& cbv,
		                           D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		/**
		 * Set the SRV on the graphics pipeline.
		 */
		void SetShaderResourceView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
		                           const std::shared_ptr<CEShaderResourceView>& srv,
		                           D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
			                           D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		                           UINT firstSubresource = 0,
		                           UINT numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		/**
		 * Set an SRV on the graphics pipeline using the default SRV for the texture.
		 */
		void SetShaderResourceView(int32_t rootParameterIndex, uint32_t descriptorOffset,
		                           const std::shared_ptr<CETexture>& texture,
		                           D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
			                           D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		                           UINT firstSubresource = 0,
		                           UINT numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
		/**
		 * Set the UAV on the graphics pipeline.
		 */
		void SetUnorderedAccessView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
		                            const std::shared_ptr<CEUnorderedAccessView>& uav,
		                            D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		                            UINT firstSubresource = 0,
		                            UINT numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		/**
		 * Set the UAV on the graphics pipline using a specific mip of the texture.
		 */
		void SetUnorderedAccessView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
		                            const std::shared_ptr<CETexture>& texture, UINT mip,
		                            D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		                            UINT firstSubresource = 0,
		                            UINT numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		/**
		 * Set the render targets for the graphics rendering pipeline.
		 */
		void SetRenderTarget(const CERenderTarget& renderTarget);

		/**
		 * Draw geometry.
		 */
		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t startVertex = 0,
		          uint32_t startInstance = 0);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t startIndex = 0,
		                 int32_t baseVertex = 0,
		                 uint32_t startInstance = 0);

		/**
		 * Dispatch a compute shader.
		 */
		void Dispatch(uint32_t numGroupsX, uint32_t numGroupsY = 1, uint32_t numGroupsZ = 1);

	protected:
		friend class CECommandQueue;
		friend class CEDynamicDescriptorHeap;
		friend class std::default_delete<CECommandList>;

		CECommandList(CEDevice& device, D3D12_COMMAND_LIST_TYPE type);
		virtual ~CECommandList();

		/*
		 * Ray tracing Command List interface
		 */
		void CreateRayTracingCommandList();

		/**
		 * Close the command list.
		 * Used by the command queue.
		 *
		 * @param pendingCommandList The command list that is used to execute pending
		 * resource barriers (if any) for this command list.
		 *
		 * @return true if there are any pending resource barriers that need to be
		 * processed.
		 */
		bool Close(const std::shared_ptr<CECommandList>& pendingCommandList);
		// Just close the command list. This is useful for pending command lists.
		void Close();

		/**
		 * Reset the command list. This should only be called by the CommandQueue
		 * before the command list is returned from CommandQueue::GetCommandList.
		 */
		void Reset();

		/**
		 * Release tracked objects. Useful if the swap chain needs to be resized.
		 */
		void ReleaseTrackedObjects();

		/**
		 * Set the currently bound descriptor heap.
		 * Should only be called by the DynamicDescriptorHeap class.
		 */
		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap);

		std::shared_ptr<CECommandList> GetGenerateMipsCommandList() const {
			return m_computeCommandList;
		}

	private:
		// Used for procedural mesh generation.
		using VertexCollection = std::vector<CEVertexPositionNormalTangentBitangentTexture>;
		using IndexCollection = std::vector<uint16_t>;

		// Create a scene that contains a single node with a single mesh.
		std::shared_ptr<CEScene> CreateScene(const VertexCollection& vertices, const IndexCollection& indicies);

		// Helper function for flipping winding of geometric primitives for LH vs. RH coords
		inline void ReverseWinding(IndexCollection& indices, VertexCollection& vertices);
		// Helper function for inverting normals for "inside" vs "outside" viewing.
		inline void CECommandList::InvertNormals(VertexCollection& vertices);
		// Helper function to compute a point on a unit circle aligned to the x,z plane and centered at the origin.
		inline DirectX::XMVECTOR GetCircleVector(size_t i, size_t tessellation) noexcept;
		// Helper function to compute a tangent vector at a point on a unit sphere aligned to the x,z plane.
		inline DirectX::XMVECTOR GetCircleTangent(size_t i, size_t tessellation) noexcept;
		// Helper creates a triangle fan to close the end of a cylinder / cone
		void CreateCylinderCap(VertexCollection& vertices, IndexCollection& indices, size_t tessellation, float height,
		                       float radius, bool isTop);

		// Add a resource to a list of tracked resources (ensures lifetime while command list is in-flight on a command
		// queue.
		void TrackResource(Microsoft::WRL::ComPtr<ID3D12Object> object);
		void TrackResource(const std::shared_ptr<CEResource>& res);

		// Generate mips for UAV compatible textures.
		void GenerateMipsUAV(const std::shared_ptr<CETexture>& texture, bool isSRGB);

		// Copy the contents of a CPU buffer to a GPU buffer (possibly replacing the previous buffer contents).
		Microsoft::WRL::ComPtr<ID3D12Resource> CopyBuffer(size_t bufferSize, const void* bufferData,
		                                                  D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

		// Binds the current descriptor heaps to the command list.
		void BindDescriptorHeaps();

		// The device that is used to create this command list.
		CEDevice& m_device;
		D3D12_COMMAND_LIST_TYPE m_commandListType;
		//TODO: Create command lists depends on RTX Support
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> m_rtxCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

		// For copy queues, it may be necessary to generate mips while loading textures.
		// Mips can't be generated on copy queues but must be generated on compute or
		// direct queues. In this case, a Compute command list is generated and executed
		// after the copy queue is finished uploading the first sub resource.
		std::shared_ptr<CECommandList> m_computeCommandList;

		// Keep track of the currently bound root signatures to minimize root
		// signature changes.
		ID3D12RootSignature* m_rootSignature;
		// Keep track of the currently bond pipeline state object to minimize PSO changes.
		ID3D12PipelineState* m_pipelineState;

		// Resource created in an upload heap. Useful for drawing of dynamic geometry
		// or for uploading constant buffer data that changes every draw call.
		std::unique_ptr<CEUploadBuffer> m_uploadBuffer;

		// Resource state tracker is used by the command list to track (per command list)
		// the current state of a resource. The resource state tracker also tracks the
		// global state of a resource in order to minimize resource state transitions.
		std::unique_ptr<CEResourceStateTracker> m_resourceStateTracker;

		// The dynamic descriptor heap allows for descriptors to be staged before
		// being committed to the command list. Dynamic descriptors need to be
		// committed before a Draw or Dispatch.
		std::unique_ptr<CEDynamicDescriptorHeap> m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		// Keep track of the currently bound descriptor heaps. Only change descriptor
		// heaps if they are different than the currently bound descriptor heaps.
		ID3D12DescriptorHeap* m_descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		// Pipeline state object for Mip map generation.
		std::unique_ptr<CEGenerateMipsPSO> m_generateMipsPSO;
		// Pipeline state object for converting panorama (equirectangular) to cubemaps
		std::unique_ptr<CEPanoToCubemapPSO> m_panoToCubemapPSO;

		using TrackedObjects = std::vector<Microsoft::WRL::ComPtr<ID3D12Object>>;

		// Objects that are being tracked by a command list that is "in-flight" on
		// the command-queue and cannot be deleted. To ensure objects are not deleted
		// until the command list is finished executing, a reference to the object
		// is stored. The referenced objects are released when the command list is
		// reset.
		TrackedObjects m_trackedObjects;

		// Keep track of loaded textures to avoid loading the same texture multiple times.
		static std::map<std::wstring, ID3D12Resource*> m_textureCache;
		static std::mutex m_textureCacheMutex;
	};

	// Definition for inline functions.
	inline DirectX::XMVECTOR CECommandList::GetCircleVector(size_t i, size_t tessellation) noexcept {
		float angle = float(i) * DirectX::XM_2PI / float(tessellation);
		float dx, dz;

		DirectX::XMScalarSinCos(&dx, &dz, angle);

		DirectX::XMVECTORF32 v = {{{dx, 0, dz, 0}}};
		return v;
	}

	inline DirectX::XMVECTOR CECommandList::GetCircleTangent(size_t i, size_t tessellation) noexcept {
		float angle = (float(i) * DirectX::XM_2PI / float(tessellation)) + DirectX::XM_PIDIV2;
		float dx, dz;

		DirectX::XMScalarSinCos(&dx, &dz, angle);

		DirectX::XMVECTORF32 v = {{{dx, 0, dz, 0}}};
		return v;
	}

	inline void CECommandList::ReverseWinding(IndexCollection& indices, VertexCollection& vertices) {
		assert((indices.size() % 3) == 0);
		for (auto it = indices.begin(); it != indices.end(); it += 3) {
			std::swap(*it, *(it + 2));
		}

		for (auto it = vertices.begin(); it != vertices.end(); ++it) {
			it->TexCoord.x = (1.f - it->TexCoord.x);
		}
	}

	inline void CECommandList::InvertNormals(VertexCollection& vertices) {
		for (auto it = vertices.begin(); it != vertices.end(); ++it) {
			it->Normal.x = -it->Normal.x;
			it->Normal.y = -it->Normal.y;
			it->Normal.z = -it->Normal.z;
		}
	}


}
