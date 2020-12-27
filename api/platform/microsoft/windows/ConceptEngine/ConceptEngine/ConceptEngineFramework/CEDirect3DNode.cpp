#include "CEDirect3DNode.h"

// CEDirect3DNode::CEDirect3DNode(const CEGraphics::CEVertexBuffer<WORD>& buffer) : CENode<WORD>(
// 	std::make_unique<CEGraphics::CEVertexBuffer<WORD>>(buffer)) {
// }

CEGraphics::CEVertexBuffer<WORD> CEDirect3DNode::GetBufferObject() {
	return *vertexBuffer;
}

std::vector<CEGraphics::CEVertex> CEDirect3DNode::GetVertices() {
	return GetBufferObject().vertices;
}

std::vector<CEGraphics::CEIndex<WORD>> CEDirect3DNode::GetIndicies() {
	return GetBufferObject().indices;
}

CEDirect3DGraphics::CEVertexPosColor* CEDirect3DNode::GetD3DVertexPosColor() {
	auto v = GetVertices();
	const auto vertex = &v[0];
	CEDirect3DGraphics::CEVertexPosColor* vertexPosColor = {};
	for (auto i = 0; i < v.size(); i++)
		vertexPosColor[i] = vertex[i];
	return vertexPosColor;
}


WORD* CEDirect3DNode::GetD3DIndicies() {
	auto indx = GetIndicies();
	const auto index = &indx[0];
	WORD* indicies = {};
	for (auto i = 0; i < indx.size(); i++)
		indicies[i] = index[i].i;
	return indicies;
}

void CEDirect3DNode::CreateBufferObject(CEGraphics::CEVertexBuffer<unsigned short> buffer) {
	CreateVertices(buffer);
	CreateIndicies(buffer);
	MoveBufferObject(buffer);
}

void CEDirect3DNode::MoveBufferObject(CEGraphics::CEVertexBuffer<unsigned short> buffer) {
	auto bufferPtr = std::make_unique<CEGraphics::CEVertexBuffer<unsigned short>>(buffer);
	vertexBuffer = std::move(bufferPtr);
}
