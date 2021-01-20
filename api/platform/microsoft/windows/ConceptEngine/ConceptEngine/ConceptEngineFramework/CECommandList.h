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

class CEBuffer;

namespace Concept::GraphicsEngine::Direct3D {
	class CEStructuredBuffer;
	class CEByteAddressBuffer;
	class CEPanoToCubemapPSO;
	class CEGenerateMipsPSO;
	class CEResourceStateTracker;
	class CEUploadBuffer;
	class CERenderTarget;
	class CEUnorderedAccessView;
	class CEConstantBufferView;
	class CEShaderResourceView;
	class CEConstantBuffer;
	class CERootSignature;
	class CEPipelineStateObject;
	class CEIndexBuffer;
	class CEScene;
	class CETexture;
	class CEVertexBuffer;
	class CEResource;
	class CEDevice;
	namespace wrl = Microsoft::WRL;

	class CECommandList : public std::enable_shared_from_this<CECommandList> {
	public:
		/**
		 * Get command list type;
		 */
		D3D12_COMMAND_LIST_TYPE GetCommandListType() const {
			return m_commandListType;
		}

		/**
		 * Get device that is used to create command list;
		 */
		CEDevice& GetDevice() const {
			return m_device;
		}

		/**
		 * Get access to CommandList Interface
		 */
		wrl::ComPtr<ID3D12GraphicsCommandList5> GetCommandList() const {
			return m_commandList;
		}

		/**
		 * Transition resource to particular state.
		 * @param resource -> resource to transition.
		 * @param stateAfter -> state to transition the resource to.
		 * Before state is resolved by the resource state which indicates that all subresources are transitioned to same state.
		 * @param flushBarriers force flush any barriers. Resource barriers need to be flushed before command
		 * (draw, dispatch, copy) that expects resource to be in a particular state can run
		*/
		void TransitionBarrier(const std::shared_ptr<CEResource>& resource, D3D12_RESOURCE_STATES stateAfter,
		                       UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flushBarriers = false);
		void TransitionBarrier(wrl::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES stateAfter,
		                       UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, bool flushBarriers = false);
		/**
		 * Add UAV barrier to ensure that any writes to a resource have completed before reading from resource.
		 * @param [resource] resource to add a UAV barrier for (can be null);
		 * @param [flushBarriers] Force flush any barriers.
		 * Resource barriers need to be flushed before a command (draw, dispatch or copy) that expects the resource to be in a particular state can run.
 		 */
		void UAVBarrier(const std::shared_ptr<CEResource>& resource = nullptr, bool flushBarriers = false);
		void UAVBarrier(const wrl::ComPtr<ID3D12Resource> resource = nullptr, bool flushBarriers = false);

		/**
		 * Add aliasing barrier to indicate transition between usage of two different resource that occupy same space in heap.
		 * @param [beforeResource] resource that currently occupies heap (can be null)
		 * @param [afterResource] resource that will occupy space in heap (can be null)
		 */
		void AliasingBarrier(const std::shared_ptr<CEResource>& beforeResource = nullptr,
		                     const std::shared_ptr<CEResource>& afterResource = nullptr,
		                     bool flushBarriers = false);
		void AliasingBarrier(wrl::ComPtr<ID3D12Resource> beforeResource,
		                     wrl::ComPtr<ID3D12Resource>& afterResource, bool flushBarriers = false);

		/**
		 * Flush any barriers that have been pushed to command list
		 */
		void FlushResourceBarriers();

		/**
		 * Copy Resources
		 */
		void CopyResource(const std::shared_ptr<CEResource>& destinationResource,
		                  const std::shared_ptr<CEResource>& sourceResource);
		void CopyResource(wrl::ComPtr<ID3D12Resource> destinationResource,
		                  wrl::ComPtr<ID3D12Resource> sourceResource);
		/**
		 * Resolve multi-sampled resource into non-multisampled resource.
		 */
		void ResolveSubResource(const std::shared_ptr<CEResource>& destinationResource,
		                        const std::shared_ptr<CEResource>& sourceResource,
		                        uint32_t destinationSubResource = 0, uint32_t sourceSubResource = 0);

		/**
		 * Copy content to a vertex buffer in GPU Memory;
		 */
		std::shared_ptr<CEVertexBuffer> CopyVertexBuffer(size_t numVertices, size_t vertexStride,
		                                                 const void* vertexBufferData);

