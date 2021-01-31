#include "CEVertexTypes.h"
using namespace Concept::GraphicsEngine::Direct3D;

// clang-format off
const D3D12_INPUT_ELEMENT_DESC CEVertexPosition::InputElements[] = {
	{
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	}
};

const D3D12_INPUT_LAYOUT_DESC CEVertexPosition::InputLayout = {
	CEVertexPosition::InputElements,
	CEVertexPosition::InputElementCount
};

const D3D12_INPUT_ELEMENT_DESC CEVertexPositionNormalTangentBitangentTexture::InputElements[] = {
	{
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{
		"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{
		"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{
		"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
	{
		"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	},
};

const D3D12_INPUT_LAYOUT_DESC CEVertexPositionNormalTangentBitangentTexture::InputLayout = {
	CEVertexPositionNormalTangentBitangentTexture::InputElements,
	CEVertexPositionNormalTangentBitangentTexture::InputElementCount
};
// clang-format on
