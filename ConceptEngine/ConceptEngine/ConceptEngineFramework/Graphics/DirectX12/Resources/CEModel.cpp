#include "CEModel.h"

using namespace ConceptEngineFramework::Graphics::DirectX12::Resources;

CEKeyFrame::CEKeyFrame() {
}

CEKeyFrame::~CEKeyFrame() {
}

float CEAnimation::GetStartTime() const {
	return 0;
}

float CEAnimation::GetEndTime() const {
	return 0;
}

void CEAnimation::Interpolate(float t, DirectX::XMFLOAT4X4& M) const {
}

float CEAnimationClip::GetClipStartTime() const {
	return 0;
}

float CEAnimationClip::GetClipEndTime() const {
	return 0;
}

void CEAnimationClip::Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& transforms) const {
}

UINT CEModelData::ElementCount() const {
	return 0;
}

float CEModelData::GetClipStartTime(const std::string& clipName) const {
	return 0;
}

float CEModelData::GetClipEndTime(const std::string& clipName) const {
	return 0;
}

void CEModelData::Set(std::vector<int>& elementHierarchy, std::vector<DirectX::XMFLOAT4X4>& elementOffset,
	std::unordered_map<std::string, CEAnimationClip> animations) {
}

void CEModelData::GetFinalTransform(const std::string& clipName, float timePos,
	std::vector<DirectX::XMFLOAT4X4>& finalTransforms) const {
}
