#include "..\include\Spectre2D\Mesh.h"
#include "..\include\Spectre2D\core.h"

#include "..\extlibs\glew\include\GL\glew.h"

namespace sp
{
	Mesh::Mesh(Primitive _primitive, DataUsage usage, size_t vertexAmount, size_t _offset, const std::vector<size_t>& _indices, bool lock)
		: ObjectWithBuffer(GL_ELEMENT_ARRAY_BUFFER, usage, lock)
	{
		primitive = getOpenGLPrimitive(_primitive);

		indices = _indices;
		offset = _offset;

		indices.resize(vertexAmount);

		for (size_t i = 0; i < vertexAmount && i < _indices.size(); i++)
			indices[i] = offset + _indices[i];
		
		for (size_t i = _indices.size(); i < vertexAmount; i++)
			indices.push_back(offset + i);

		requestRebuffer();
	}

	int32_t Mesh::setVertexAmount(size_t amount)
	{
		int32_t diff = (int32_t)amount - indices.size();

		if (diff)
		{
			size_t lastSize = indices.size();

			indices.resize(amount);

			size_t newSize = indices.size();

			for (size_t i = lastSize; i < newSize; i++)
				indices[i] = offset + i;

			requestRebuffer();
		}

		return diff;
	}

	size_t Mesh::getVertexAmount() const
	{
		return indices.size();
	}

	void Mesh::setIndices(const std::vector<size_t>& _indices, size_t _offset)
	{
		for (size_t i = 0; i < _indices.size() && i < indices.size() - _offset; i++)
		{
			indices[_offset + i] = offset + _indices[i];
		}

		bufferRequests.push_back({ _offset, std::min(_indices.size(), indices.size() - _offset) });

		requestBuffer();
	}

	const std::vector<size_t>& Mesh::getIndices() const
	{
		return indices;
	}

	void Mesh::setVertexIndex(size_t vertexIndex, size_t index)
	{
		if (vertexIndex >= indices.size())
			throw Error(SPECTRE_INVALID_VALUE, "Attempt to set index of vertex ot of range.");

		indices[vertexIndex] = offset + index;

		bufferRequests.push_back({ vertexIndex, 1 });

		requestBuffer();
	}

	size_t Mesh::getVertexIndex(size_t vertexIndex) const
	{
		if (vertexIndex >= indices.size())
			throw Error(SPECTRE_INVALID_VALUE, "Attempt to get index of vertex ot of range.");

		return indices[vertexIndex];
	}

	bool Mesh::buffer()
	{
		if (ObjectWithBuffer::buffer())
			return true;

		// Bind the buffer
		bind();

		for (const auto& request : bufferRequests)
		{
			// Buffer each request
			bufferSubData(request.first * sizeof(size_t), request.second * sizeof(size_t), &indices.data()[request.first]);
		}

		bufferRequests.clear();

		return false;
	}

	void Mesh::setOffset(size_t newOffset, bool fixIndices)
	{
		if (fixIndices)
		{
			int32_t diff = (uint32_t)newOffset - offset;

			// Offset the indices
			for (auto& index : indices)
				index = (size_t)((int32_t)index + diff);

			// Since all the indices most likely changed, we can clear the buffer queue and add a full buffer request.
			bufferRequests.clear();

			bufferRequests.push_back({ 0, indices.size() });
		}

		offset = newOffset;
	}

	size_t Mesh::getOffset() const
	{
		return offset;
	}

	void Mesh::setPrimitive(Primitive _primitive)
	{
		primitive = getOpenGLPrimitive(_primitive);
	}

	Primitive Mesh::getPrimitive() const
	{
		return getSpectrePrimitive(primitive);
	}

	uint32_t Mesh::getRawPrimitive() const
	{
		return primitive;
	}

	uint32_t Mesh::getOpenGLPrimitive(Primitive spPrimitive)
	{
		GLenum pr = 0;

		switch (spPrimitive)
		{
		case Primitive::Points:
			pr = GL_POINTS;
			break;

		case Primitive::Lines:
			pr = GL_LINES;
			break;

		case Primitive::LineStrip:
			pr = GL_LINE_STRIP;
			break;

		case Primitive::LineLoop:
			pr = GL_LINE_LOOP;
			break;

		case Primitive::Triangles:
			pr = GL_TRIANGLES;
			break;

		case Primitive::TriangleStrip:
			pr = GL_TRIANGLE_STRIP;
			break;

		case Primitive::TriangleFan:
			pr = GL_TRIANGLE_FAN;
			break;
		}

		return pr;
	}

	Primitive Mesh::getSpectrePrimitive(uint32_t openglPrimitive)
	{
		Primitive pr = Primitive::Points;

		switch (openglPrimitive)
		{
		case GL_POINTS:
			pr = Primitive::Points;
			break;

		case GL_LINES:
			pr = Primitive::Lines;
			break;

		case GL_LINE_STRIP:
			pr = Primitive::LineStrip;
			break;

		case GL_LINE_LOOP:
			pr = Primitive::LineLoop;
			break;

		case GL_TRIANGLES:
			pr = Primitive::Triangles;
			break;

		case GL_TRIANGLE_FAN:
			pr = Primitive::TriangleFan;
			break;

		case GL_TRIANGLE_STRIP:
			pr = Primitive::TriangleStrip;
			break;
		}

		return pr;
	}

	void Mesh::rebuffer()
	{
		ObjectWithBuffer::rebuffer();

		// Bind the buffer
		bind();

		// And buffer the data
		bufferData(indices.size() * sizeof(size_t), indices.data());

		// Clear requests
		bufferRequests.clear();
	}

}