		template <typename T>
		std::shared_ptr<CEVertexBuffer> CopyVertexBuffer(const std::vector<T>& vertexBufferData) {
			return CopyVertexBuffer(vertexBufferData.size(), sizeof(T), vertexBufferData.data());
		}

		/**
		 * Copy content to a index buffer in GPU Memory;
		 */
		std::shared_ptr<CEIndexBuffer> CopyIndexBuffer(size_t numIndices, DXGI_FORMAT indexFormat,
		                                               const void* indexBufferData);

		template <typename T>
		std::shared_ptr<CEIndexBuffer> CopyIndexBuffer(const std::vector<T>& indexBufferData) {
			assert(sizeof(T) == 2 || sizeof(T) == 4);
			DXGI_FORMAT indexFormat = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			return CopyIndexBuffer(indexBufferData.size(), indexFormat, indexBufferData.data());
		}

		/**
		 * Copy content to a constant buffer in GPU Memory;
		 */
		std::shared_ptr<CEConstantBuffer> CopyConstantBuffer(size_t bufferSize,
		                                                     const void* bufferData);

		template <typename T>
		std::shared_ptr<CEConstantBuffer> CopyConstantBuffer(const T& bufferData) {
			return CopyConstantBuffer(sizeof(T), bufferData);
		}

		/**
		 * Copy content to a byte address buffer in GPU Memory;
		 */
		std::shared_ptr<CEByteAddressBuffer> CopyByteAddressBuffer(size_t bufferSize,
		                                                           const void* bufferData);

		template <typename T>
		std::shared_ptr<CEByteAddressBuffer> CopyByteAddressBuffer(const T& bufferData) {
			return CopyByteAddressBuffer(sizeof(T), &bufferData);
		}

		/**
		 * Copy content to a structured buffer in GPU Memory;
		 */
		std::shared_ptr<CEStructuredBuffer> CopyStructuredBuffer(size_t numElements, size_t elementSize,
		                                                         const void* bufferData);

		template <typename T>
		std::shared_ptr<CEStructuredBuffer> CopyStructuredBuffer(const std::vector<T>& bufferData) {
			return CopyStructuredBuffer(bufferData.size(), sizeof(T), bufferData.data());
		}


		/**
		 * Set current primitive topology for rendering pipeline.
		 */
		void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology);

		/**
		 * Load texture from file
		 */
		std::shared_ptr<CETexture> LoadTextureFromFile(const std::wstring& fileName, bool sRGB = false);

		/**
		 * Load scene file
		 *
		 * @param fileName path to scene file definition.
		 * @param [loadingProgress] optional callback function that can be used to report loading progress
		 */
		std::shared_ptr<CEScene> LoadSceneFromFile(const std::wstring& fileName,
		                                           const std::function<bool(float)>& loadingProgress =
			                                           std::function<bool(float)>());
		/**
		 * Load Scene from string.
		 *
		 * @param sceneString, scene file definition as a string.
		 * @param format, format of scene file (for example "NFF" or original "*.Concept")
		 * https://www.fileformat.info/format/nff/egff.htm
		 */
		std::shared_ptr<CEScene> LoadSceneFromString(const std::string& sceneString, const std::string& format);

		/**
		 * TODO: Separate objects in scene from scene, right now every objects is a Scene, create Scene Objects to place it in scene. IMPORTANT AFTER CREATE GRAPHICS ENGINE!
		 */

		/**
		 * Create Cube
		 *
		 * @param size, size of one side of the cube
		 * @param reverseWinding, whether to reverse winding order of triangles (useful for skyboxes)
		 */
		std::shared_ptr<CEScene> CreateCube(float size = 1.0, bool reverseWinding = false);

		/**
		 * Create Sphere
		 *
		 * @param radius, Radius of sphere.
		 * @param tessellation, determines how smooth sphere is.
		 * @param reverseWinding, whether to reverse winding order of triangles (useful for sydomes)
		 */
		std::shared_ptr<CEScene> CreateSphere(float radius = 0.5f, uint32_t tesselation = 16,
		                                      bool reverseWinding = false);

		/**
		 * Create Cylinder
		 *
		 * @param radius, radius of primitive axis of cylinder.
		 * @param height, height of cylinder.
		 * @param tessellation, determines how smooth cylinder will be.
		 * @param reverseWinding, whether to reverse winding order of triangles
		 */
		std::shared_ptr<CEScene> CreateCylinder(float radius = 0.5f, float height = 1.0f,
		                                        uint32_t tessellation = 32, bool reverseWinding = false);

