#include "..\include\Spectre2D\Window.h"
#include "..\include\Spectre2D\Monitor.h"
#include "..\include\Spectre2D\internal.h"
#include "..\include\Spectre2D\Cursor.h"
#include "..\include\Spectre2D\Image.h"

#include <GLFW/glfw3.h>

namespace sp
{
	void empty_Winptr_ivec2cref(Window*,const glm::ivec2&)
	{}

	void empty_Winptr_uvec2cref(Window*, const glm::uvec2&)
	{}

	void empty_Winptr(Window*)
	{}

	void empty_Winptr_vec2cref(Window*,const glm::vec2&)
	{}

	void empty_Winptr_int32_int32_uint32(Window*,int32_t,int32_t,uint32_t)
	{}
	
	void empty_Winptr_dvec2cref_dvec2cref(Window*,const glm::dvec2&,const glm::dvec2&)
	{}

	void empty_Winptr_int32_int32_int32_uint32(Window*,int32_t,int32_t,int32_t,uint32_t)
	{}

	void empty_Winptr_uint32(Window*,uint32_t)
	{}

	void empty_Winptr_pathvectorcref(Window*,const std::vector<std::filesystem::path>&)
	{}

	Window::Window()
		: ResizableObject()
	{
		framebufferSize = glm::ivec2(0);
		swapInterval = 0;
		wnd = 0;
		ext_GLX_EXT_swap_control_tear = ext_WGL_EXT_swap_control_tear = false;
		vsync = false;
		opengl_minor = 3;
		opengl_major = 3;
		position = glm::ivec2(0);

		resizeCallback = empty_Winptr_uvec2cref;
		closedCallback = empty_Winptr;
		refreshedCallback = empty_Winptr;
		focusGainedCallback = empty_Winptr;
		focusLostCallback = empty_Winptr;
		iconifyCallback = empty_Winptr;
		deiconifyCallback = empty_Winptr;
		maximizeCallback = empty_Winptr;
		demaximizeCallback = empty_Winptr;
		framebufferResizeCallback = empty_Winptr_ivec2cref;
		contentScaleChangeCallback = empty_Winptr_vec2cref;

		mousePressedCallback = empty_Winptr_int32_int32_uint32;
		mouseReleasedCallback = empty_Winptr_int32_int32_uint32;
		mouseMovedCallback = empty_Winptr_dvec2cref_dvec2cref;
		mouseScrolledCallback = empty_Winptr_dvec2cref_dvec2cref;
		mouseEnteredCallback = empty_Winptr;
		mouseLeftCallback = empty_Winptr;
		keyPressedCallback = empty_Winptr_int32_int32_int32_uint32;
		keyReleasedCallback = empty_Winptr_int32_int32_int32_uint32;
		keyHeldCallback = empty_Winptr_int32_int32_int32_uint32;
		charInputCallback = empty_Winptr_uint32;
		fileDroppedCallback = empty_Winptr_pathvectorcref;

		lastMousePressTime = 0;
		lastMouseReleaseTime = 0;

		mousePressRepeat = 0;
		mouseReleaseRepeat = 0;

		mousePosition = glm::dvec2(0.0);
		scrollPosition = glm::dvec2(0.0);

		lastKeyPressTime = 0;
		lastKeyReleaseTime = 0;

		keyPressRepeat = 0;
		keyReleaseRepeat = 0;
		keyHeldRepeat = 0;
	}

	Window::Window(const glm::uvec2& _size, const std::string& _caption, Monitor* monitor, Window* share)
		: ResizableObject(_size)
	{
		framebufferSize = glm::ivec2(0);
		swapInterval = 0;
		wnd = 0;
		ext_GLX_EXT_swap_control_tear = ext_WGL_EXT_swap_control_tear = false;
		opengl_minor = 3;
		opengl_major = 3;
		position = glm::ivec2(0);

		resizeCallback = empty_Winptr_ivec2cref;
		closedCallback = empty_Winptr;
		refreshedCallback = empty_Winptr;
		focusGainedCallback = empty_Winptr;
		focusLostCallback = empty_Winptr;
		iconifyCallback = empty_Winptr;
		deiconifyCallback = empty_Winptr;
		maximizeCallback = empty_Winptr;
		demaximizeCallback = empty_Winptr;
		framebufferResizeCallback = empty_Winptr_ivec2cref;
		contentScaleChangeCallback = empty_Winptr_vec2cref;

		mousePressedCallback = empty_Winptr_int32_int32_uint32;
		mouseReleasedCallback = empty_Winptr_int32_int32_uint32;
		mouseMovedCallback = empty_Winptr_dvec2cref_dvec2cref;
		mouseScrolledCallback = empty_Winptr_dvec2cref_dvec2cref;
		mouseEnteredCallback = empty_Winptr;
		mouseLeftCallback = empty_Winptr;
		keyPressedCallback = empty_Winptr_int32_int32_int32_uint32;
		keyReleasedCallback = empty_Winptr_int32_int32_int32_uint32;
		keyHeldCallback = empty_Winptr_int32_int32_int32_uint32;
		charInputCallback = empty_Winptr_uint32;
		fileDroppedCallback = empty_Winptr_pathvectorcref;

		lastMousePressTime = 0;
		lastMouseReleaseTime = 0;

		mousePressRepeat = 0;
		mouseReleaseRepeat = 0;

		mousePosition = glm::dvec2(0.0);
		scrollPosition = glm::dvec2(0.0);

		lastKeyPressTime = 0;
		lastKeyReleaseTime = 0;

		keyPressRepeat = 0;
		keyReleaseRepeat = 0;
		keyHeldRepeat = 0;

		create(_size, _caption, monitor, share);
	}

