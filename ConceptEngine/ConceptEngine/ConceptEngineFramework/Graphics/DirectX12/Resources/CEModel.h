#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	/*
	 * A CEKeyFrame defines transformation at an instance in time
	 */
	struct CEKeyFrame {

	};

	/*
	 * CEAnimation is defined by a list of keyframes. For time values in between two keyframes, we interpolate between the two nearest keyframes that bound the time.
	 *
	 * We assume an animation always has two keyframes
	 */
	struct CEAnimation {

	};

	/*
	 * Examples of AnimationClips are "walk", "run", "attack", "defend"
	 * An CEAnimationClip requires a CEAnimation for every element to form animation clip.
	 */
	struct CEAnimationClip {

	};

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
