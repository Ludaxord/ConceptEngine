#pragma once
#include <cstdint>
#include <string>


#include "../CEEvents.h"
#include "../CEGame.h"

namespace ConceptEngineFramework::Game {
	class CEWindow;

	namespace Playgrounds {
		class CEPlayground {
		public:
			CEPlayground(const std::wstring& name, uint32_t width, uint32_t height, bool vSync = false) :
				m_forward(0),
				m_backward(0),
				m_left(0),
				m_right(0),
				m_up(0),
				m_down(0),
				m_pitch(0),
				m_yaw(0),
				m_animateLights(false),
				m_shift(false),
				m_width(width),
				m_height(height),
				m_vSync(vSync) {
				m_console = CEGame::Get().GetConsole();
				m_window = CEGame::Get().CreateWindow(name, width, height);

				// m_window->Update += UpdateEvent::slot(&CEPlayground::OnUpdate, this);
				// m_window->KeyPressed += KeyboardEvent::slot(&CEPlayground::OnKeyPressed, this);
				// m_window->KeyReleased += KeyboardEvent::slot(&CEPlayground::OnKeyReleased, this);
				// m_window->MouseMoved += MouseMotionEvent::slot(&CEPlayground::OnMouseMoved, this);
				// m_window->MouseWheel += MouseWheelEvent::slot(&CEPlayground::OnMouseWheel, this);
				// m_window->Resize += ResizeEvent::slot(&CEPlayground::OnResize, this);
				// m_window->DPIScaleChanged += DPIScaleEvent::slot(&CEPlayground::OnDPIScaleChanged, this);
			};
			virtual ~CEPlayground() = default;

			/*
			 * Start game loop and return error code if occurs
			 */
			uint32_t Run() {
				LoadContent();

				m_console->GetLogger()->info("ConceptFramework Engine Playground Content Loaded");

				// m_window->Show();

				// m_console->GetLogger()->info("ConceptFramework Engine Window Presented");

				const uint32_t returnCode = CEGame::Get().Run();
				UnloadContent();
				return returnCode;
			}

			/*
			 * Load content required for playground
			 */
			virtual bool LoadContent() = 0;

			/*
			 * UnLoad content loaded in LoadContent
			 */
			virtual void UnloadContent() = 0;
		protected:
			/*
			 * Update game logic
			 */
			virtual void OnUpdate(UpdateEventArgs& e) = 0;
			virtual void OnRender() = 0;

			/*
			 * Invoked by registered window when key is pressed
			 * while window has focus.
			 */
			virtual void OnKeyPressed(KeyEventArgs& e) = 0;

			/*
			 * Invoked when key on keyboard is released
			 */
			virtual void OnKeyReleased(KeyEventArgs& e) = 0;

			/*
			 * Invoked when mouse is moved over registered window
			 */
			virtual void OnMouseMoved(MouseMotionEventArgs& e) = 0;

			/*
			 * Invoked when mouse wheel is scrolled while registered window has focus
			 */
			virtual void OnMouseWheel(MouseWheelEventArgs& e) = 0;

			/*
			 * Invoked when window is resized
			 */
			virtual void OnResize(ResizeEventArgs& e) = 0;

			virtual void OnDPIScaleChanged(DPIScaleEventArgs& e) = 0;

			void DisplayDebugFPSOnUpdate(UpdateEventArgs& e) const {
				static uint64_t frameCount = 0;
				static double totalTime = 0.0;

				totalTime += e.DeltaTime;
				++frameCount;

				if (totalTime > 1.0) {
					g_FPS = frameCount / totalTime;
					frameCount = 0;
					totalTime = 0.0;

					m_console->GetLogger()->info("FPS: {:.7}", g_FPS);

					wchar_t buffer[256];
					::swprintf_s(buffer, L"ConceptEngine [FPS: %f]", g_FPS);
					// m_window->SetWindowTitle(buffer);
				}
			}

			/*
			 * Setters
			 */
			void SetCameraForward(float forward) {
				m_forward = forward;
			}

			void SetCameraBackward(float backward) {
				m_backward = backward;
			}

			void SetCameraLeft(float left) {
				m_left = left;
			}

			void SetCameraRight(float right) {
				m_right = right;
			}

			void SetCameraUp(float up) {
				m_up = up;
			}

			void SetCameraDown(float down) {
				m_down = down;
			}

			void SetPitch(float pitch) {
				m_pitch = pitch;
			}

			void SetYaw(float yaw) {
				m_yaw = yaw;
			}

			void SetAnimateLights(bool animateLights) {
				m_animateLights = animateLights;
			}

			void SetShift(bool shift) {
				m_shift = shift;
			}

			void SetScreenResolution(int width, int height) {
				m_width = width;
				m_height = height;
			}

			void SetVSync(bool vSync) {
				m_vSync = vSync;
			}

			/*
			 * Getters
			 */
			float GetCameraForward() const {
				return m_forward;
			}

			float GetCameraBackward() const {
				return m_backward;
			}

			float GetCameraLeft() const {
				return m_left;
			}

			float GetCameraRight() const {
				return m_right;
			}

			float GetCameraUp() const {
				return m_up;
			}

			float GetCameraDown() const {
				return m_down;
			}

			float GetPitch() const {
				return m_pitch;
			}

			float GetYaw() const {
				return m_yaw;
			}

			bool GetAnimateLights() const {
				return m_animateLights;
			}

			bool GetShift() const {
				return m_shift;
			}

			int GetScreenWidth() const {
				return m_width;
			}

			int GetScreenHeight() const {
				return m_height;
			}

			bool GetVSync() const {
				return m_vSync;
			}

			std::shared_ptr<CEWindow> m_window;

			/*
			 * Logger for logging messages
			 */
			std::shared_ptr<CEConsole> m_console;

			/*
			* Camera controller
			*/
			float m_forward;
			float m_backward;
			float m_left;
			float m_right;
			float m_up;
			float m_down;

			float m_pitch;
			float m_yaw;

			/*
			 * Rotate lights in a circle
			 */
			bool m_animateLights;

			/*
			 * Set trie if shift key is pressed
			 */
			bool m_shift;

			int m_width;
			int m_height;
			bool m_vSync;

			inline static double g_FPS = 0.0;
		private:
		};
	}
}