	void Window::create(const glm::uvec2& _size, const std::string& _caption, Monitor* monitor, Window* share)
	{
		if (!_caption.empty())
			caption = _caption;

		ResizableObject::setSize(_size);

		glfwWindowHint(GLFW_FOCUSED, getFlag(WindowFlagB::Focused));
		glfwWindowHint(GLFW_MAXIMIZED, getFlag(WindowFlagB::Maximized));
		glfwWindowHint(GLFW_DECORATED, getFlag(WindowFlagB::Decorated));
		glfwWindowHint(GLFW_FLOATING, getFlag(WindowFlagB::Floating));
		glfwWindowHint(GLFW_VISIBLE, getFlag(WindowFlagB::Visible));
		glfwWindowHint(GLFW_RESIZABLE, getFlag(WindowFlagB::Resizable));
		glfwWindowHint(GLFW_AUTO_ICONIFY, getFlag(WindowFlagB::AutoIconify));
		glfwWindowHint(GLFW_FOCUS_ON_SHOW, getFlag(WindowFlagB::FocusOnShow));
		glfwWindowHint(GLFW_CENTER_CURSOR, getFlag(WindowFlagB::CenterCursor));
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, getFlag(WindowFlagB::TransparentFramebuffer));
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, getFlag(WindowFlagB::ScaleToMonitor));
		glfwWindowHint(GLFW_STEREO, getFlag(WindowFlagB::Stereo));
		glfwWindowHint(GLFW_SRGB_CAPABLE, getFlag(WindowFlagB::SRGBCapable));

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#else
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, getFlag(WindowFlagB::ForwardCompatible));
#endif

		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, getFlag(WindowFlagB::DebugContext));
		glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, getFlag(WindowFlagB::CocoaRetinaFramebuffer));
		glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, getFlag(WindowFlagB::CocoaGraphicsSwitching));
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_STENCIL_BITS, 8);
		glfwWindowHint(GLFW_SAMPLES, getFlag(WindowFlagI::Samples));
		glfwWindowHint(GLFW_SAMPLES, getFlag(WindowFlagI::RefreshRate));

		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHintString(GLFW_X11_CLASS_NAME, getFlag(WindowFlagS::X11ClassName).c_str());
		glfwWindowHintString(GLFW_X11_INSTANCE_NAME, getFlag(WindowFlagS::X11InstanceName).c_str());
		glfwWindowHintString(GLFW_COCOA_FRAME_NAME, getFlag(WindowFlagS::CocoaFrameName).c_str());

		int contextCreationAPI = 0;
		switch (getContextCreationAPI())
		{
#ifndef __APPLE__
		case ContextCreationAPIEnum::Egl:
			contextCreationAPI = GLFW_EGL_CONTEXT_API;
			break;
#endif

		case ContextCreationAPIEnum::Native:
			contextCreationAPI = GLFW_NATIVE_CONTEXT_API;
			break;

		case ContextCreationAPIEnum::OSMesa:
			contextCreationAPI = GLFW_OSMESA_CONTEXT_API;
			break;

		}

		glfwWindowHint(GLFW_CONTEXT_CREATION_API, contextCreationAPI);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, opengl_major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, opengl_minor);
		
		glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_ROBUSTNESS);
		glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_ANY_RELEASE_BEHAVIOR);

		wnd = glfwCreateWindow((int32_t)getSize().x, (int32_t)getSize().y, caption.c_str(), (monitor != nullptr) ? monitor->monitor : nullptr, (share != nullptr) ? share->wnd : nullptr);

		glfwSetWindowUserPointer(wnd, this);
		glfwMakeContextCurrent(wnd);

		if (vsync)
			setSwapInterval(1);

		glfwGetWindowPos(wnd, &position.x, &position.y);
		glfwGetFramebufferSize(wnd, &framebufferSize.x, &framebufferSize.y);

		glfwGetCursorPos(wnd, &mousePosition.x, &mousePosition.y);

		glfwSetWindowSizeCallback(wnd, _onResized);
		glfwSetWindowCloseCallback(wnd, _onClosed);
		glfwSetWindowRefreshCallback(wnd, _onRefreshed);
		glfwSetWindowFocusCallback(wnd, _onFocusChanged);
		glfwSetWindowIconifyCallback(wnd, _onIconifiedChanged);
		glfwSetWindowMaximizeCallback(wnd, _onMaximizedChanged);
		glfwSetFramebufferSizeCallback(wnd, _onFramebufferResized);
		glfwSetWindowContentScaleCallback(wnd, _onContentScaleChanged);

		glfwSetMouseButtonCallback(wnd, _onMouseButtonChanged);
		glfwSetCursorPosCallback(wnd, _onMouseMoved);
		glfwSetCursorEnterCallback(wnd, _onMouseEntered);
		glfwSetCharCallback(wnd, _onCharInput);
		glfwSetKeyCallback(wnd, _onKeyChanged);
		glfwSetDropCallback(wnd, _onFileDropped);
		glfwSetScrollCallback(wnd, _onMouseScrolled);

	}

	bool Window::isCreated() const
	{
		return wnd != nullptr;
	}

	bool Window::isActive() const
	{
		return glfwGetCurrentContext() == wnd;
	}

	void Window::setActive(bool active)
	{
		if (!wnd)
		{
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to activate a not created window.");
			return;
		}

		glfwMakeContextCurrent(wnd);
	}

	int32_t Window::getSwapInterval() const
	{
		return swapInterval;
	}

	void Window::setSwapInterval(int32_t interval)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to call `Window::setSwapInterval()` on a not created window.");

		Window* activeWnd = Window::getActiveWindow();

		if (activeWnd != this)
			setActive(true);

		if (interval < 0 && !(ext_GLX_EXT_swap_control_tear || ext_WGL_EXT_swap_control_tear))
		{
			if (activeWnd != this)
				setActive(false);

			throw Error(SPECTRE_INVALID_VALUE, "Attempt to set negative swap interval without proper extensions.");
		}

		glfwSwapInterval(interval);
		swapInterval = interval;

		if (activeWnd != this)
		{
			activeWnd->setActive(false);
		}
	}

	bool Window::extensionSupported(const char* name)
	{
		if (!wnd)
		{
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to call `Window::extensionSupported()` on a not created window.");
			return false;
		}

		if (!name)
		{
			throw Error(SPECTRE_INVALID_VALUE, "Invalid extension name.");
			return false;
		}

		bool result = false;
		Window* activeWnd = Window::getActiveWindow();

		if (activeWnd != this)
			setActive(true);

		result = glfwExtensionSupported(name);

		if (activeWnd != this)
			activeWnd->setActive(false);

		return result;
	}

	void* Window::getOpenGLProcAddress(const char* name)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to call `Window::getOpenGLProcAddress()` on a not created window.");

		if (!name)
			throw Error(SPECTRE_INVALID_VALUE, "Invalid extension name.");

		void* result;
		Window* activeWnd = Window::getActiveWindow();

		if (activeWnd != this)
			setActive(true);

		result = glfwGetProcAddress(name);

		if (activeWnd != this)
			activeWnd->setActive(false);

		return result;
	}

	void Window::getOpenGLVersion(int32_t* major, int32_t* minor) const
	{
		*major = opengl_major;
		*minor = opengl_minor;
	}

	void Window::setOpenGLVersion(int32_t major, int32_t minor)
	{
		opengl_major = major;
		opengl_minor = minor;
	}

	bool Window::isVSyncEnabled() const
	{
		return vsync;
	}

	void Window::setVSyncEnabled(bool value)
	{
		vsync = value;

		if (wnd)
			setSwapInterval(value);
	}

	bool Window::shouldClose() const
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to close a not created window.");

		return wnd && glfwWindowShouldClose(wnd);
	}

	void Window::setShouldClose(bool shouldClose)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to close a not created window.");

		glfwSetWindowShouldClose(wnd, shouldClose);
	}

	const std::string& Window::getCaption() const
	{
		return caption;
	}

	void Window::setCaption(const std::string& _caption)
	{
		caption = _caption;
	}

	void Window::close()
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to close a not created window.");

		if (isActive())
			setActive(false);

		glfwDestroyWindow(wnd);
		wnd = 0;
	}

	void Window::setDefaultFlags()
	{
		flagSet = WindowFlagSet();
	}

	void Window::setFlag(WindowFlagB flag, bool value)
	{
		flagSet.setFlag(flag, value);
	}

	void Window::setFlag(WindowFlagI flag, int32_t value)
	{
		flagSet.setFlag(flag, value);
	}

	void Window::setFlag(WindowFlagS flag, const std::string& value)
	{
		flagSet.setFlag(flag, value);
	}

	void Window::setFlag(ContextCreationAPIEnum value)
	{
		flagSet.setFlag(value);
	}

	ContextCreationAPIEnum Window::getContextCreationAPI() const
	{
		return flagSet.getContextCreationAPI();
	}

	bool Window::getFlag(WindowFlagB flag) const
	{
		return flagSet.getFlag(flag);
	}

	int32_t Window::getFlag(WindowFlagI flag) const
	{
		return flagSet.getFlag(flag);
	}

	const std::string& Window::getFlag(WindowFlagS flag) const
	{
		return flagSet.getFlag(flag);
	}

	Window* Window::getActiveWindow()
	{
		return (Window*)glfwGetWindowUserPointer(glfwGetCurrentContext());
	}

	GLFWwindow* Window::getGLFWWindow()
	{
		return wnd;
	}

	WindowFlagSet::WindowFlagSet()
	{
		bflags = {
			{WindowFlagB::Focused,					true},
			{WindowFlagB::Maximized,				false},
			{WindowFlagB::Resizable,				false},
			{WindowFlagB::Decorated,				true},
			{WindowFlagB::Visible,					true},
			{WindowFlagB::AutoIconify,				false},
			{WindowFlagB::Floating,					false},
			{WindowFlagB::FocusOnShow,				false},
			{WindowFlagB::CenterCursor,				false},
			{WindowFlagB::TransparentFramebuffer,	false},
			{WindowFlagB::ScaleToMonitor,			false},
			{WindowFlagB::Stereo,					false},
			{WindowFlagB::SRGBCapable,				false},
			{WindowFlagB::ForwardCompatible,		false},
			{WindowFlagB::DebugContext,				false},
			{WindowFlagB::CocoaRetinaFramebuffer,	false},
			{WindowFlagB::CocoaGraphicsSwitching,	false},
		};

		iflags = {
			{WindowFlagI::Samples,		0},
			{WindowFlagI::RefreshRate,	-1},
		};

		sflags = {
			{WindowFlagS::CocoaFrameName,	""},
			{WindowFlagS::X11ClassName,		""},
			{WindowFlagS::X11InstanceName,	""},
		};

		contextCreationAPI = ContextCreationAPIEnum::Native;
	}

	WindowFlagSet::~WindowFlagSet()
	{
		for (auto& [key, value] : sflags)
		{
			value.clear();
		}

		sflags.clear();
		bflags.clear();
		iflags.clear();
	}

	void WindowFlagSet::setFlag(WindowFlagI flag, int32_t value)
	{
		iflags.insert_or_assign(flag, value);
	}

	void WindowFlagSet::setFlag(WindowFlagB flag, bool value)
	{
		bflags.insert_or_assign(flag, value);
	}

	void WindowFlagSet::setFlag(WindowFlagS flag, const std::string& value)
	{
		sflags.insert_or_assign(flag, value);
	}

	void WindowFlagSet::setFlag(ContextCreationAPIEnum value)
	{
		contextCreationAPI = value;
	}

	bool WindowFlagSet::getFlag(WindowFlagB flag) const
	{
		return bflags.at(flag);
	}

	int32_t WindowFlagSet::getFlag(WindowFlagI flag) const
	{
		return iflags.at(flag);
	}

	const std::string& WindowFlagSet::getFlag(WindowFlagS flag) const
	{
		return sflags.at(flag);
	}

	ContextCreationAPIEnum WindowFlagSet::getContextCreationAPI() const
	{
		return contextCreationAPI;
	}

	void Window::setIcon(const std::vector<ImageData>& icons)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to set icon of a not created window.");

		GLFWimage* images = 0;

		// +1 so that the compiler does not output a warning. Just in case. There should be no problem with this snippet, but you can't be too careful.
		images = new GLFWimage[icons.size() + 1];
		images[icons.size()] = {0, 0, 0};

		for (size_t i = 0; i < icons.size(); i++)
		{
			images[i].width = icons[i].getWidth();
			images[i].height = icons[i].getHeight();

			const auto& rawData = icons[i].getRaw();


			// +1 so that the compiler does not output a warning. Just in case. There should be no problem with this snippet, but you can't be too careful.
			unsigned char* data = new unsigned char[rawData.size() * 4 + 4];

			for (size_t j = 0; j < rawData.size(); j++)
			{
				data[j * 4 + 0] = (unsigned char)(rawData[j].r * 255);
				data[j * 4 + 1] = (unsigned char)(rawData[j].g * 255);
				data[j * 4 + 2] = (unsigned char)(rawData[j].b * 255);
				data[j * 4 + 3] = (unsigned char)(rawData[j].a * 255);
			}

			images[i].pixels = data;
		}

		glfwSetWindowIcon(wnd, icons.size(), images);

		for (size_t i = 0; i < icons.size(); i++)
		{
			delete[] (images[i].pixels);
		}
	}

	void Window::setIcon(const std::vector<std::filesystem::path>& icons)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to set icon of a not created window.");

		std::vector<ImageData> images;

		for (const auto& file : icons)
			images.push_back(GenericImageLoader::load(file));

		setIcon(images);
	}

	void Window::restoreDefaultIcon()
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to set an icon of a not created window.");

		glfwSetWindowIcon(wnd, 0, 0);
	}

	const glm::ivec2& Window::getPosition() const
	{
		return position;
	}

	void Window::setPosition(const glm::ivec2& pos)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to set position of a not created window.\0");

		position = pos;
		glfwSetWindowPos(wnd, pos.x, pos.y);
	}

	void Window::setSizeLimits(const glm::uvec2& minSize, const glm::uvec2& maxSize)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to set size limits of a not created window.");

		glfwSetWindowSizeLimits(wnd, (int32_t)minSize.x, (int32_t)minSize.y, (int32_t)maxSize.x, (int32_t)maxSize.y);
	}

	void Window::setSize(const glm::uvec2& _size)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to resize a not created window.\0");

		ResizableObject::setSize(_size);

		glfwSetWindowSize(wnd, (int32_t)_size.x, (int32_t)_size.y);
		glfwGetFramebufferSize(wnd, &framebufferSize.x, &framebufferSize.y);
	}

	void Window::setAspectRatio(int32_t nominator, int32_t denominator)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to set aspect ratio a not created window.");

		glfwSetWindowAspectRatio(wnd, nominator, denominator);
	}

	void Window::getFrameSize(int32_t* top, int32_t* right, int32_t* bottom, int32_t* left)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to get frame size of a not created window.");

		glfwGetWindowFrameSize(wnd, left, top, right, bottom);
	}

	float Window::getOpacity() const
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to get opacity of a not created window.\0");

		return glfwGetWindowOpacity(wnd);
	}

	void Window::setOpacity(float opacity)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to set opacity of a not created window.");

		glfwSetWindowOpacity(wnd, opacity);
	}

	void Window::iconify()
	{
		if (wnd)
			glfwIconifyWindow(wnd);
		else
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to iconify a not created window.");
	}

	bool Window::isIconified() const
	{
		if (wnd)
			return glfwGetWindowAttrib(wnd, GLFW_ICONIFIED);
		else
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to call `Window::isIconified()` on a not created window.");
	}

	void Window::restore()
	{
		if (wnd)
			glfwRestoreWindow(wnd);
		else
		{
			setFlag(WindowFlagB::Maximized, false);
		}
	}

	void Window::maximize()
	{
		if (wnd)
			glfwMaximizeWindow(wnd);
		else
			setFlag(WindowFlagB::Maximized, true);
	}

	bool Window::isMaximized() const
	{
		if (wnd)
			return glfwGetWindowAttrib(wnd, GLFW_MAXIMIZED);
		else
			return getFlag(WindowFlagB::Maximized);
	}

	void Window::show()
	{
		if (wnd)
			glfwShowWindow(wnd);
		else
			setFlag(WindowFlagB::Visible, true);
	}

	void Window::hide()
	{
		if (wnd)
			glfwHideWindow(wnd);
		else
			setFlag(WindowFlagB::Visible, false);
	}

	void Window::focus(bool value)
	{
		if (wnd)
			glfwFocusWindow(wnd);
		else
			setFlag(WindowFlagB::Focused, value);
	}

	bool Window::isFocused() const
	{
		if (wnd)
			return glfwGetWindowAttrib(wnd, GLFW_FOCUSED);
		else
			return getFlag(WindowFlagB::Focused);
	}

	bool Window::isVisible() const
	{
		if (wnd)
			return glfwGetWindowAttrib(wnd, GLFW_VISIBLE);
		else
			return getFlag(WindowFlagB::Visible);
	}

	bool Window::isResizable() const
	{
		if (wnd)
			return glfwGetWindowAttrib(wnd, GLFW_RESIZABLE);
		else
			return getFlag(WindowFlagB::Resizable);
	}

	void Window::setResizable(bool value)
	{
		if (wnd)
			glfwSetWindowAttrib(wnd, GLFW_RESIZABLE, value);
		else
			setFlag(WindowFlagB::Resizable, value);
	}

	bool Window::isHovered() const
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to get `hovered` flag a not created window.");

		return glfwGetWindowAttrib(wnd, GLFW_HOVERED);
	}

	bool Window::isDecorated() const
	{
		if (wnd)
			return glfwGetWindowAttrib(wnd, GLFW_DECORATED);
		else
			return getFlag(WindowFlagB::Decorated);
	}

	void Window::setDecorated(bool value)
	{
		if (wnd)
			glfwSetWindowAttrib(wnd, GLFW_DECORATED, value);
		else
			setFlag(WindowFlagB::Decorated, value);
	}

	bool Window::isAutoIconfied() const
	{
		if (wnd)
			return glfwGetWindowAttrib(wnd, GLFW_AUTO_ICONIFY);
		else
			return getFlag(WindowFlagB::AutoIconify);
	}

	void Window::setAutoIconify(bool value)
	{
		if (wnd)
			glfwSetWindowAttrib(wnd, GLFW_AUTO_ICONIFY, value);
		else
			setFlag(WindowFlagB::AutoIconify, value);
	}

	bool Window::isFloating() const
	{
		if (wnd)
			return glfwGetWindowAttrib(wnd, GLFW_FLOATING);
		else
			return getFlag(WindowFlagB::Floating);
	}

	void Window::setFloating(bool value)
	{
		if (wnd)
			glfwSetWindowAttrib(wnd, GLFW_FLOATING, value);
		else
			setFlag(WindowFlagB::Floating, value);
	}

	bool Window::hasTransparentFramebuffer() const
	{
		if (wnd)
			return glfwGetWindowAttrib(wnd, GLFW_TRANSPARENT_FRAMEBUFFER);
		else
			return getFlag(WindowFlagB::TransparentFramebuffer);
	}

	bool Window::doesFocusOnShow() const
	{
		if (wnd)
			return glfwGetWindowAttrib(wnd, GLFW_FOCUS_ON_SHOW);
		else
			return getFlag(WindowFlagB::FocusOnShow);
	}

	void Window::setFocusOnShow(bool value)
	{
		if (wnd)
			glfwSetWindowAttrib(wnd, GLFW_FOCUS_ON_SHOW, value);
		else
			setFlag(WindowFlagB::FocusOnShow, value);
	}

	void Window::requestAttention()
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to request attention for a not created window");

		glfwRequestWindowAttention(wnd);
	}

	Monitor* Window::getMonitor() const
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to get monitor of not created window.");

		return (Monitor*)glfwGetMonitorUserPointer(glfwGetWindowMonitor(wnd));
	}

	void Window::setMonitor(Monitor* monitor, const glm::ivec2& pos, const glm::uvec2& _size, int32_t refreshRate)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to set monitor of not created window.");

		glfwSetWindowMonitor(wnd, monitor->monitor, pos.x, pos.y, (int32_t)_size.x, (int32_t)_size.y, refreshRate);

		position = pos;

		ResizableObject::setSize(_size);

		glfwGetFramebufferSize(wnd, &framebufferSize.x, &framebufferSize.y);
	}

	const glm::ivec2& Window::getPixelSize() const
	{
		return framebufferSize;
	}

	void Window::onResized(const ResizeCallback& callback)
	{
		resizeCallback = callback;
	}

	const Window::ResizeCallback& Window::getOnResized() const
	{
		return resizeCallback;
	}

	void Window::onClosed(const ClosedCallback& callback)
	{
		closedCallback = callback;
	}

	const Window::ClosedCallback& Window::getOnClosed() const
	{
		return closedCallback;
	}

	void Window::onRefreshed(const RefreshedCallback& callback)
	{
		refreshedCallback = callback;
	}

	const Window::RefreshedCallback& Window::getOnRefreshed() const
	{
		return refreshedCallback;
	}

	void Window::onFocusGained(const FocusGainedCallback& callback)
	{
		focusGainedCallback = callback;
	}

	const Window::FocusGainedCallback& Window::getOnFocusGained() const
	{
		return focusGainedCallback;
	}

	void Window::onFocusLost(const FocusLostCallback& callback)
	{
		focusLostCallback = callback;
	}

	const Window::FocusLostCallback& Window::getOnFocusLost() const
	{
		return focusLostCallback;
	}

	void Window::onIconified(const IconifyCallback& callback)
	{
		iconifyCallback = callback;
	}

	const Window::IconifyCallback& Window::getOnIconify() const
	{
		return iconifyCallback;
	}

	void Window::onDeiconified(const DeiconifyCallback& callback)
	{
		deiconifyCallback = callback;
	}

	const Window::DeiconifyCallback& Window::getOnDeiconified() const
	{
		return deiconifyCallback;
	}

	void Window::onMaximized(const MaximizeCallback& callback)
	{
		maximizeCallback = callback;
	}

	const Window::MaximizeCallback& Window::getOnMaximized() const
	{
		return maximizeCallback;
	}

	void Window::onDemaximized(const DemaximizeCallback& callback)
	{
		demaximizeCallback = callback;
	}

	const Window::DemaximizeCallback& Window::getOnDemaximized() const
	{
		return demaximizeCallback;
	}

	void Window::onFramebufferResized(const FramebufferResizeCallback& callback)
	{
		framebufferResizeCallback = callback;
	}

	const Window::FramebufferResizeCallback& Window::getOnFramebufferResized() const
	{
		return framebufferResizeCallback;
	}

	void Window::onContentScaleChanged(const ContentScaleChangeCallback& callback)
	{
		contentScaleChangeCallback = callback;
	}

	const Window::ContentScaleChangeCallback& Window::getOnContentScaleChanged() const
	{
		return contentScaleChangeCallback;
	}

	void Window::onMousePressed(const MousePressedCallback& callback)
	{
		mousePressedCallback = callback;
	}

	const Window::MousePressedCallback& Window::getOnMousePressed() const
	{
		return mousePressedCallback;
	}

	void Window::onMouseReleased(const MouseReleasedCallback& callback)
	{
		mouseReleasedCallback = callback;
	}

	const Window::MouseReleasedCallback& Window::getOnMouseReleased() const
	{
		return mouseReleasedCallback;
	}

	void Window::onMouseMoved(const MouseMovedCallback& callback)
	{
		mouseMovedCallback = callback;
	}

	const Window::MouseMovedCallback& Window::getOnMouseMoved() const
	{
		return mouseMovedCallback;
	}

	void Window::onMouseScrolled(const MouseScrolledCallback& callback)
	{
		mouseScrolledCallback = callback;
	}

	const Window::MouseScrolledCallback& Window::getOnMouseScrolled() const
	{
		return mouseScrolledCallback;
	}

	void Window::onMouseEntered(const MouseEnteredCallback& callback)
	{
		mouseEnteredCallback = callback;
	}

	const Window::MouseEnteredCallback& Window::getOnMouseEntered() const
	{
		return mouseEnteredCallback;
	}

	void Window::onMouseLeft(const MouseLeftCallback& callback)
	{
		mouseLeftCallback = callback;
	}

	const Window::MouseLeftCallback& Window::getOnMouseLeft() const
	{
		return mouseLeftCallback;
	}

	void Window::onKeyPressed(const KeyPressedCallback& callback)
	{
		keyPressedCallback = callback;
	}

	const Window::KeyPressedCallback& Window::getOnKeyPressed() const
	{
		return keyPressedCallback;
	}

	void Window::onKeyReleased(const KeyReleasedCallback& callback)
	{
		keyReleasedCallback = callback;
	}

	const Window::KeyReleasedCallback& Window::getOnKeyReleased() const
	{
		return keyReleasedCallback;
	}

	void Window::onKeyHeld(const KeyHeldCallback& callback)
	{
		keyHeldCallback = callback;
	}

	const Window::KeyHeldCallback& Window::getOnKeyHeld() const
	{
		return keyHeldCallback;
	}

	void Window::onCharInput(const CharInputCallback& callback)
	{
		charInputCallback = callback;
	}

	const Window::CharInputCallback& Window::getOnCharInput() const
	{
		return charInputCallback;
	}

	void Window::onFileDropped(const FileDroppedCallback& callback)
	{
		fileDroppedCallback = callback;
	}

	const Window::FileDroppedCallback& Window::getOnFileDropped() const
	{
		return fileDroppedCallback;
	}

	double_t Window::getRepeatedEventInterval() const
	{
		return repeatedEventInterval;
	}

	void Window::setRepeatedEventInterval(double_t interval)
	{
		repeatedEventInterval = interval;
	}

	const glm::dvec2& Window::getMousePosition() const
	{
		return mousePosition;
	}

	void Window::setMousePosition(double x, double y)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to set cursor position for not created window.");

		mousePosition.x = x;
		mousePosition.y = y;

		glfwSetCursorPos(wnd, x, y);
	}

	void Window::setMousePosition(const glm::dvec2& _position)
	{
		setMousePosition(_position.x, _position.y);
	}

	const glm::dvec2& sp::Window::getScrollPosition() const
	{
		return scrollPosition;
	}

	void Window::flip()
	{
		glfwSwapBuffers(wnd);
	}

	bool Window::getStickyMouseButtons() const
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attemot to call `Window::getStickyMouseButtons()` on a not created windwow.");

		return glfwGetInputMode(wnd, GLFW_STICKY_MOUSE_BUTTONS);
	}

	void Window::setStickyMouseButtons(bool value)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to call `Window::setStickyMouseButtons()` on a not created window.");

		glfwSetInputMode(wnd, GLFW_STICKY_MOUSE_BUTTONS, value);
	}

	bool Window::getStickyKeys() const
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attemot to call `Window::getStickyKeys()` on a not created windwow.");

		return glfwGetInputMode(wnd, GLFW_STICKY_KEYS);
	}

	void Window::setStickyKeys(bool value)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to call `Window::setStickyKeys()` on a not created window.");

		glfwSetInputMode(wnd, GLFW_STICKY_KEYS, value);
	}

	bool Window::getKeyModsLocked() const
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attemot to call `Window::getKeyModsLocked()` on a not created windwow.");

		return glfwGetInputMode(wnd, GLFW_LOCK_KEY_MODS);
	}

	void Window::setKeyModsLocked(bool value)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to call `Window::setKeyModsLocked()` on a not created window.");

		glfwSetInputMode(wnd, GLFW_LOCK_KEY_MODS, value);
	}

	bool Window::getRawMouseMotion() const
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attemot to call `Window::getMouseRawMotion()` on a not created windwow.");

		return glfwGetInputMode(wnd, GLFW_RAW_MOUSE_MOTION);
	}

	void Window::setRawMouseMotion(bool value)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to call `Window::setMouseRawMotion()` on a not created window.");

		glfwSetInputMode(wnd, GLFW_RAW_MOUSE_MOTION, value);
	}

	CursorMode Window::getCursorMode() const
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to call `Window::setMouseRawMotion()` on a not created window.");

		switch (glfwGetInputMode(wnd, GLFW_CURSOR))
		{
		case GLFW_CURSOR_NORMAL:
			return CursorMode::Normal;
		case GLFW_CURSOR_DISABLED:
			return CursorMode::Disabled;
		case GLFW_CURSOR_HIDDEN:
			return CursorMode::Hidden;
		default:
			return CursorMode::Normal;
		}
	}

	void Window::setCursorMode(CursorMode mode)
	{
		if (!wnd)
			throw Error(SPECTRE_WINDOW_NOT_CREATED, "Attempt to call `Window::setMouseRawMotion()` on a not created window.");

		int cmode = 0;

		switch (mode)
		{
		case CursorMode::Normal:
			cmode = GLFW_CURSOR_NORMAL;
			break;

		case CursorMode::Disabled:
			cmode = GLFW_CURSOR_DISABLED;
			break;

		case CursorMode::Hidden:
			cmode = GLFW_CURSOR_HIDDEN;
			break;

		default:
			cmode = GLFW_CURSOR_NORMAL;
			break;
		}

		glfwSetInputMode(wnd, GLFW_CURSOR, cmode);
	}

	bool Window::keyPressed(int32_t keycode) const
	{
		return glfwGetKey(wnd, keycode) == GLFW_PRESS;
	}

	bool Window::mousePressed(int32_t button) const
	{
		return glfwGetMouseButton(wnd, button) == GLFW_PRESS;
	}

	void Window::setCursor(const Cursor& cursor)
	{
		glfwSetCursor(wnd, cursor.cursor);
	}

	void Window::updateSize()
	{
		if (wnd)
		{
			int32_t width, height;
			glfwGetWindowSize(wnd, &width, &height);

			ResizableObject::setSize({ width, height });

			glfwGetFramebufferSize(wnd, &framebufferSize.x, &framebufferSize.y);
		}
	}

}