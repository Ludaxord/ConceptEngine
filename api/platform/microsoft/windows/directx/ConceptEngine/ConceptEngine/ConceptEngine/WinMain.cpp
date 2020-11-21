#include <ostream>
#include <sstream>


#include "CEConverters.h"
#include "CEWindow.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	try {

		CEWindow ceWindow(800, 300, "Concept Engine Editor", CEWindow::main);
		CEWindow ceWindowAdditional(300, 800, "Concept Engine Tools", CEWindow::additional);

		MSG msg;
		BOOL result;

		//mouse wheel test
		int wheel_count = 0;

		
		while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0) {
			//TranslateMessage used to use passed input ex. Values of key press on keyboard. That Used WM_CHAR from MSG object.
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			//Keyboard test
			if (ceWindow.keyboard.IsKeyPressed(VK_SPACE)) {
				MessageBox(
					nullptr,
					CEConverters::convertCharArrayToLPCWSTR("Space pressed!"),
					CEConverters::convertCharArrayToLPCWSTR("Space was pressed in main window"),
					MB_OK | MB_ICONEXCLAMATION
				);
			}

			//Mouse test
			while (!ceWindow.mouse.IsEmpty()) {
				const auto e = ceWindow.mouse.Read();
				switch (e.GetType()) {
				case CEMouse::CEMouseEvent::CEMouseEventType::leave: {
					std::ostringstream oss;
					oss << "Mouse Leave Window" << std::endl;
					OutputDebugString(CEConverters::convertCharArrayToLPCWSTR(oss.str().c_str()));
				}
				break;
				case CEMouse::CEMouseEvent::CEMouseEventType::move: {
					std::ostringstream oss;
					oss << "Mouse Position: (x: " << e.GetMousePositionX() << ", y: " << e.GetMousePositionY() << ")"
						<< std::endl;
					OutputDebugString(CEConverters::convertCharArrayToLPCWSTR(oss.str().c_str()));
				}
				break;
				case CEMouse::CEMouseEvent::CEMouseEventType::wheelUp: {
					wheel_count++;
					std::ostringstream oss;
					oss << "Wheel: (" << wheel_count << ")" << std::endl;
					OutputDebugString(CEConverters::convertCharArrayToLPCWSTR(oss.str().c_str()));
				}
				case CEMouse::CEMouseEvent::CEMouseEventType::wheelDown: {
					wheel_count--;
					std::ostringstream oss;
					oss << "Wheel: (" << wheel_count << ")" << std::endl;
					OutputDebugString(CEConverters::convertCharArrayToLPCWSTR(oss.str().c_str()));
				}
				}
			}
		}

		if (result == -1) {
			return -1;
		}

		return msg.wParam;

	}
	catch (const CEException& e) {
		// CEException::CEExceptionBox(nullptr, e.what(), e.GetType(),MB_OK | MB_ICONEXCLAMATION);
		MessageBox(
			nullptr,
			CEConverters::convertCharArrayToLPCWSTR(e.what()),
			CEConverters::convertCharArrayToLPCWSTR(e.GetType()),
			MB_OK | MB_ICONEXCLAMATION
		);
	} catch (const std::exception& e) {
		// CEException::CEExceptionBox(nullptr, e.what(), "Exception", MB_OK | MB_ICONEXCLAMATION);
		MessageBox(
			nullptr,
			CEConverters::convertCharArrayToLPCWSTR(e.what()),
			CEConverters::convertCharArrayToLPCWSTR("Exception"),
			MB_OK | MB_ICONEXCLAMATION
		);
	} catch (...) {
		// CEException::CEExceptionBox(nullptr, "No error details", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
		MessageBox(
			nullptr,
			CEConverters::convertCharArrayToLPCWSTR("No error details"),
			CEConverters::convertCharArrayToLPCWSTR("Exception"),
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	return -1;
}
