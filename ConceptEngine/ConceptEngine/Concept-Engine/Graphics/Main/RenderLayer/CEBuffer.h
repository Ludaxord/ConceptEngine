#pragma once
#include "CEResource.h"
#include "../../../Core/Common/CETypes.h"

namespace ConceptEngine::Graphics::Main::RenderLayer {
	enum class CEIndexFormat {
		Unknown = 0,
		uint16 = 1,
		uint32 = 2
	};

	enum CEBufferFlags : uint32 {
		BufferFlag_None = 0,
		BufferFlag_Default = FLAG(1),
		BufferFlag_Upload = FLAG(2),
		BufferFlag_UAV = FLAG(3),
		BufferFlag_SRV = FLAG(4)
	};

	inline CEIndexFormat GetIndexFormatFromStride(uint32 StrideInBytes) {
		if (StrideInBytes == 2) {
			return CEIndexFormat::uint16;
		}
		else if (StrideInBytes == 4) {
			return CEIndexFormat::uint32;
		}
		else {
			return CEIndexFormat::Unknown;
		}
	}

	inline uint32 GetStrideFromIndexFormat(CEIndexFormat IndexFormat) {
		if (IndexFormat == CEIndexFormat::uint16) {
			return 2;
		}
		else if (IndexFormat == CEIndexFormat::uint32) {
			return 4;
		}
		else {
			return 0;
		}
	}

	class CEBuffer : public CEResource {
	public:
		CEBuffer(uint32 flags) : CEResource(), Flags(flags) {

		}

		virtual class CEVertexBuffer* AsVertexBuffer() {
			return nullptr;
		}

		virtual class CEIndexBuffer* AsIndexBuffer() {
			return nullptr;
		}

		virtual class CEConstantBuffer* AsConstantBuffer() {
			return nullptr;
		}

		virtual class CEStructuredBuffer* AsStructuredBuffer() {
			return nullptr;
		}

		virtual void* Map(uint32 offset, uint32 size) = 0;
		virtual void Unmap(uint32 offset, uint32 size) = 0;

		uint32 GetFlags() const {
			return Flags;
		}

		bool IsUpload() const {
			return (Flags & BufferFlag_Upload);
		}

		bool IsUAV() const {
			return (Flags & BufferFlag_UAV);
		}

		bool IsSRV() const {
			return (Flags & BufferFlag_SRV);
		}

	private:
		uint32 Flags;
	};

	class CEVertexBuffer : public CEBuffer {
	public:
		CEVertexBuffer(uint32 numVertices, uint32 stride, uint32 flags) : CEBuffer(flags),
		                                                                  NumVertices(numVertices),
		                                                                  Stride(stride) {

		}

		virtual CEVertexBuffer* AsVertexBuffer() override {
			return this;
		}

		uint32 GetStride() const {
			return Stride;
		}

		uint32 GetNumVertices() const {
			return NumVertices;
		}

	private:
		uint32 NumVertices;
		uint32 Stride;
	};

	class CEIndexBuffer : public CEBuffer {
	public:
		CEIndexBuffer(CEIndexFormat format, uint32 numIndices, uint32 flags) : CEBuffer(flags), Format(format),
		                                                                       NumIndices(numIndices) {

		}

		virtual CEIndexBuffer* AsIndexBuffer() override {
			return this;
		}

		CEIndexFormat GetFormat() const {
			return Format;
		}

		uint32 GetNumIndices() const {
			return NumIndices;
		}

	private:
		CEIndexFormat Format;
		uint32 NumIndices;
	};

	class CEConstantBuffer : public CEBuffer {
	public:
		CEConstantBuffer(uint32 size, uint32 flags) : CEBuffer(flags), Size(size) {

		}

		virtual CEConstantBuffer* AsConstantBuffer() override {
			return this;
		}

		uint32 GetSize() const {
			return Size;
		}

	private:
		uint32 Size;
	};

	class CEStructuredBuffer : public CEBuffer {
	public:
		CEStructuredBuffer(uint32 numElements, uint32 stride, uint32 flags) : CEBuffer(flags), Stride(stride),
		                                                                      NumElements(numElements) {

		}

		virtual CEStructuredBuffer* AsStructuredBuffer() override {
			return this;
		}

		uint32 GetStride() const {
			return Stride;
		}

		uint32 GetNumElements() const {
			return NumElements;
		}

	private:
		uint32 Stride;
		uint32 NumElements;
	};
}
