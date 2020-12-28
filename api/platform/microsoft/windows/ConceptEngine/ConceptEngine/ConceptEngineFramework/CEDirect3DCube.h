#pragma once
#include "CEDirect3DGraphics.h"
#include "CEDirect3DNode.h"

class CEDirect3DCube : CEDirect3DNode {

public:
	using super = CEDirect3DNode;
	CEDirect3DCube();
	void CreateVertices(CEGraphics::CEVertexBuffer<WORD> buffer) override;
	void CreateIndicies(CEGraphics::CEVertexBuffer<WORD> buffer) override;
	CEDirect3DGraphics::CEVertexPosColor* GetD3DVertexPosColor() override;
	WORD* GetD3DIndicies() override;
	CEGraphics::CEVertexBuffer<WORD> GetBufferObject() override;
	std::vector<CEGraphics::CEVertex> GetVertices() override;
	std::vector<CEGraphics::CEIndex<WORD>> GetIndicies() override;

public:
	CEDirect3DGraphics::CEVertexPosColor g_Vertices[8] = {
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)}, // 0
		{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)}, // 1
		{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)}, // 2
		{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)}, // 3
		{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)}, // 4
		{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f)}, // 5
		{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)}, // 6
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f)} // 7
	};
	WORD g_Indicies[36] = {
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		4, 5, 1, 4, 1, 0,
		3, 2, 6, 3, 6, 7,
		1, 5, 6, 1, 6, 2,
		4, 0, 3, 4, 3, 7
	};

};
