#include "..\include\Spectre2D\Drawable.h"

namespace sp
{
	Drawable::Drawable()
	{
		tint = glm::vec4(1.0f);
	}

	const glm::vec4& Drawable::getTint() const
	{
		return tint;
	}

	void Drawable::setTint(const glm::vec4& _tint)
	{
		tint = _tint;
	}

}