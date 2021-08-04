#include "..\include\Spectre2D\Monitor.h"
#include "..\include\Spectre2D\internal.h"

#include "..\extlibs\glfw\include\GLFW\glfw3.h"

namespace sp
{
	std::vector<Monitor*> allMonitors;

	void empty_Mtrptr(Monitor*)
	{}

	Monitor::ConfigurationCallback monitorConnectedCallback = empty_Mtrptr;
	Monitor::ConfigurationCallback monitorDisconnectedCallback = empty_Mtrptr;

	GammaRamp::~GammaRamp()
	{
		delete[] red;
		delete[] green;
		delete[] blue;
	}

	Monitor::Monitor(GLFWmonitor* m)
	{
		monitor = m;

		name = glfwGetMonitorName(m);
		
		int x, y, w, h;
		glfwGetMonitorPos(m, &x, &y);
		position = glm::ivec2(x, y);

		glfwGetMonitorWorkarea(m, &x, &y, &w, &h);
		workAreaPosition = glm::ivec2(x, y);
		workAreaSize = glm::ivec2(w, h);

		glfwGetMonitorPhysicalSize(m, &w, &h);
		physicalSize = glm::ivec2(w, h);

		float fx, fy;
		glfwGetMonitorContentScale(m, &fx, &fy);
		contentScale = glm::vec2(fx, fy);

		const auto* ramp = glfwGetGammaRamp(m);

		gammaRamp = new GammaRamp{ new uint16_t[ramp->size], new uint16_t[ramp->size], new uint16_t[ramp->size], ramp->size };

		for (size_t i = 0; i < ramp->size; i++)
		{
			gammaRamp->red[i] = ramp->red[i];
			gammaRamp->green[i] = ramp->green[i];
			gammaRamp->blue[i] = ramp->blue[i];
		}

		int vm_count;
		const auto* vms = glfwGetVideoModes(m, &vm_count);
		const auto* current_vm = glfwGetVideoMode(m);

		videoModeCount = (size_t)vm_count;
		videoModes = new VideoMode[vm_count];

		currentVideoMode.width = current_vm->width;
		currentVideoMode.height = current_vm->height;
		currentVideoMode.redBits = current_vm->redBits;
		currentVideoMode.greenBits = current_vm->greenBits;
		currentVideoMode.blueBits = current_vm->blueBits;
		currentVideoMode.refreshRate = current_vm->refreshRate;

		for (size_t i = 0; i < videoModeCount; i++)
		{
			videoModes[i].width = vms[i].width;
			videoModes[i].height = vms[i].height;
			videoModes[i].redBits = vms[i].redBits;
			videoModes[i].greenBits = vms[i].greenBits;
			videoModes[i].blueBits = vms[i].blueBits;
			videoModes[i].refreshRate = vms[i].refreshRate;
		}

		glfwSetMonitorUserPointer(m, this);
	}

	Monitor::~Monitor()
	{
		if (videoModes)
			delete[] videoModes;

		delete gammaRamp;

		glfwSetMonitorUserPointer(monitor, 0);
	}

	glm::ivec2 Monitor::getPosition() const
	{
		if (!monitor)
			throw Error(SPECTRE_MONITOR_NOT_LINKED, "Attempt to call a function that requires a linked monitor without a linked monitor.\0");

		return position;
	}

	glm::ivec2 Monitor::getWorkAreaPosition() const
	{
		if (!monitor)
			throw Error(SPECTRE_MONITOR_NOT_LINKED, "Attempt to call a function that requires a linked monitor without a linked monitor.\0");
		return workAreaPosition;
	}

	glm::ivec2 Monitor::getWorkAreaSize() const
	{
		if (!monitor)
			throw Error(SPECTRE_MONITOR_NOT_LINKED, "Attempt to call a function that requires a linked monitor without a linked monitor.\0");
		return workAreaSize;
	}

