#pragma once
#include <DirectXMath.h>
#include <intsafe.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace ConceptEngineFramework::Graphics::DirectX12::Resources {
	/*
	 * A CEKeyFrame defines transformation at an instance in time
	 */
	struct CEKeyFrame {
		CEKeyFrame();
		~CEKeyFrame();

		float TimePos;
		DirectX::XMFLOAT3 Translation;
		DirectX::XMFLOAT3 Scale;
		DirectX::XMFLOAT4 Rotation;
	};

	/*
	 * CEAnimation is defined by a list of keyframes. For time values in between two keyframes, we interpolate between the two nearest keyframes that bound the time.
	 *
	 * We assume an animation always has two keyframes
	 */
	struct CEAnimation {
		float GetStartTime() const;
		float GetEndTime() const;

		void Interpolate(float t, DirectX::XMFLOAT4X4& M) const;

		std::vector<CEKeyFrame> KeyFrames;
	};

	/*
	 * Examples of AnimationClips are "walk", "run", "attack", "defend"
	 * An CEAnimationClip requires a CEAnimation for every element to form animation clip.
	 */
	struct CEAnimationClip {
		float GetClipStartTime() const;
		float GetClipEndTime() const;

		void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& transforms) const;
		std::vector<CEAnimation> Animations;
	};

	class CEModelData {
	public:
		UINT ElementCount() const;

		float GetClipStartTime(const std::string& clipName) const;
		float GetClipEndTime(const std::string& clipName) const;

		void Set(std::vector<int>& elementHierarchy,
		         std::vector<DirectX::XMFLOAT4X4>& elementOffset,
		         std::unordered_map<std::string, CEAnimationClip> animations);

		/*
		 * later cache result if there was a chance that it was called several times with same clipName at same timePos
		 */
		void GetFinalTransform(const std::string& clipName,
		                       float timePos,
		                       std::vector<DirectX::XMFLOAT4X4>& finalTransforms) const;
	protected:
	private:
		//Gives parentIndex of ith element
		std::vector<int> m_elementHierarchy;
		std::vector<DirectX::XMFLOAT4X4> m_elementOffset;
		std::unordered_map<std::string, CEAnimationClip> m_animations;
	};

	struct CEModel {
		/*
		 * Called every frame and increments the time position, interpolates,
		 * the animations for each element based on current animation clip, and generates final transforms which are ultimately set to effect for processing in vertex shader
		 */
		virtual void UpdateAnimation(float dt) = 0;

		CEModelData* GetModelInfo() {
			return ModelInfo;
		}

		float GetTimePos() {
			return TimePos;
		}

		std::string GetClipName() {
			return ClipName;
		}

		std::vector<DirectX::XMFLOAT4X4> GetFinalTransforms() {
			return FinalTransforms;
		}

		CEModelData* ModelInfo = nullptr;
		std::vector<DirectX::XMFLOAT4X4> FinalTransforms;
		std::string ClipName;
		float TimePos = 0.0f;

	};
}
