#include "..\include\Spectre2D\internal.h"
#include "..\include\Spectre2D\Window.h"
#include "..\include\Spectre2D\Monitor.h"
#include "..\include\Spectre2D\Joystick.h"

#include "..\extlibs\glew\include\GL\glew.h"
#include "..\extlibs\glfw\include\GLFW\glfw3.h"

#include <iostream>

namespace sp
{
	bool BIG_ENDIAN = false;

	void internalErrorCallback(int32_t code, const char* msg)
	{
		// Capture any GLFW errors and 'convert' them to Spectre2D errors.
		switch (code)
		{
		case GLFW_NOT_INITIALIZED:
			throw Error(SPECTRE_NOT_INITIALIZED, "Spectre2D library was not initialized.");
			break;

		case GLFW_NO_CURRENT_CONTEXT:
			throw Error(SPECTRE_NO_ACTIVE_WINDOW, "Attempt to call a function requiring an active window without activating a window.");
			break;

		case GLFW_NO_WINDOW_CONTEXT:
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to call a function on a not created window.");
			break;

		case GLFW_INVALID_VALUE:
			throw Error(SPECTRE_INVALID_VALUE, msg);
			break;

		case GLFW_PLATFORM_ERROR:
			throw Error(SPECTRE_PLATFORM_ERROR, msg);
			break;

		case GLFW_INVALID_ENUM:
			throw Error(SPECTRE_INVALID_ENUM, msg);
			break;

		default:
			throw Error(SPECTRE_UNKNOWN_ERROR, msg);
			break;
		}
	}

	void init()
	{
		// Prepare error callback
		glfwSetErrorCallback(internalErrorCallback);

		// Tell GLFW not to count joystick hats as buttons
		glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);

		// Init
		if (!glfwInit())
		{
			throw Error(SPECTRE_NOT_INITIALIZED, "Spectre2D failed to initialize. GLFW could not initialize.");
		}

		// Load monitors and joysticks
		Monitor::loadMonitors();
		Joystick::loadJoysticks();

		// Setup monitor and joystick callbacks
		glfwSetMonitorCallback(_onMonitorConfigurationChanged);
		glfwSetJoystickCallback(_onJoystickConfigurationChanged);

		// Test for endianness (unnecessary, the code requires a fix)
		union {
			uint16_t i;
			char c[4];
		} endianTest = {0x0102};

		BIG_ENDIAN = endianTest.c[0] == 0x01;
	}

	void finish()
	{
		// Deactivate any active windows
		if (Window::getActiveWindow() && Window::getActiveWindow()->isCreated())
			Window::getActiveWindow()->setActive(false);

		// Free memory for monitors and joysticks
		Monitor::clearMonitors();
		Joystick::clearJoysticks();

		// Terminate GLFW
		glfwTerminate();
	}

	void pollEvents()
	{
		glfwPollEvents();
	}

	void waitForEvents()
	{
		glfwWaitEvents();
	}

	void waitForEventsWithTimeout(double timeout)
	{
		glfwWaitEventsTimeout(timeout);
	}

	void raiseEmptyEvent()
	{
		glfwPostEmptyEvent();
	}

	bool rawMouseMotionSupported()
	{
		return glfwRawMouseMotionSupported();
	}

	std::string getKeyName(int32_t keycode)
	{
		return glfwGetKeyName(keycode, getKeyScancode(keycode));
	}

	int32_t getKeyScancode(int32_t keycode)
	{
		return glfwGetKeyScancode(keycode);
	}

	double_t getTime()
	{
		return glfwGetTime();
	}
	std::string getClipboardText()
	{
		return glfwGetClipboardString(NULL);
	}

	void setClipboardText(const std::string& text)
	{
		glfwSetClipboardString(NULL, text.c_str());
	}

	Error::Error(uint16_t _code, const std::string& _description)
		: code(_code), description(_description)
	{
	}

}