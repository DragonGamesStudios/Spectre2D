#include "..\include\Spectre2D\ObjectWithBuffer.h"

#include <GL/glew.h>

namespace sp
{
	ObjectWithBuffer::ObjectWithBuffer(uint32_t _binding, DataUsage _usage, bool lock)
		: LockableObject(lock)
	{
		usage = _usage;
		id = 0;
		rebufferNeeded = false;
		binding = _binding;
	}

	ObjectWithBuffer::~ObjectWithBuffer()
	{
		// Delete the previous buffer
		if (id)
			glDeleteBuffers(1, &id);

		id = 0;
	}

	uint32_t ObjectWithBuffer::getOpenGLID() const
	{
		return id;
	}

	bool ObjectWithBuffer::buffer()
	{
		if (rebufferNeeded)
		{
			rebuffer();
			return true;
		}

		return false;
	}

	void ObjectWithBuffer::setDataUsage(DataUsage _usage)
	{
		usage = _usage;

		if (!getLocked())
			rebuffer();
		else
			rebufferNeeded = true;
	}

	DataUsage ObjectWithBuffer::getDataUsage() const
	{
		return usage;
	}

	void ObjectWithBuffer::setLocked(bool value)
	{
		LockableObject::setLocked(value);

		if (!value)
		{
			if (rebufferNeeded)
				rebuffer();

			// Any buffer requests should be cleared when calling rebuffer(), so we don't need an if here.
			buffer();
		}
	}

	void ObjectWithBuffer::requestRebuffer()
	{
		if (!getLocked())
			rebuffer();
		else
			rebufferNeeded = true;
	}

	void ObjectWithBuffer::requestBuffer()
	{
		if (!getLocked())
			buffer();
	}

	void ObjectWithBuffer::rebuffer()
	{
		// Delete the previous buffer
		if (id)
			glDeleteBuffers(1, &id);

		// Generate new buffer
		glGenBuffers(1, &id);

		rebufferNeeded = false;
	}

	uint32_t ObjectWithBuffer::getBinding() const
	{
		return binding;
	}

	void ObjectWithBuffer::bufferData(size_t size, const void* data)
	{
		glBufferData(binding, size, data, usage == DataUsage::Static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	}

	void ObjectWithBuffer::bufferSubData(size_t offset, size_t size, const void* data)
	{
		glBufferSubData(binding, offset, size, data);
	}

	void ObjectWithBuffer::bind() const
	{
		glBindBuffer(binding, id);
	}

}