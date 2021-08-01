#include "..\include\Spectre2D\RenderTarget.h"

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

namespace sp
{
	RenderTarget::RenderTarget(const glm::uvec2& size, bool lock)
		: ResizableObject(size), Transformable(lock)
	{
		view = projection = model = glm::mat4(1.0f);
		updateProjection = true;
		pointSize = 1.0f;
		lineWidth = 1.0f;

		activeShader = nullptr;

		create(size);
	}

	void RenderTarget::create(const glm::uvec2& size)
	{
		ResizableObject::setSize(size);

		updateSize();
	}

	void RenderTarget::clear()
	{
		glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

		// We're clearing the depth and stencil buffers as well to prevent any trouble when the user uses raw OpenGL.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void RenderTarget::setBackgroundColor(const glm::vec4& color)
	{
		backgroundColor = color;
	}

	const glm::vec4& RenderTarget::getBackgroundColor() const
	{
		return backgroundColor;
	}

	const glm::mat4& RenderTarget::getProjection() const
	{
		return projection;
	}

	void RenderTarget::setProjection(const glm::mat4& matrix)
	{
		projection = matrix;

		if (activeShader && active)
			activeShader->setUniform("sp_viewProj", getProjection() * getView());
	}

	const glm::mat4& RenderTarget::getView() const
	{
		return view;
	}

	void RenderTarget::setView(const glm::mat4& matrix)
	{
		view = matrix;

		if (activeShader && active)
			activeShader->setUniform("sp_viewProj", getProjection() * getView());
	}

	const glm::mat4& RenderTarget::getModel() const
	{
		return model;
	}

	void RenderTarget::setModel(const glm::mat4& matrix)
	{
		model = matrix;

		if (activeShader && active)
			activeShader->setUniform("sp_model", evaluateTransform() * getModel());
	}

	void RenderTarget::setTint(const glm::vec4& _tint)
	{
		tint = _tint;

		if (activeShader && active)
			activeShader->setUniform("sp_tint", tint);
	}

	const glm::vec4& RenderTarget::getTint() const
	{
		return tint;
	}

	void RenderTarget::setSize(const glm::uvec2& size)
	{
		ResizableObject::setSize(size);

		updateSize();
	}

	const glm::uvec2& RenderTarget::getSize() const
	{
		return ResizableObject::getSize();
	}

	void RenderTarget::setTransformSize(size_t _size)
	{
		Transformable::setSize(_size);
	}

	size_t RenderTarget::getTransformSize() const
	{
		return Transformable::getSize();
	}

	void RenderTarget::setUpdateProjection(bool value)
	{
		updateProjection = value;
	}

	bool RenderTarget::getUpdateProjection() const
	{
		return updateProjection;
	}

	void RenderTarget::updateSize()
	{
		if (updateProjection)
		{
			setProjection(glm::ortho(0.0f, (float_t)ResizableObject::getSize().x, (float_t)ResizableObject::getSize().y, 0.0f, 0.0f, 100.f));

			if (activeShader && active)
				activeShader->setUniform("sp_viewProj", getProjection() * getView());
		}
	}

	void RenderTarget::draw(Drawable* drawable)
	{
		drawable->draw(this);
	}

	bool RenderTarget::getRenderActive() const
	{
		return active;
	}

	void RenderTarget::setActiveShader(Shader* shader)
	{
		activeShader = shader;

		if (shader)
		{
			glUseProgram(shader->getOpenGLID());

			shader->setUniform("sp_viewProj", getProjection() * getView());
			shader->setUniform("sp_model", getModel());
			shader->setUniform("sp_tint", getTint());
			shader->setUniform("sp_pointSize", getPointSize());
		}
		else
		{
			glUseProgram(0);
			glBindVertexArray(0);
		}
	}

	Shader* RenderTarget::getActiveShader() const
	{
		return activeShader;
	}

	void RenderTarget::setPointSize(float_t value)
	{
		pointSize = value;

		if (activeShader && active)
			activeShader->setUniform("sp_pointSize", value);
	}

	float_t RenderTarget::getPointSize() const
	{
		return pointSize;
	}

	void RenderTarget::setLineWidth(float_t value)
	{
		lineWidth = value;

		if (active)
			glLineWidth(value);
	}

	float_t RenderTarget::getLineWidth() const
	{
		return lineWidth;
	}

	void RenderTarget::setSmoothLines(bool smooth)
	{
		smoothLines = smooth;

		if (active)
		{
			if (smooth)
				glEnable(GL_LINE_SMOOTH);
			else
				glDisable(GL_LINE_SMOOTH);
		}
	}

	bool RenderTarget::getSmoothLines() const
	{
		return smoothLines;
	}

	void RenderTarget::setRenderActive(bool value)
	{
		active = value;

		if (activeShader)
		{
			glUseProgram(activeShader->getOpenGLID());

			activeShader->setUniform("sp_viewProj", getProjection() * getView());
			activeShader->setUniform("sp_model", evaluateTransform() * getModel());
			activeShader->setUniform("sp_tint", getTint());
			activeShader->setUniform("sp_pointSize", getPointSize());
			glLineWidth(getLineWidth());

			if (smoothLines)
				glEnable(GL_LINE_SMOOTH);
			else
				glDisable(GL_LINE_SMOOTH);

			// Set uniforms
			activeShader->setRawUniforms();
		}
	}

}