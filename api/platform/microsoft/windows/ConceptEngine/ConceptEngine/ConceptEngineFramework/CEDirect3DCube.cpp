#include "CEDirect3DCube.h"

CEDirect3DCube::CEDirect3DCube(): CEDirect3DNode() {
	const auto buffer = CEDirect3DGraphics::CED3DVertexBuffer();
	CEDirect3DNode::CreateBufferObject(buffer);
}

void CEDirect3DCube::CreateVertices(CEGraphics::CEVertexBuffer<WORD> buffer) {
	CEDirect3DGraphics::CEVertexPositionColor vertices[] = {
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)}, // 0
		{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)}, // 1
		{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)}, // 2
		{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)}, // 3
		{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)}, // 4
		{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f)}, // 5
		{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)}, // 6
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f)} // 7
	};
	static_cast<CEDirect3DGraphics::CED3DVertexBuffer>(buffer).CreateVertices(vertices);
}

void CEDirect3DCube::CreateIndicies(CEGraphics::CEVertexBuffer<WORD> buffer) {
	WORD ind[] = {
		0, 1, 2, 0, 2, 3,
		4, 6, 5, 4, 7, 6,
		4, 5, 1, 4, 1, 0,
		3, 2, 6, 3, 6, 7,
		1, 5, 6, 1, 6, 2,
		4, 0, 3, 4, 3, 7
	};
	static_cast<CEDirect3DGraphics::CED3DVertexBuffer>(buffer).CreateIndicies(ind);
}

CEDirect3DGraphics::CEVertexPositionColor* CEDirect3DCube::GetD3DVertexPosColor() {
	std::wstringstream wssx;
	wssx << "CEDirect3DNode::GetD3DVertexPosColor" << std::endl;
	OutputDebugStringW(wssx.str().c_str());
	return super::GetD3DVertexPosColor();
}

WORD* CEDirect3DCube::GetD3DIndicies() {
	return super::GetD3DIndicies();
}

CEGraphics::CEVertexBuffer<WORD> CEDirect3DCube::GetBufferObject() {
	return super::GetBufferObject();
}

std::vector<CEGraphics::CEVertex> CEDirect3DCube::GetVertices() {
	return super::GetVertices();
}

std::vector<CEGraphics::CEIndex<WORD>> CEDirect3DCube::GetIndicies() {
	return super::GetIndicies();
}
