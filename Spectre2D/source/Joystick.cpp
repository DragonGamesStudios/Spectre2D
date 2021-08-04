#include "..\include\Spectre2D\Joystick.h"

#include "..\extlibs\glfw\include\GLFW\glfw3.h"

namespace sp
{
	void empty_Jskptr(Joystick*)
	{}

	std::array<Joystick*, GLFW_JOYSTICK_LAST> joysticks{0};

	Joystick::Joystick(int32_t _id)
	{
		id = _id;
		gamepad = false;
		connectedCallback = empty_Jskptr;
		disconnectedCallback = empty_Jskptr;

		glfwSetJoystickUserPointer(id, this);

		update();
	}

	Joystick::~Joystick()
	{
		Joystick* ptr = (Joystick*)glfwGetJoystickUserPointer(id);

		if (ptr == this)
			glfwSetJoystickUserPointer(id, 0);
	}

	bool Joystick::isPresent() const
	{
		return glfwJoystickPresent(id);
	}

	bool Joystick::isGamepad() const
	{
		return gamepad;
	}

	const std::vector<const float_t*>& Joystick::getAxes() const
	{
		return axes;
	}

	float_t Joystick::getAxis(size_t i) const
	{
		return *axes[i];
	}

	size_t Joystick::getAxesCount() const
	{
		return axes.size();
	}

	const std::vector<const uint8_t*>& Joystick::getButtons() const
	{
		return buttons;
	}

	uint8_t Joystick::getButton(size_t i)
	{
		return *buttons[i];
	}

	size_t Joystick::getButtonsCount() const
	{
		return buttons.size();
	}

	const std::vector<const uint8_t*>& Joystick::getHats() const
	{
		return hats;
	}

	uint8_t Joystick::getHat(size_t i)
	{
		return *hats[i];
	}

	size_t Joystick::getHatsCount() const
	{
		return hats.size();
	}

	void Joystick::updateGamepadMappings(const std::string& mappings)
	{
		glfwUpdateGamepadMappings(mappings.c_str());
	}

	const std::string& Joystick::getGamepadName()
	{
		return gamepadName;
	}

	int32_t Joystick::getId() const
	{
		return id;
	}

	void Joystick::onConnected(const ConfigurationCallback& callback)
	{
		connectedCallback = callback;
	}

	const Joystick::ConfigurationCallback& Joystick::getOnConnected() const
	{
		return connectedCallback;
	}

	void Joystick::onDisconnected(const ConfigurationCallback& callback)
	{
		disconnectedCallback = callback;
	}

	const Joystick::ConfigurationCallback& Joystick::getOnDisconnected() const
	{
		return disconnectedCallback;
	}

	void Joystick::getGamepadState(GamepadState* state) const
	{
		GLFWgamepadstate gs;
		glfwGetGamepadState(id, &gs);

		/*
		struct GLFWgamepadstate
		{
			unsigned char buttons[15];
			float axes[6];
		}
		*/

		for (size_t i = 0; i < 15; i++)
			state->buttons[i] = gs.buttons[i];

		for (size_t i = 0; i < 6; i++)
			state->axes[i] = gs.axes[i];
	}

	void Joystick::loadJoysticks()
	{
		for (int32_t i = 0; i < GLFW_JOYSTICK_LAST; i++)
			joysticks[i] = new Joystick(i);
	}

	void Joystick::clearJoysticks()
	{
		for (int32_t i = 0; i < GLFW_JOYSTICK_LAST; i++)
			delete joysticks[i];
	}

	Joystick* Joystick::getJoystick(int32_t id)
	{
		return (Joystick*)glfwGetJoystickUserPointer(id);
	}

	void Joystick::update()
	{
		if (isPresent())
		{
			int32_t axes_count;
			const float* axes_array = glfwGetJoystickAxes(id, &axes_count);

			for (int32_t i = 0; i < axes_count; i++)
				axes.push_back(&axes_array[i]);

			gamepad = glfwJoystickIsGamepad(id);
			name = glfwGetJoystickName(id);
			gamepadName = glfwGetGamepadName(id);
			GUID = glfwGetJoystickGUID(id);

			int32_t buttonsCount;
			const uint8_t* buttonArray = glfwGetJoystickButtons(id, &buttonsCount);

			for (int32_t i = 0; i < buttonsCount; i++)
				buttons.push_back(&buttonArray[i]);

			int32_t hatsCount;
			const uint8_t* hatArray = glfwGetJoystickHats(id, &hatsCount);

			for (int32_t i = 0; i < hatsCount; i++)
				hats.push_back(&hatArray[i]);
		}
		else
		{
			gamepad = false;
			name.clear();
			gamepadName.clear();
			GUID.clear();
			buttons.clear();
			hats.clear();
			axes.clear();
		}
	}

}