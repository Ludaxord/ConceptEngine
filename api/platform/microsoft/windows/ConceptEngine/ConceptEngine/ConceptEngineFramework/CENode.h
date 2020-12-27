#pragma once
#include "CEGraphics.h"

template <typename T>
class CENode : public std::enable_shared_from_this<CENode<T>> {
public:
	enum class CENodeType {
		Clear,
		Cube
	};

	CENode() : vertexBuffer(nullptr) {
	}

	// CENode(std::unique_ptr<CEGraphics::CEVertexBuffer<T>>& buffer) : vertexBuffer(buffer) {
	// }

	CENode(const CENode&) = delete;
	virtual CENode& operator=(const CENode&) = delete;
	virtual ~CENode() = default;

	virtual void CreateVertices(CEGraphics::CEVertexBuffer<T> buffer) = 0;
	virtual std::vector<CEGraphics::CEVertex> GetVertices() = 0;

	virtual void CreateIndicies(CEGraphics::CEVertexBuffer<T> buffer) = 0;
	virtual std::vector<CEGraphics::CEIndex<T>> GetIndicies() = 0;

	virtual CEGraphics::CEVertexBuffer<T> GetBufferObject() = 0;
	virtual void CreateBufferObject(CEGraphics::CEVertexBuffer<T> buffer) = 0;
	virtual void MoveBufferObject(CEGraphics::CEVertexBuffer<T> buffer) = 0;

protected:
	std::unique_ptr<CEGraphics::CEVertexBuffer<T>> vertexBuffer;

};
