#include "..\include\Spectre2D\RenderWindow.h"
#include "..\include\Spectre2D\RenderTexture.h"
#include "..\include\Spectre2D\core.h"

#include "..\extlibs\glew\include\GL\glew.h"

namespace sp
{
	RenderWindow::RenderWindow()
		: activeRenderTexture(0), Window(), RenderTarget()
	{
	}

	RenderWindow::RenderWindow(const glm::ivec2& _size, const std::string& _caption, Monitor* monitor, Window* share)
		: activeRenderTexture(0), Window(_size, _caption, monitor, share), RenderTarget(_size)
	{
	}

	void RenderWindow::create(const glm::uvec2& size, const std::string& caption, Monitor* monitor, Window* share)
	{
		Window::create(size, caption, monitor, share);

		if (glewInit())
			throw Error(SPECTRE_NOT_INITIALIZED, "OpenGL could not have been initialized.");

		RenderTarget::setSize(getSize());

		glEnable(GL_PROGRAM_POINT_SIZE);

		glViewport(0, 0, getSize().x, getSize().y);
		defaultShader.create(this, ShaderSource::getDefaultVertexShaderSource(), ShaderSource::getDefaultFragmentShaderSource());

		setActiveShader(&defaultShader);
		setRenderActive(true);
	}

	void RenderWindow::setActiveRenderTexture(RenderTexture* texture)
	{
		if (texture)
		{
			setRenderActive(false);

			// TODO: Bind the framebuffer, update glViewport() etc.
			activeRenderTexture = texture;

			if (!texture->getActiveShader())
				texture->setActiveShader(&defaultShader);

			texture->setRenderActive(true);
		}
		else
		{
			if (activeRenderTexture)
				texture->setRenderActive(false);

			activeRenderTexture = nullptr;

			setRenderActive(true);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, getSize().x, getSize().y);
		}
	}

	void RenderWindow::setSize(const glm::uvec2& size)
	{
		Window::setSize(size);
		RenderTarget::setSize(size);

		if (!activeRenderTexture)
			glViewport(0, 0, size.x, size.y);
	}

	void RenderWindow::updateSize()
	{
		Window::updateSize();

		if (!activeRenderTexture)
			glViewport(0, 0, getSize().x, getSize().y);

		RenderTarget::setSize(getSize());
	}

	const Shader& RenderWindow::getDefaultShader() const
	{
		return defaultShader;
	}

	void RenderWindow::setRenderActive(bool value)
	{
		RenderTarget::setRenderActive(value);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, getSize().x, getSize().y);
	}

}