		/**
		 * Create Cone
		 *
		 * @param radius, radius of base cone,
		 * @param height, height of cone,
		 * @param tessellation, how smooth is cone.
		 * @param reverseWinding, Whether to reverse winding order of triangles.
		 */
		std::shared_ptr<CEScene> CreateCone(float radius = 0.5f, float height = 1.0f, uint32_t tessellation = 32,
		                                    bool reverseWinding = false);

		/**
		 * Create Quad
		 *
		 * @param size The size of Quad.
		 * @param reverseWinding Whether to reverse the winding order of the triangles.
		 */
		std::shared_ptr<CEScene> CreateQuad(float size = 1.0, bool reverseWinding = false);

		/**
		 * Create Triangle
		 * 
		 * @param size The size of triangle.
		 * @param reverseWinding Whether to reverse the winding order of the triangle.
		 */
		std::shared_ptr<CEScene> CreateTriangle(float size = 1.0, bool reverseWinding = false);

		/**
		 * Create Torus
		 *
		 * @param radius, radius of torus.
		 * @param thickness, thickness of the torus.
		 * @param tessellation, smoothness of torus.
		 * @param reverseWinding, reverse winding order of vertices.
		 */
		std::shared_ptr<CEScene> CreateTorus(float radius = 0.5f, float thickness = 0.333f,
		                                     uint32_t tessellation = 32, bool reverseWinding = false);

		/**
		 * Create Plane
		 *
		 * @param width, width of plane,
		 * @param height, height of plane,
		 * @param reverseWinding whether to reverse winding order of plane.
		 */
		std::shared_ptr<CEScene> CreatePlane(float width = 1.0f, float height = 1.0f,
		                                     bool reverseWinding = false);

		/**
		 * Create Car
		 * 
		 * @param width, width of Car,
		 * @param height, height of Car,
		 * @param reverseWinding whether to reverse winding order of Car.
		 */
		std::shared_ptr<CEScene> CreateCar(float width = 1.0f, float height = 1.0f,
		                                   bool reverseWinding = false);

		/**
		 * Clear texture
		 */
		void ClearTexture(const std::shared_ptr<CETexture>& texture, const float clearColor[4]);

		/**
		 * Clear depth/stencil texture
		 */
		void ClearDepthStencilTexture(const std::shared_ptr<CETexture>& texture, D3D12_CLEAR_FLAGS clearFlags,
		                              float depth = 1.0f, uint8_t stencil = 0);

		/**
		 * Generate mips for texture
		 * first subresource is used to generate mip chain.
		 * Mips are automatically generated for textures loaded from files.
		 */
		void GenerateMips(const std::shared_ptr<CETexture> texture);

		/**
		 * Generate cubeMap texture from panoramic (EquiRectangular) texture;
		 */
		void PanoToCubeMap(const std::shared_ptr<CETexture>& cubeMapTexture,
		                   const std::shared_ptr<CETexture>& panoTexture);

		/**
		 * Copy SubResource data to texture;
		 */
		void CopyTextureSubResource(const std::shared_ptr<CETexture>& texture, uint32_t firstSubResource,
		                            uint32_t numSubResources, D3D12_SUBRESOURCE_DATA* subResourceData);

