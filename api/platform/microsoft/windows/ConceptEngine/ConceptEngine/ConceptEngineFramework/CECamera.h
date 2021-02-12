#pragma once
namespace ConceptFramework::GraphicsEngine {
	enum class Space {
		Local,
		World
	};

	class CECamera {
	public:
		CECamera(): m_viewDirty(true), m_inverseViewDirty(true), m_projectionDirty(true),
		            m_inverseProjectionDirty(true), m_vFoV(45.0f), m_aspectRatio(1.0f), m_zNear(0.1f), m_zFar(100.0f) {
		}

		virtual ~CECamera() {
		}

		/*
		 * Set camera to perspective projection matrix.
		 * @param fovy, vertical field of vie4w in degrees
		 * @param aspect, aspect ratio of screen
		 * @param zNear, distance to near clipping plane
		 * @param zFar, distance to far clipping plane
		 */
		void set_Projection(float fovy, float aspect, float zNear, float zFar) {
			m_vFoV = fovy;
			m_aspectRatio = aspect;
			m_zNear = zNear;
			m_zFar = zFar;

			m_projectionDirty = true;
			m_inverseProjectionDirty = true;
		}
		
		/*
		 * Set field of view in degrees
		 */
		void Set_FoV(float fovy) {
			if (m_vFoV != fovy) {
				m_vFoV = fovy;
				m_projectionDirty = true;
				m_inverseProjectionDirty = true;
			}
		}

		/*
		 * Get field of view in degrees
		 */
		float get_FoV() const {
			return m_vFoV;
		}

	protected:
		virtual void UpdateViewMatrix() const = 0;
		virtual void UpdateInverseViewMatrix() const = 0;
		virtual void UpdateProjectionMatrix() const = 0;
		virtual void UpdateInverseProjectionMatrix() const = 0;

		/*
		 * Projection parameters
		 */
		/*
		 * Vertical field of view
		 */
		float m_vFoV;
		/*
		 * Aspect ratio
		 */
		float m_aspectRatio;
		/*
		 * Near clip distance;
		 */
		float m_zNear;
		/*
		 * far clip distance
		 */
		float m_zFar;

		/*
		 * if variables are true, view matrix needs to be updated
		 */
		mutable bool m_viewDirty, m_inverseViewDirty;
		/*
		 * if variables are true projection matrix needs to be updated
		 */
		mutable bool m_projectionDirty, m_inverseProjectionDirty;

	private:

	};
}
