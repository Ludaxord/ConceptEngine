#pragma once
#include "CEDirect3DGraphics.h"
#include "CENode.h"

class CEDirect3DNode : CENode<WORD> {
public:
	using super = CENode;
	CEDirect3DNode() = default;
	// CEDirect3DNode(const CEGraphics::CEVertexBuffer<WORD>& buffer);
	CEGraphics::CEVertexBuffer<WORD> GetBufferObject() override;

	std::vector<CEGraphics::CEVertex> GetVertices() override;
	std::vector<CEGraphics::CEIndex<WORD>> GetIndicies() override;

	CEDirect3DGraphics::CEVertexPosColor* GetD3DVertexPosColor();
	WORD* GetD3DIndicies();
	void CreateBufferObject(CEGraphics::CEVertexBuffer<unsigned short> buffer) override;
	void MoveBufferObject(CEGraphics::CEVertexBuffer<unsigned short> buffer) override;
};
