#include "..\include\Spectre2D\ResizableObject.h"

namespace sp
{
	ResizableObject::ResizableObject(const glm::uvec2& _size)
	{
		size = _size;
	}

	const glm::uvec2& ResizableObject::getSize() const
	{
		return size;
	}

	void ResizableObject::setSize(const glm::uvec2& _size)
	{
		size = _size;
	}
}