	glm::ivec2 Monitor::getPhysicalSize() const
	{
		return physicalSize;
	}

	glm::ivec2 Monitor::getContentScale() const
	{
		return contentScale;
	}

	const std::string& Monitor::getName() const
	{
		return name;
	}

	const VideoMode* Monitor::getVideoModes(size_t* count) const
	{
		*count = videoModeCount;
		return videoModes;
	}

	const VideoMode& Monitor::getCurrentVideoMode() const
	{
		return currentVideoMode;
	}

	void Monitor::setGamma(float_t gamma)
	{
		glfwSetGamma(monitor, gamma);
	}

	void Monitor::setGammaRamp(const GammaRamp* ramp)
	{
		if (!monitor)
		{
			throw Error(SPECTRE_MONITOR_NOT_LINKED, "Attempt to call a function that requires a linked monitor without a linked monitor.\0");
			return;
		}

		GLFWgammaramp g_ramp{ 0, 0, 0, 0 };
		

		gammaRamp->size = ramp->size;
		g_ramp.size = (uint32_t)ramp->size;
		
		if (gammaRamp->red)
		{
			delete[] gammaRamp->red, gammaRamp->green, gammaRamp->blue;
		}

		gammaRamp->red = new uint16_t[gammaRamp->size];
		gammaRamp->green = new uint16_t[gammaRamp->size];
		gammaRamp->blue = new uint16_t[gammaRamp->size];

		g_ramp.red = new uint16_t[gammaRamp->size];
		g_ramp.blue = new uint16_t[gammaRamp->size];
		g_ramp.green = new uint16_t[gammaRamp->size];

		for (size_t i = 0; i < gammaRamp->size; i++)
		{
			g_ramp.red[i] = gammaRamp->red[i] = ramp->red[i];
			g_ramp.green[i] = gammaRamp->green[i] = ramp->green[i];
			g_ramp.blue[i] = gammaRamp->blue[i] = ramp->blue[i];
		}

		glfwSetGammaRamp(monitor, &g_ramp);
	}

	const GammaRamp* Monitor::getGammaRamp() const
	{
		if (!monitor)
			throw Error(SPECTRE_MONITOR_NOT_LINKED, "Attempt to call a function that requires a linked monitor without a linked monitor.\0");
		return gammaRamp;
	}

	Monitor* Monitor::getPrimary()
	{
		return (Monitor*)glfwGetMonitorUserPointer(glfwGetPrimaryMonitor());
	}

	const std::vector<Monitor*>& Monitor::getAll()
	{
		return allMonitors;
	}

	void Monitor::loadMonitors()
	{
		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);

		for (int32_t i = 0; i < count; i++)
		{
			allMonitors.push_back(new Monitor(monitors[i]));
		}
	}

	void Monitor::clearMonitors()
	{
		for (const auto& monitor : allMonitors)
		{
			delete monitor;
		}
		
		allMonitors.clear();
	}

	void Monitor::deleteMonitor(Monitor* monitor)
	{
		auto it = allMonitors.begin();

		for (; it != allMonitors.end(); it++)
		{
			if (*it == monitor)
				break;
		}

		std::swap(*it, allMonitors.back());
		allMonitors.pop_back();
	}

	Monitor* Monitor::pushMonitor(GLFWmonitor* monitor)
	{
		allMonitors.push_back(new Monitor(monitor));

		return allMonitors.back();
	}



	void Monitor::onMonitorConnected(const ConfigurationCallback& callback)
	{
		monitorConnectedCallback = callback;
	}

	const Monitor::ConfigurationCallback& Monitor::getOnMonitorConnected()
	{
		return monitorConnectedCallback;
	}

	void Monitor::onMonitorDisconnected(const ConfigurationCallback& callback)
	{
		monitorDisconnectedCallback = callback;
	}

	const Monitor::ConfigurationCallback& Monitor::getOnMonitorDisconnected()
	{
		return monitorDisconnectedCallback;
	}

}