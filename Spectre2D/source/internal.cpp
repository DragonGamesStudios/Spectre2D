#include "..\include\Spectre2D\internal.h"
#include "..\include\Spectre2D\Window.h"
#include "..\include\Spectre2D\Monitor.h"
#include "..\include\Spectre2D\Joystick.h"

#include "..\extlibs\glfw\include\GLFW\glfw3.h"

namespace sp
{
	void Internal::onResized(GLFWwindow* window, int32_t width, int32_t height)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		wnd->updateSize();

		wnd->getOnResized()(wnd, { wnd->getSize() });
	}

	void Internal::onClosed(GLFWwindow* window)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		wnd->closedCallback(wnd);
	}

	void Internal::onRefreshed(GLFWwindow* window)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		wnd->refreshedCallback(wnd);
	}

	void Internal::onFocusChanged(GLFWwindow* window, int32_t focused)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		if (focused)
			wnd->focusGainedCallback(wnd);
		else
			wnd->focusLostCallback(wnd);
	}

	void Internal::onIconifiedChanged(GLFWwindow* window, int32_t iconified)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		if (iconified)
			wnd->iconifyCallback(wnd);
		else
			wnd->deiconifyCallback(wnd);
	}

	void Internal::onMaximizedChanged(GLFWwindow* window, int32_t maximized)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		if (maximized)
			wnd->getOnMaximized()(wnd);
		else
			wnd->getOnDemaximized()(wnd);
	}

	void Internal::onFramebufferResized(GLFWwindow* window, int32_t width, int32_t height)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		glfwGetFramebufferSize(window, &wnd->framebufferSize.x, &wnd->framebufferSize.y);

		wnd->framebufferResizeCallback(wnd, wnd->framebufferSize);
	}

	void Internal::onContentScaleChanged(GLFWwindow* window, float_t xscale, float_t yscale)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		wnd->contentScaleChangeCallback(wnd, { xscale, yscale });
	}

	void Internal::onMouseButtonChanged(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		switch (action)
		{
		case GLFW_PRESS:
			// Consider repetition

			if (wnd->lastMousePressTime)
			{
				if (glfwGetTime() - wnd->lastMousePressTime >= wnd->repeatedEventInterval)
					wnd->mousePressRepeat = 0;
				else
					wnd->mousePressRepeat++;
			}

			wnd->lastMousePressTime = glfwGetTime();

			wnd->mousePressedCallback(wnd, button, mods, wnd->mousePressRepeat);

			break;

		case GLFW_RELEASE:
			// Consider repetition

			if (wnd->lastMouseReleaseTime)
			{
				if (glfwGetTime() - wnd->lastMouseReleaseTime >= wnd->repeatedEventInterval)
					wnd->mouseReleaseRepeat = 0;
				else
					wnd->mouseReleaseRepeat++;
			}

			wnd->lastMouseReleaseTime = glfwGetTime();

			wnd->mouseReleasedCallback(wnd, button, mods, wnd->mouseReleaseRepeat);

			break;
		}
	}

	void Internal::onMouseMoved(GLFWwindow* window, double_t x, double_t y)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		glm::dvec2 old = wnd->mousePosition;
		wnd->mousePosition.x = x;
		wnd->mousePosition.y = y;

		wnd->mouseMovedCallback(wnd, wnd->mousePosition, wnd->mousePosition - old);
	}

	void Internal::onMouseEntered(GLFWwindow* window, int32_t entered)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		if (entered)
			wnd->mouseEnteredCallback(wnd);
		else
			wnd->mouseLeftCallback(wnd);
	}

	void Internal::onMouseScrolled(GLFWwindow* window, double_t x, double_t y)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		glm::dvec2 old = wnd->scrollPosition;
		wnd->scrollPosition.x = x;
		wnd->scrollPosition.y = y;

		wnd->mouseScrolledCallback(wnd, wnd->scrollPosition, wnd->scrollPosition - old);
	}

	void Internal::onKeyChanged(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		switch (action)
		{
		case GLFW_PRESS:
			// Consider repetition

			if (wnd->lastKeyPressTime)
			{
				if (glfwGetTime() - wnd->lastKeyPressTime >= wnd->repeatedEventInterval)
					wnd->keyPressRepeat = 0;
				else
					wnd->keyPressRepeat++;
			}

			wnd->lastKeyPressTime = glfwGetTime();

			wnd->keyHeldRepeat = 1;

			wnd->keyPressedCallback(wnd, key, scancode, mods, wnd->keyPressRepeat);

			break;

		case GLFW_RELEASE:
			// Consider repetition

			if (wnd->lastKeyReleaseTime)
			{
				if (glfwGetTime() - wnd->lastKeyReleaseTime >= wnd->repeatedEventInterval)
					wnd->keyReleaseRepeat = 0;
				else
					wnd->keyReleaseRepeat++;
			}

			wnd->lastKeyReleaseTime = glfwGetTime();

			wnd->keyHeldRepeat = 0;

			wnd->keyReleasedCallback(wnd, key, scancode, mods, wnd->keyReleaseRepeat);

			break;

		case GLFW_REPEAT:
			// Consider repetition

			wnd->keyHeldRepeat++;

			wnd->keyHeldCallback(wnd, key, scancode, mods, wnd->keyHeldRepeat);

			break;
		}
	}

	void Internal::onCharInput(GLFWwindow* window, uint32_t code)
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		wnd->charInputCallback(wnd, code);
	}

	void Internal::onFileDropped(GLFWwindow* window, int32_t pathCount, const char* paths[])
	{
		Window* wnd = (Window*)glfwGetWindowUserPointer(window);

		std::vector<std::filesystem::path> vec;

		for (int32_t i = 0; i < pathCount; i++)
			vec.push_back(paths[i]);

		wnd->fileDroppedCallback(wnd, vec);
	}

	void Internal::onMonitorConfigurationChanged(GLFWmonitor* monitor, int32_t action)
	{
		Monitor* mt = 0;

		if (action == GLFW_CONNECTED)
		{
			mt = Monitor::pushMonitor(monitor);
		}
		else
		{
			 mt = (Monitor*)glfwGetMonitorUserPointer(monitor);

			if (mt)
				Monitor::deleteMonitor(mt);
		}

		switch (action)
		{
		case GLFW_CONNECTED:
			Monitor::getOnMonitorConnected()(mt);
			break;

		case GLFW_DISCONNECTED:
			Monitor::getOnMonitorDisconnected()(mt);
			break;
		}
	}

	void Internal::onJoystickConfigurationChanged(int32_t id, int32_t action)
	{
		Joystick* joystick = (Joystick*)glfwGetJoystickUserPointer(id);

		if (joystick)
		{
			switch (action)
			{
			case GLFW_CONNECTED:
				joystick->update();
				joystick->getOnConnected()(joystick);

				break;

			case GLFW_DISCONNECTED:
				joystick->getOnDisconnected()(joystick);

				break;
			}
		}
	}

	void _onResized(GLFWwindow* window, int32_t width, int32_t height)
	{
		Internal::onResized(window, width, height);
	}

	void _onClosed(GLFWwindow* window)
	{
		Internal::onClosed(window);
	}

	void _onRefreshed(GLFWwindow* window)
	{
		Internal::onRefreshed(window);
	}

	void _onFocusChanged(GLFWwindow* window, int32_t focused)
	{
		Internal::onFocusChanged(window, focused);
	}

	void _onIconifiedChanged(GLFWwindow* window, int32_t iconified)
	{
		Internal::onIconifiedChanged(window, iconified);
	}

	void _onMaximizedChanged(GLFWwindow* window, int32_t maximized)
	{
		Internal::onMaximizedChanged(window, maximized);
	}

	void _onFramebufferResized(GLFWwindow* window, int32_t width, int32_t height)
	{
		Internal::onFramebufferResized(window, width, height);
	}

	void _onContentScaleChanged(GLFWwindow* window, float_t xscale, float_t yscale)
	{
		Internal::onContentScaleChanged(window, xscale, yscale);
	}

	void _onMouseButtonChanged(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
	{
		Internal::onMouseButtonChanged(window, button, action, mods);
	}

	void _onMouseMoved(GLFWwindow* window, double_t x, double_t y)
	{
		Internal::onMouseMoved(window, x, y);
	}

	void _onMouseEntered(GLFWwindow* window, int32_t entered)
	{
		Internal::onMouseEntered(window, entered);
	}

	void _onMouseScrolled(GLFWwindow* window, double_t x, double_t y)
	{
		Internal::onMouseScrolled(window, x, y);
	}

	void _onKeyChanged(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
	{
		Internal::onKeyChanged(window, key, scancode, action, mods);
	}

	void _onCharInput(GLFWwindow* window, uint32_t code)
	{
		Internal::onCharInput(window, code);
	}

	void _onFileDropped(GLFWwindow* window, int32_t pathCount, const char* paths[])
	{
		Internal::onFileDropped(window, pathCount, paths);
	}

	void _onMonitorConfigurationChanged(GLFWmonitor* monitor, int32_t action)
	{
		Internal::onMonitorConfigurationChanged(monitor, action);
	}

	void _onJoystickConfigurationChanged(int32_t id, int32_t action)
	{
		Internal::onJoystickConfigurationChanged(id, action);
	}

}