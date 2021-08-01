#include <Spectre2D/core.h>
#include <Spectre2D/Monitor.h>
#include <Spectre2D/Window.h>
#include <iostream>

struct Rect
{
	glm::dvec2 pos;
	glm::dvec2 size;
};

struct Platform
{
	Rect rect;
	glm::dvec2 speed;
};

struct Ball
{
	Rect rect;
	glm::dvec2 speed;
};

Platform platform{};
Ball ball{};

sp::Window platform_wnd;
sp::Window ball_wnd;
sp::VideoMode vidmode;

void onKeyPressed(sp::Window* wnd, int key, int scancode, int mods, uint32_t repeat)
{
	switch (key)
	{
	case SPECTRE_KEY_LEFT:
		platform.speed.x = -300.0;
		break;

	case SPECTRE_KEY_RIGHT:
		platform.speed.x = 300.0;
		break;
	}
}

void onKeyReleased(sp::Window* wnd, int key, int scancode, int mods, uint32_t repeat)
{
	switch (key)
	{
	case SPECTRE_KEY_LEFT:
		if (platform.speed.x < 0)
			platform.speed.x = 0;
		break;

	case SPECTRE_KEY_RIGHT:
		if (platform.speed.x > 0)
			platform.speed.x = 0;
		break;
	}
}

void update(double dt)
{
	platform.rect.pos += platform.speed * dt;

	// Restrict platform movement
	if (platform.rect.pos.x <= 0)
		platform.rect.pos.x = 0;
	else if (platform.rect.pos.x + platform.rect.size.x >= vidmode.width)
		platform.rect.pos.x = vidmode.width - platform.rect.size.x;

	//std::cout << dt << "|" << platform.rect.pos.x << "|" << platform.speed.x << std::endl;

	ball.rect.pos += ball.speed * dt;

	// Ball collision
	if (ball.rect.pos.y <= 0)
	{
		ball.speed *= 0.9 * glm::dvec2(1.2, -1.0);
		ball.rect.pos.y = 0;
	}
	else if (ball.rect.pos.y + ball.rect.size.y >= vidmode.height)
		platform_wnd.setShouldClose(true);

	if (ball.rect.pos.x <= 0)
	{
		ball.speed *= 0.9 * glm::dvec2(-1.0, 1.2);
		ball.rect.pos.x = 0;
	}
	else if (ball.rect.pos.x + ball.rect.size.x >= vidmode.width)
	{
		ball.speed *= 0.9 * glm::dvec2(-1.0, 1.2);
		ball.rect.pos.x = vidmode.width - ball.rect.size.x;
	}

	if (ball.rect.pos.y + ball.rect.size.y >= platform.rect.pos.y && ball.rect.pos.x >= platform.rect.pos.x && ball.rect.pos.x + ball.rect.size.x <= platform.rect.pos.x + platform.rect.size.x)
	{
		ball.speed *= 0.9 * glm::dvec2(1.0, -1.0);
		ball.speed += 0.5 * platform.speed;
		ball.rect.pos.y = platform.rect.pos.y - ball.rect.size.y;
	}

	platform_wnd.setPosition(platform.rect.pos);
	ball_wnd.setPosition(ball.rect.pos);
}

int main2()
{
	try
	{
		sp::init();
		double last_time = sp::getTime();
		double current_time = 0, delta = 0;

		sp::Monitor* primary = sp::Monitor::getPrimary();
		vidmode = primary->getCurrentVideoMode();

		ball_wnd.setFlag(sp::WindowFlagB::Focused, false);

		platform_wnd.create({ 400, 30 }, "Platform");
		ball_wnd.create({ 70, 70 }, "Ball");

		platform_wnd.onKeyPressed(onKeyPressed);
		platform_wnd.onKeyReleased(onKeyReleased);

		int32_t frameSize[4] = { 0 };

		platform_wnd.getFrameSize(&frameSize[0], &frameSize[1], &frameSize[2], &frameSize[3]);

		platform_wnd.setSize({ 400 - frameSize[1] - frameSize[3], 30 - frameSize[0] - frameSize[2] });
		ball_wnd.setSize({ 130 - frameSize[1] - frameSize[3], 130 - frameSize[0] - frameSize[2] });

		platform.speed = glm::vec2(0.0f);
		platform.rect.size = { 400, 30, };
		platform.rect.pos = { (vidmode.width - platform.rect.size.x) / 2, vidmode.height - 150 };

		ball.speed = glm::vec2(600.0, -600.0);
		ball.rect.size = { 130, 130 };
		ball.rect.pos = { (vidmode.width - ball.rect.size.x) / 2, (vidmode.height - ball.rect.size.y) / 2 };

		while (!platform_wnd.shouldClose() && !ball_wnd.shouldClose())
		{
			sp::pollEvents();

			double current_time = sp::getTime();
			delta = current_time - last_time;
			last_time = current_time;

			update(delta);
		}

		sp::finish();
	}
	catch (const sp::Error& err)
	{
		std::cout << "An error happened. Code: " << err.code << "\nDescription: " << err.description << std::endl;
	}

	return 0;
}