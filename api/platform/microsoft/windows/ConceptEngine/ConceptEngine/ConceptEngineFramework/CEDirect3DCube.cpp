#include "CEDirect3DCube.h"

CEDirect3DCube::CEDirect3DCube(): super(CEDirect3DGraphics::CED3DVertexBuffer()) {
}

void CEDirect3DCube::CreateVertices(CEGraphics::CEVertexBuffer<WORD> buffer) {
	buffer.vertices = {};
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
