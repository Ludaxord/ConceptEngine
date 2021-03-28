#include "CEModel.h"

#include "CETypes.h"
#include "../CEDX12Manager.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CEKeyFrame::CEKeyFrame(): TimePos(0.0f),
                          Translation(0.0f, 0.0f, 0.0f),
                          Scale(1.0f, 1.0f, 1.0f),
                          Rotation(0.0f, 0.0f, 0.0f, 1.0f) {
}

CEKeyFrame::~CEKeyFrame() {
}

float CEAnimation::GetStartTime() const {
	//KeyFrames are sorted by time, so first keyframe gives start time.
	return KeyFrames.front().TimePos;
}

float CEAnimation::GetEndTime() const {
	//Keyframes are sorted by time, so last keyframe gives end time.
	float f = KeyFrames.back().TimePos;
	return f;
}

void CEAnimation::Interpolate(float t, DirectX::XMFLOAT4X4& M) const {
	if (t <= KeyFrames.front().TimePos) {
		XMVECTOR S = XMLoadFloat3(&KeyFrames.front().Scale);
		XMVECTOR T = XMLoadFloat3(&KeyFrames.front().Translation);
		XMVECTOR R = XMLoadFloat4(&KeyFrames.front().Rotation);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, R, T));
	}
	else if (t >= KeyFrames.back().TimePos) {
		XMVECTOR S = XMLoadFloat3(&KeyFrames.back().Scale);
		XMVECTOR T = XMLoadFloat3(&KeyFrames.back().Translation);
		XMVECTOR R = XMLoadFloat4(&KeyFrames.back().Rotation);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, R, T));
	}
	else {
		for (UINT i = 0; i < KeyFrames.size() - 1; ++i) {
			if (t >= KeyFrames[i].TimePos && t <= KeyFrames[i + 1].TimePos) {
				float lerpPercent = (t - KeyFrames[i].TimePos) / (KeyFrames[i + 1].TimePos - KeyFrames[i].TimePos);

				XMVECTOR s0 = XMLoadFloat3(&KeyFrames[i].Scale);
				XMVECTOR s1 = XMLoadFloat3(&KeyFrames[i + 1].Scale);

				XMVECTOR t0 = XMLoadFloat3(&KeyFrames[i].Translation);
				XMVECTOR t1 = XMLoadFloat3(&KeyFrames[i + 1].Translation);

				XMVECTOR r0 = XMLoadFloat4(&KeyFrames[i].Rotation);
				XMVECTOR r1 = XMLoadFloat4(&KeyFrames[i + 1].Rotation);

				XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
				XMVECTOR T = XMVectorLerp(t0, t1, lerpPercent);
				XMVECTOR R = XMQuaternionSlerp(r0, r1, lerpPercent);

				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, R, T));

				break;
			}
		}
	}
}

float CEAnimationClip::GetClipStartTime() const {
	//Find smallest start time over all elements in clip
	float t = Resources::Infinity;
	for (UINT i = 0; i < Animations.size(); ++i) {
		t = CEDX12Manager::Min(t, Animations[i].GetStartTime());
	}

	return t;
}

float CEAnimationClip::GetClipEndTime() const {
	//Find largets end time over all elements in clip
	float t = 0.0f;
	for (UINT i = 0; i < Animations.size(); ++i) {
		t = CEDX12Manager::Max(t, Animations[i].GetEndTime());
	}

	return t;
}

void CEAnimationClip::Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& transforms) const {
	for (UINT i = 0; i < Animations.size(); ++i) {
		Animations[i].Interpolate(t, transforms[i]);
	}
}

UINT CEModelData::ElementCount() const {
	return m_elementHierarchy.size();
}

float CEModelData::GetClipStartTime(const std::string& clipName) const {
	auto clip = m_animations.find(clipName);
	return clip->second.GetClipStartTime();
}

float CEModelData::GetClipEndTime(const std::string& clipName) const {
	auto clip = m_animations.find(clipName);
	return clip->second.GetClipEndTime();
}

void CEModelData::Set(std::vector<int>& elementHierarchy,
                      std::vector<DirectX::XMFLOAT4X4>& elementOffset,
                      std::unordered_map<std::string, CEAnimationClip> animations) {
	m_elementHierarchy = elementHierarchy;
	m_elementOffset = elementOffset;
	m_animations = animations;
}

void CEModelData::GetFinalTransform(const std::string& clipName,
                                    float timePos,
                                    std::vector<DirectX::XMFLOAT4X4>& finalTransforms) const {
	UINT numElements = m_elementOffset.size();

	std::vector<XMFLOAT4X4> toParentTransforms(numElements);

	//Interpolate all elements of clip at given time instance.
	auto clip = m_animations.find(clipName);
	clip->second.Interpolate(timePos, toParentTransforms);

	/*
	 * Traverse hierarchy and transform all elements to the root space.
	 */
	std::vector<XMFLOAT4X4> toRootTransforms(numElements);

	/*
	 * Root element has index 0. Root element has no parent, so its toRootTransform
	 * is just its local element transform
	 */
	toRootTransforms[0] = toParentTransforms[0];

	//Now find toRootTransform of children
	for (UINT i = 1; i < numElements; ++i) {
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		int parentIndex = m_elementHierarchy[i];
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	//Remultiply by bone offset transform to get final transform.
	for (UINT i = 0; i < numElements; ++i) {
		XMMATRIX offset = XMLoadFloat4x4(&m_elementOffset[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMMATRIX finalTransform = XMMatrixMultiply(offset, toRoot);
		XMStoreFloat4x4(&finalTransforms[i], XMMatrixTranspose(finalTransform));
	}
}
