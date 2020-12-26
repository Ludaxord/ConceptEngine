#pragma once
#include "CEDirect3DGraphics.h"
#include "CEDirect3DNode.h"

class CEDirect3DCube : CEDirect3DNode {

public:
	using super = CEDirect3DNode;
	CEDirect3DCube();
	void CreateVertices(CEGraphics::CEVertexBuffer<WORD> buffer) override;
	void CreateIndicies(CEGraphics::CEVertexBuffer<WORD> buffer) override;
};