		/**
		 * Set Dynamic Constant Buffer data to inline descriptor in root signature;
		 */
		void SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, size_t sizeInBytes, const void* bufferData);

		template <typename T>
		void SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, const T& data) {
			SetGraphicsDynamicConstantBuffer(rootParameterIndex, sizeof(T), &data);
		}

		/**
		 * Set of 32-bit constants on graphics pipeline.
		 */
		void SetGraphics32BitConstants(uint32_t rootParameterIndex, uint32_t numConstants, const void* constants);

		template <typename T>
		void SetGraphics32BitConstants(uint32_t rootParameterIndex, const T& constants) {
			static_assert(sizeof(T) % sizeof(uint32_t) == 0, "Size of given type must be a multiple of 4 bytes");
			SetGraphics32BitConstants(rootParameterIndex, sizeof(T) / sizeof(uint32_t), &constants);
		}

		/*
		 * Set of 32-bit constants on compute pipeline
		 */
		void SetCompute32BitConstants(uint32_t rootParameterIndex, uint32_t numConstants, const void* constants);

		template <typename T>
		void SetCompute32BitConstants(uint32_t rootParameterIndex, const T& constants) {
			static_assert(sizeof(T) % sizeof(uint32_t) == 0, "Size of type must be multiple of 4 bytes");
			SetCompute32BitConstants(rootParameterIndex, sizeof(T) / sizeof(uint32_t), &constants);
		}

		/**
		 * Set vertex buffer to the rendering pipeline.
		 *
		 * @param slot, slot to bind vertex buffer to.
		 * @param vertexBuffer, vertex buffer to bind (can be null to remove vertex buffer from the slot).
		 */
		void SetVertexBuffers(uint32_t startSlot,
		                      const std::vector<std::shared_ptr<CEVertexBuffer>>& vertexBufferViews);
		void SetVertexBuffer(uint32_t slot, const std::shared_ptr<CEVertexBuffer>& vertexBufferView);

		/**
		 * Set Dynamic Vertex Buffer Data to rendering pipeline.
		 */
		void SetDynamicVertexBuffer(uint32_t slot, size_t numVertices, size_t vertexSize, const void* vertexBufferData);

		template <typename T>
		void SetDynamicVertexBuffer(uint32_t slot, const std::vector<T>& vertexBufferData) {
			return SetDynamicVertexBuffer(slot, vertexBufferData.size(), sizeof(T), vertexBufferData.data());
		}

		/**
		 * Bind the index buffer to the rendering pipeline.
		 *
		 * @param indexBuffer, indexBuffer to bind to rendering pipeline
		 */
		void SetIndexBuffer(const std::shared_ptr<CEIndexBuffer>& indexBuffer);

		/**
		 * Bind dynamic index buffer data to the rendering pipeline.
		 */
		void SetDynamicIndexBuffer(size_t numIndicates, DXGI_FORMAT indexFormat, const void* indexBufferData);

		template <typename T>
		void SetDynamicIndexBuffer(const std::vector<T>& indexBufferData) {
			static_assert(sizeof(T) == 2 || sizeof(T) == 4);
			DXGI_FORMAT indexFormat = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			SetDynamicIndexBuffer(indexBufferData.size(), indexFormat, indexBufferData.data());
		}

		/**
		 * Set dynamic structured buffer contents;
		 */
		void SetGraphicsDynamicStructuredBuffer(uint32_t slot, size_t numElements, size_t elementSize,
		                                        const void* bufferData);

		template <typename T>
		void SetGraphicsDynamicStructuredBuffer(uint32_t slot, const std::vector<T>& bufferData) {
			SetGraphicsDynamicStructuredBuffer(slot, bufferData.size(), sizeof(T), bufferData.data());
		}

		/**
		 * Set Viewports
		 */
		void SetViewport(const D3D12_VIEWPORT& viewport);
		void SetViewports(const std::vector<D3D12_VIEWPORT>& viewports);

		/**
		 * Set Scissor rects
		 */
		void SetScissorRect(const D3D12_RECT& scissorRect);
		void SetScissorRects(const std::vector<D3D12_RECT>& scissorRects);

		/**
		 * Set pipeline state object on the command list
		 */
		void SetPipelineState(const std::shared_ptr<CEPipelineStateObject>& pipelineState);

		/**
		 * Set the current root signature on the command list;
		 */
		void SetGraphicsRootSignature(const std::shared_ptr<CERootSignature>& rootSignature);
		void SetComputeRootSignature(const std::shared_ptr<CERootSignature>& rootSignature);

		/**
		 * Set inline CBV
		 *
		 * Note: Only ConstantBuffer can be used with inline CBV.
		 */
		void SetConstantBufferView(uint32_t rootParameterIndex, const std::shared_ptr<CEConstantBuffer>& buffer,
		                           D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		                           uint32_t bufferOffset = 0);

		/**
		 * Set inline SRV
		 *
		 * Note: Only Buffer resources can be used with SRV
		 */
		void SetShaderResourceView(uint32_t rootParameterIndex, const std::shared_ptr<CEBuffer>& buffer,
		                           D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
			                           D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		                           uint32_t bufferOffset = 0);

		/**
		 * Set inline UAV
		 *
		 * Note: Only Buffer resources can be used with inline UAV
		 */
		void SetUnorderedAccessView(uint32_t rootParameterIndex, const std::shared_ptr<CEBuffer>& buffer,
		                            D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		                            size_t bufferOffset = 0);

		/**
		 * Set CBV on the rendering pipeline
		 */
		void SetConstantBufferView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
		                           const std::shared_ptr<CEConstantBufferView>& cbv,
		                           D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

		/**
		 * Set SRV on the graphics pipeline
		 */
		void SetShaderResourceView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
		                           const std::shared_ptr<CEShaderResourceView>& srv,
		                           D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
			                           D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, UINT firstSubResource = 0,
		                           UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		/**
		 * Set an SRV on the graphics pipeline using the default SRV for the texture.
		 */
		void SetShaderResourceView(int32_t rootParameterIndex, uint32_t descriptorOffset,
		                           const std::shared_ptr<CETexture>& texture,
		                           D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
			                           D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		                           UINT firstSubResource = 0,
		                           UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		/**
		 * Set UAV on graphics pipeline.
		 */
		void SetUnorderedAccessView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
		                            const std::shared_ptr<CEUnorderedAccessView>& uav,
		                            D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		                            UINT firstSubResource = 0,
		                            UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		/**
		 * Set UAV on graphics pipeline using specific mip of texture
		 */
		void SetUnorderedAccessView(uint32_t rootParameterIndex, uint32_t descriptorOffset,
		                            const std::shared_ptr<CETexture>& texture, UINT mip,
		                            D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		                            UINT firstSubResource = 0,
		                            UINT numSubResources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

		/**
		 * Set render targets for graphics rendering pipeline
		 */
		void SetRenderTarget(const CERenderTarget& renderTarget);

		/**
		 * Draw geometry
		 */
		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t startVertex = 0,
		          uint32_t startInstance = 0);
		void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t startIndex = 0,
		                 int32_t baseVertex = 0, uint32_t startInstance = 0);

		/**
		 * Dispatch a compute shader.
		 */
		void Dispatch(uint32_t numGroupsX, uint32_t numGroupsY = 1, uint32_t numGroupsZ = 1);

	protected:
		friend class CECommandQueue;
		friend class CEDynamicDescriptorHeap;
		friend class std::default_delete<CECommandList>;

		CECommandList(CEDevice& device, D3D12_COMMAND_LIST_TYPE type);
		~CECommandList();

		/**
		 * Close command list
		 * Used by command queue.
		 *
		 * @param pendingCommandList, command list that is used to execute pending resource barriers (if any) fo command list.
		 *
		 * @return true if there are any pending resource barriers that need to be processed.
		 */
		bool Close(const std::shared_ptr<CECommandList>& pendingCommandList);
		/**
		 * Just close command list, useful for pending command lists.
		 */
		void Close();

		/**
		 * Reset command list, this should be called by Command Queue.
		 * Before command list is returned from CommandQueue::GetCommandList.
		 */
		void Reset();

		/**
		 * Release tracked object. Useful if swap chain need to be resized.
		 */
		void ReleaseTrackedObjects();

		/**
		 * Set currently bound descriptor heap.
		 * Should only be called by DynamicDescriptorHeap class.
		 */
		void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, ID3D12DescriptorHeap* heap);

		std::shared_ptr<CECommandList> GetGenerateMipsCommandList() const {
			return m_computeCommandList;
		}

	private:
		/**
		 * Use for procedural mesh generation
		 */
		using VertexCollection = std::vector<CEVertexPositionNormalTangentBitangentTexture>;
		using IndexCollection = std::vector<uint16_t>;

		/**
		 * Create a scene that contains single node with a single mesh
		 */
		std::shared_ptr<CEScene> CreateScene(const VertexCollection& vertices,
		                                     const IndexCollection& indices);

		/**
		 * Helper function for flipping winding of geometric primitives for LH vs. RH coordinates.
		 */
		inline void ReverseWinding(IndexCollection& indices, VertexCollection& vertices);

		/**
		 * Helper function for inverting normals for "inside" vs "outside" viewing.
		 */
		inline void CECommandList::InvertNormals(VertexCollection& vertices);

		/**
		 * Helper function to compute a point on a unit circle aligned to x,z plane and centered at the origin.
		 */
		inline ::DirectX::XMVECTOR GetCircleVector(size_t i, size_t tessellation) noexcept;

		/**
		 * Helper function to compute a tangent vector at a point on a unit sphere aligned to the x,z plane.
		 */
		inline ::DirectX::XMVECTOR GetCircleTangent(size_t i, size_t tessellation) noexcept;

		/**
		 * Helper creates a triangle fan to close end of a cylinder / cone
		 */
		void CreateCylinderCap(VertexCollection& vertices, IndexCollection& indices, size_t tessellation, float height,
		                       float radius, bool isTop);

		/**
		 * Add resource to list of tracked resources (ensure lifetime while command list is in use in command queue)
		 */
		void TrackResource(wrl::ComPtr<ID3D12Object> object);
		void TrackResource(const std::shared_ptr<CEResource>& res);

		/**
		 * Generate mips for UAV compatible textures;
		 */
		void GenerateMipsUAV(const std::shared_ptr<CETexture>& texture, bool isSRGB);

		/**
		 * Copy contents of CPU Buffer to a GPU Buffer (possibly replacing previous buffer contents)
		 */
		wrl::ComPtr<ID3D12Resource> CopyBuffer(size_t bufferSize, const void* bufferData,
		                                       D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

		/**
		 * Binds current descriptor heaps to the command list.
		 */
		void BindDescriptorHeaps();

		/**
		 * Device that is used to create command list;
		 */
		CEDevice& m_device;
		D3D12_COMMAND_LIST_TYPE m_commandListType;
		wrl::ComPtr<ID3D12GraphicsCommandList5> m_commandList;
		wrl::ComPtr<ID3D12CommandAllocator> m_commandAllocator;

		/**
		 * For copy queue, it may be necessary to generate mips while loading textures.
		 * Mips can't be generated on cpy queues but must be generated on compute or direct queues.
		 * In this case, a Compute command list is generated and executed
		 * after copy queue is finished uploading first sub resource.
		 */
		std::shared_ptr<CECommandList> m_computeCommandList;

		/**
		 * Keep track of currently bound root signatures to minimize root signature changes.
		 */
		ID3D12RootSignature* m_rootSignature;

		/**
		 * Keep track of current bound pipeline state object to minimize PSO changes.
		 */
		ID3D12PipelineState* m_pipelineState;

		/**
		 * Resource created in upload heap. Useful for drawing of dynamic geometry or
		 * for uploading constant buffer data that changes every draw call
		 */
		std::unique_ptr<CEUploadBuffer> m_uploadBuffer;

		/**
		 * Resource state tracker is used by command list to track (per command list)
		 * current state of a resource. Resource state tracker also tracks global state
		 * of a resource in order to minimize resource state transitions.
		 */
		std::unique_ptr<CEResourceStateTracker> m_resourceStateTracker;

		/**
		 * Dynamic descriptor heap allows for descriptors to be staged before being committed to the command list
		 * Dynamic descriptors need to be committed before a draw or dispatch.
		 */
		std::unique_ptr<CEDynamicDescriptorHeap> m_dynamicDescriptorHeap[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		/**
		 * Keep track of currently bound descriptor heaps.
		 * Only change descriptor heaps if they are different than currently bound descriptor heaps.
		 */
		ID3D12DescriptorHeap* m_descriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		/**
		 * Pipeline state object for mip map generation.
		 */
		std::unique_ptr<CEGenerateMipsPSO> m_generateMipsPSO;

		/**
		 * Pipeline state object for converting panorama (quirectangular) to cubemaps
		 */
		std::unique_ptr<CEPanoToCubemapPSO> m_panoToCubemapPSO;

		using TrackedObjects = std::vector<wrl::ComPtr<ID3D12Object>>;

		/**
		 * Objects that are being tracked by command list that is in use on command list and cannot be deleted.
		 * To ensure objects are not deleted until command list is finished executing, a reference to object in stored.
		 * Referenced objects are released when command list is reset.
		 */
		TrackedObjects m_trackedObjects;

		/**
		 * Keep track of loaded textures to avoid loading same texture multiple times.
		 */
		static std::map<std::wstring, ID3D12Resource*> m_textureCache;
		static std::mutex m_textureCacheMutex;
	};

	inline ::DirectX::XMVECTOR CECommandList::GetCircleVector(size_t i, size_t tessellation) noexcept {
		float angle = float(i) * ::DirectX::XM_2PI / float(tessellation);
		float dx, dz;

		::DirectX::XMScalarSinCos(&dx, &dz, angle);
		::DirectX::XMVECTORF32 v = {
			{
				{dx, 0, dz, 0}
			}
		};
		return v;
	}

	inline ::DirectX::XMVECTOR CECommandList::GetCircleTangent(size_t i, size_t tessellation) noexcept {
		float angle = (float(i) * ::DirectX::XM_2PI / float(tessellation)) + ::DirectX::XM_PIDIV2;
		float dx, dz;
		::DirectX::XMScalarSinCos(&dx, &dz, angle);
		::DirectX::XMVECTORF32 v = {
			{
				{dx, 0, dz, 0}
			}
		};
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
