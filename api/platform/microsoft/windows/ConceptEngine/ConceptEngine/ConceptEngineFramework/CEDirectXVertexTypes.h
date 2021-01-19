#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

namespace ConceptEngine::GraphicsEngine::Direct3D12 {
	namespace wrl = Microsoft::WRL;

	struct CEDirectXVertexPosition {
		CEDirectXVertexPosition() = default;

		explicit CEDirectXVertexPosition(const ::DirectX::XMFLOAT3& position) : Position(position) {
		}

		explicit CEDirectXVertexPosition(::DirectX::FXMVECTOR position) {
			::DirectX::XMStoreFloat3(&(this->Position), position);
		}

		::DirectX::XMFLOAT3 Position;

		static const D3D12_INPUT_LAYOUT_DESC InputLayout;
	private:
		static const int InputElementCount = 1;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	struct CEDirectXVertexPositionNormalTangentBitangentTexture {
		CEDirectXVertexPositionNormalTangentBitangentTexture() = default;

		explicit CEDirectXVertexPositionNormalTangentBitangentTexture(
			const ::DirectX::XMFLOAT3& position,
			const ::DirectX::XMFLOAT3& normal,
			const ::DirectX::XMFLOAT3& texCoord,
			const ::DirectX::XMFLOAT3& tangent = {0, 0, 0},
			const ::DirectX::XMFLOAT3& bitangent = {0, 0, 0}
		): Position(position),
		   Normal(normal),
		   Tangent(texCoord),
		   Bitangent(tangent),
		   TexCoord(bitangent) {
		}

		explicit CEDirectXVertexPositionNormalTangentBitangentTexture(
			::DirectX::FXMVECTOR position,
			::DirectX::FXMVECTOR normal,
			::DirectX::FXMVECTOR texCoord,
			::DirectX::GXMVECTOR tangent = {0, 0, 0, 0},
			::DirectX::HXMVECTOR bitangent = {0, 0, 0, 0}
		) {
			::DirectX::XMStoreFloat3(&(this->Position), position);
			::DirectX::XMStoreFloat3(&(this->Normal), normal);
			::DirectX::XMStoreFloat3(&(this->TexCoord), texCoord);
			::DirectX::XMStoreFloat3(&(this->Tangent), tangent);
			::DirectX::XMStoreFloat3(&(this->Bitangent), bitangent);
		}

		::DirectX::XMFLOAT3 Position;
		::DirectX::XMFLOAT3 Normal;
		::DirectX::XMFLOAT3 TexCoord;
		::DirectX::XMFLOAT3 Tangent;
		::DirectX::XMFLOAT3 Bitangent;

		static const D3D12_INPUT_LAYOUT_DESC InputLayout;
	private:
		static const int InputElementCount = 5;
		static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};
}
