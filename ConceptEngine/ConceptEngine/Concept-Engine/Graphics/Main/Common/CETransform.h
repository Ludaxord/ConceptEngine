#pragma once
namespace ConceptEngine::Graphics::Main::Common {
	class CETransform {
	public:
		CETransform();
		~CETransform() = default;

		virtual void SetTranslation(float x, float y, float z) = 0;

		virtual void SetScale(float x, float y, float z) = 0;

		void SetUniformScale(float scale) {
			SetScale(scale, scale, scale);
		}

		virtual void SetRotation(float x, float y, float z) = 0;

	protected:
	};
}
