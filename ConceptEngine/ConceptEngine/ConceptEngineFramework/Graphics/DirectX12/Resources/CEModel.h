#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	class CEModelData {

	};

	struct CEModel {
	public:
		virtual void UpdateAnimation(float dt) = 0;

		CEModelData* GetModelInfo() {
			return m_modelInfo;
		}

		float GetTimePos() {
			return m_timePos;
		}

		std::string GetClipName() {
			return m_clipName;
		}

		std::vector<DirectX::XMFLOAT4X4> GetFinalTransforms() {
			return m_finalTransforms;
		}

	protected:
	private:
		CEModelData* m_modelInfo = nullptr;
		std::vector<DirectX::XMFLOAT4X4> m_finalTransforms;
		std::string m_clipName;
		float m_timePos = 0.0f;
	};
}
