#pragma once
#include "CEDirect3DGraphics.h"
#include "CENode.h"

class CEDirect3DNode : CENode<WORD> {
public:
	using super = CENode;
	explicit CEDirect3DNode(const CEGraphics::CEVertexBuffer<WORD>& buffer);
	virtual CEGraphics::CEVertexBuffer<WORD> GetBufferObject() override;

	std::vector<CEGraphics::CEVertex> GetVertices() override;
	std::vector<CEGraphics::CEIndex<WORD>> GetIndicies() override;

	CEDirect3DGraphics::CEVertexPosColor* GetD3DVertexPosColor();
	WORD* GetD3DIndicies();
};
