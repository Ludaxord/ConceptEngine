#pragma once
#include "CEGraphics.h"

template <typename T>
class CENode {
public:
	enum class CENodeType {
		Clear,
		Cube
	};

	explicit ::CENode<T>::CENode(CEGraphics::CEVertexBuffer<T> buffer) {
		CENode<T>::CreateBufferObject(buffer);
	}

	CENode(const CENode&) = delete;
	virtual CENode& operator=(const CENode&) = delete;
	virtual ~CENode() = default;

	virtual void CreateVertices(CEGraphics::CEVertexBuffer<T> buffer);
	virtual std::vector<CEGraphics::CEVertex> GetVertices();

	virtual void CreateIndicies(CEGraphics::CEVertexBuffer<T> buffer);
	virtual std::vector<CEGraphics::CEIndex<T>> GetIndicies();

	virtual CEGraphics::CEVertexBuffer<T> GetBufferObject();
	void CreateBufferObject(CEGraphics::CEVertexBuffer<T> buffer);
	void MoveBufferObject(CEGraphics::CEVertexBuffer<T> buffer);

protected:
	std::unique_ptr<CEGraphics::CEVertexBuffer<T>> vertexBuffer;
};

template <typename T>
void CENode<T>::CreateBufferObject(CEGraphics::CEVertexBuffer<T> buffer) {
	CENode<T>::CreateVertices(buffer);
	CENode<T>::CreateIndicies(buffer);
	CENode<T>::MoveBufferObject(buffer);
}

template <typename T>
CEGraphics::CEVertexBuffer<T> CENode<T>::GetBufferObject() {
	return *vertexBuffer;
}

template <typename T>
void CENode<T>::MoveBufferObject(CEGraphics::CEVertexBuffer<T> buffer) {
	auto bufferPtr = std::make_unique<CEGraphics::CEVertexBuffer<T>>(buffer);
	vertexBuffer = std::move(bufferPtr);
}
