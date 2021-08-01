#include "..\include\Spectre2D\VertexData.h"
#include "..\include\Spectre2D\Shader.h"

#include <GL/glew.h>

#include <set>
#include <iostream>

sp::VertexDataUnit operator*(const sp::VertexDataUnit& left, int32_t right)
{
	sp::VertexDataUnit unit = left;
	unit.size *= right;
	return unit;
}

namespace sp
{
	VertexDataStream::VertexDataStream()
	{
		size = 0;
	}

	VertexDataStream& VertexDataStream::operator<<(const VertexDataStream& right)
	{
		push(right);
		return *this;
	}

	VertexDataStream& VertexDataStream::operator<<(const VertexDataUnit& right)
	{
		push(right);
		return *this;
	}

	void VertexDataStream::push(const VertexDataStream& right)
	{
		for (const auto& unit : right.getUnits())
			push(unit);
	}

	void VertexDataStream::push(const VertexDataUnit& right)
	{
		units.push_back(right);
		size += right.bytesize();
	}

	void VertexDataStream::clear()
	{
		units.clear();
	}

	size_t VertexDataStream::bytesize() const
	{
		return size;
	}

	const std::vector<VertexDataUnit>& VertexDataStream::getUnits() const
	{
		return units;
	}

	VertexDataUnit VertexDataStream::Int(const std::string& name)
	{
		return VertexDataUnit{VertexDataType::Int, 1, 1, name};
	}

	VertexDataUnit VertexDataStream::Uint(const std::string& name)
	{
		return VertexDataUnit{ VertexDataType::Uint, 1, 1, name };
	}

	VertexDataUnit VertexDataStream::Float(const std::string& name)
	{
		return VertexDataUnit{ VertexDataType::Float, 1, 1, name };
	}

	VertexDataUnit VertexDataStream::Double(const std::string& name)
	{
		return VertexDataUnit{ VertexDataType::Double, 1, 1, name };
	}

	VertexDataUnit VertexDataStream::Bool(const std::string& name)
	{
		return VertexDataUnit{ VertexDataType::Bool, 1, 1, name };
	}

	VertexDataUnit VertexDataStream::Empty(const std::string& name)
	{
		return VertexDataUnit{ VertexDataType::Empty, 1, 1, name };
	}

	VertexDataUnit VertexDataStream::Ignore()
	{
		return VertexDataUnit{ VertexDataType::Ignore, 1, 1, "" };
	}

	VertexDataUnit VertexDataStream::Vec(const std::string& name, size_t size)
	{
		return VertexDataUnit{ VertexDataType::Float, 1, size, name };
	}

	VertexDataUnit VertexDataStream::Ivec(const std::string& name, size_t size)
	{
		return VertexDataUnit{ VertexDataType::Int, 1, size, name };
	}

	VertexDataUnit VertexDataStream::Uvec(const std::string& name, size_t size)
	{
		return VertexDataUnit{ VertexDataType::Uint, 1, size, name };
	}

	VertexDataUnit VertexDataStream::Dvec(const std::string& name, size_t size)
	{
		return VertexDataUnit{ VertexDataType::Double, 1, size, name };
	}

	VertexDataUnit VertexDataStream::Bvec(const std::string& name, size_t size)
	{
		return VertexDataUnit{ VertexDataType::Bool, 1, size, name };
	}

	VertexDataUnit VertexDataStream::Mat(const std::string& name, size_t columns, size_t rows)
	{
		return VertexDataUnit{ VertexDataType::Float, 1, columns, name, rows };
	}

	VertexDataUnit VertexDataStream::Imat(const std::string& name, size_t columns, size_t rows)
	{
		return VertexDataUnit{ VertexDataType::Int, 1, columns, name, rows };
	}

	VertexDataUnit VertexDataStream::Umat(const std::string& name, size_t columns, size_t rows)
	{
		return VertexDataUnit{ VertexDataType::Uint, 1, columns, name, rows };
	}

	VertexDataUnit VertexDataStream::Dmat(const std::string& name, size_t columns, size_t rows)
	{
		return VertexDataUnit{ VertexDataType::Double, 1, columns, name, rows };
	}

	VertexDataUnit VertexDataStream::Bmat(const std::string& name, size_t columns, size_t rows)
	{
		return VertexDataUnit{ VertexDataType::Bool, 1, columns, name, rows };
	}

	VertexDataUnit VertexDataStream::Tex2D(const std::string& name)
	{
		return VertexDataUnit{ VertexDataType::Texture2D, 1, 1, name };
	}

	size_t VertexDataUnit::bytesize() const
	{
		size_t typesize = size * innerSize * (matrixRows ? matrixRows : innerSize) * valueBytesize();

		return typesize;
	}

	size_t VertexDataUnit::valueBytesize() const
	{
		size_t typesize = 1;

		switch (type)
		{
		case VDT::Empty:
		case VDT::Bool:
			typesize = 1;
			break;

		case VDT::Double:
			typesize = sizeof(double);
			break;

		case VDT::Float:
			typesize = sizeof(float);
			break;

		case VDT::Int:
			typesize = sizeof(int);
			break;

		case VDT::Uint:
			typesize = sizeof(unsigned int);
			break;

		case VDT::Texture2D:
			typesize = sizeof(int);
			break;

		case VDT::Ignore:
			break;
		}

		return typesize;
	}

	size_t VertexDataUnit::locationIncrease() const
	{
		// Every location in glsl has 16 bytes. To get the increase in the location of a unit, we need to get its part size and ceil it.
		return (type != VDT::Empty) ? (((bytesize() / size) + 15) / 16) : 0;
	}

	VertexData::VertexData(DataUsage usage, size_t _vertexAmount, bool lock)
		: ObjectWithBuffer(GL_ARRAY_BUFFER, usage, lock)
	{
		VAO = 0;
		vertexAmount = _vertexAmount;

		setLayout(Shader::getDefaultVertexInterface());
	}

	VertexData::~VertexData()
	{
		if (VAO)
			glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	void VertexData::setLayout(const VertexDataStream& stream)
	{
		layout = stream;

		// Fill in the values
		std::map<std::string, std::tuple<size_t, size_t, VertexAttribVector>> existing = values;

		values.clear();

		size_t offset = 0;
		std::string name;

		for (const auto& unit : layout.getUnits())
		{
			name = unit.name;
			size_t matrixRows = unit.matrixRows ? unit.matrixRows : unit.innerSize;
			VertexAttribVector empty;

			switch (unit.type)
			{
			case VDT::Bool:
				empty = std::vector<bool>();
				break;

			case VDT::Double:
				empty = std::vector<double_t>();
				break;

			case VDT::Empty:
				offset += unit.size;
				continue;
				break;

			case VDT::Float:
				empty = std::vector<float_t> ();
				break;

			case VDT::Ignore:
				continue;
				break;

			case VDT::Int:
				empty = std::vector<int32_t>();
				break;

			case VDT::Uint:
				empty = std::vector<uint32_t>();
				break;

			case VDT::Texture2D:
				throw Error(SPECTRE_INVALID_VALUE, "Vertex attribute of type `sampler2D` (VertexDataType::Texture2D) is not supported.");
				break;
			}

			for (size_t i = 0; i < unit.size; i++)
			{
				std::string name = unit.name;

				if (unit.size > 1)
					name += "[" + std::to_string(i) + "]";

				for (size_t j = 0; j < matrixRows; j++)
				{
					std::string subname = name;

					if (matrixRows > 1)
						subname += "[" + std::to_string(j) + "]";

					std::map<std::string, std::tuple<size_t, size_t, VertexAttribVector>>::iterator it = values.insert({ subname, { offset, unit.innerSize, empty } }).first;

					auto found = existing.find(subname);

					if (found != existing.end())
					{
						std::get<2>(it->second) = std::get<2>(found->second);
						existing.erase(found);
					}

					offset += unit.valueBytesize() * unit.innerSize;
				}
			}
		}

		// Fill the new vectors
		setVertexAmount(vertexAmount, true, true);

		// After setVertexAmount(), the data will either be rebuffered or a rebuffer will be requested. Same goes for sub-buffering.
	}

	const VertexDataStream& VertexData::getLayout() const
	{
		return layout;
	}

	int32_t VertexData::setVertexAmount(size_t amount, bool shrink, bool ignoreDiff)
	{
		int32_t diff = (int32_t)amount - vertexAmount;

		if (diff > 0 || (diff && shrink) || ignoreDiff)
		{
			vertexAmount = amount;

			for (auto& [key, value] : values)
			{
				auto& vertexAttribValues = std::get<2>(value);

				switch (vertexAttribValues.index())
				{
				case 0:
					// Double
					std::get<std::vector<double_t>>(vertexAttribValues).resize(amount * std::get<1>(value), 0.0);
					break;

				case 1:
					// Float
					std::get<std::vector<float_t>>(vertexAttribValues).resize(amount * std::get<1>(value), 0.0f);
					break;

				case 2:
					// Bool
					std::get<std::vector<bool>>(vertexAttribValues).resize(amount * std::get<1>(value), 0);
					break;

				case 3:
					// Int
					std::get<std::vector<int32_t>>(vertexAttribValues).resize(amount * std::get<1>(value), 0);
					break;

				case 4:
					// Uint
					std::get<std::vector<uint32_t>>(vertexAttribValues).resize(amount * std::get<1>(value), 0);
					break;
				}
			}

			requestRebuffer();
		}

		return diff;
	}

	size_t VertexData::getVertexAmount() const
	{
		return vertexAmount;
	}

	size_t VertexData::bytesize() const
	{
		return layout.bytesize() * vertexAmount;
	}

	bool VertexData::buffer()
	{
		if (ObjectWithBuffer::buffer())
			return true;

		bind();

		// Buffer the values
		size_t layoutSize = layout.bytesize();
		size_t maxAttribSize = 1;

		for (const auto& request : bufferRequests)
			if (std::get<1>(request.first->second) > maxAttribSize)
				maxAttribSize = std::get<1>(request.first->second);

		// Buffers
		// +1 so that the compiler shuts up
		double_t* dbuff = new double_t[maxAttribSize + 1]{ 0 };
		float_t* fbuff = new float_t[maxAttribSize + 1]{ 0 };
		bool* bbuff = new bool[maxAttribSize + 1]{ 0 };
		int32_t* ibuff = new int32_t[maxAttribSize + 1]{ 0 };
		uint32_t* ubuff = new uint32_t[maxAttribSize + 1]{ 0 };

		for (auto& request : bufferRequests)
		{
			auto& value = request.first->second;

			size_t offset = std::get<0>(value) + request.second * layoutSize;
			size_t attribSize = std::get<1>(value);
			const auto& attribValues = std::get<2>(value);
			void* data = 0;
			size_t typesize = 1;

			// Decide which buffer to use
			// And buffer the data of the right type
			switch (attribValues.index())
			{
			case 0:
				// Double
				typesize = sizeof(double_t);
				data = dbuff;
				for (size_t j = 0; j < attribSize; j++)
					dbuff[j] = std::get<std::vector<double_t>>(attribValues)[request.second * attribSize + j];
				break;

			case 1:
				// Float
				typesize = sizeof(float_t);
				data = fbuff;
				for (size_t j = 0; j < attribSize; j++)
					fbuff[j] = std::get<std::vector<float_t>>(attribValues)[request.second * attribSize + j];
				break;

			case 2:
				// Bool
				typesize = sizeof(bool);
				data = bbuff;
				for (size_t j = 0; j < attribSize; j++)
					bbuff[j] = std::get<std::vector<bool>>(attribValues)[request.second * attribSize + j];
				break;

			case 3:
				// Int
				typesize = sizeof(int32_t);
				data = ibuff;
				for (size_t j = 0; j < attribSize; j++)
					ibuff[j] = std::get<std::vector<int32_t>>(attribValues)[request.second * attribSize + j];
				break;

			case 4:
				// Uint
				typesize = sizeof(uint32_t);
				data = ubuff;
				for (size_t j = 0; j < attribSize; j++)
					ubuff[j] = std::get<std::vector<uint32_t>>(attribValues)[request.second * attribSize + j];
				break;
			}

			bufferSubData(offset, attribSize * typesize, data);
		}

		// Free the buffers
		delete[] dbuff, fbuff, bbuff, ibuff, ubuff;

		// Clear requests
		bufferRequests.clear();

		return false;
	}

	uint32_t VertexData::getOpenGLVAO() const
	{
		return VAO;
	}

	bool VertexData::hasVertexAttrib(const std::string& attrib) const
	{
		return values.find(attrib) != values.end();
	}

	void VertexData::setVertexPosition(size_t vertexIndex, const glm::vec2& position)
	{
		setVertexAttribValue("sp_vertexPosition", vertexIndex, glm::vec3(position, 0.0f));
	}

	void VertexData::setVertexPosition(size_t vertexIndex, const glm::vec3& position)
	{
		setVertexAttribValue("sp_vertexPosition", vertexIndex, position);
	}

	glm::vec3 VertexData::getVertexPosition(size_t vertexIndex) const
	{
		return getVertexAttribValue<float_t, 3>("sp_vertexPosition", vertexIndex);
	}

	void VertexData::setVertexColor(size_t vertexIndex, const glm::vec4& color)
	{
		setVertexAttribValue("sp_vertexColor", vertexIndex, color);
	}

	glm::vec4 VertexData::getVertexColor(size_t vertexIndex) const
	{
		return getVertexAttribValue<float_t, 4>("sp_vertexColor", vertexIndex);
	}

	void VertexData::setVertexUV(size_t vertexIndex, const glm::vec2& uv)
	{
		setVertexAttribValue("sp_vertexUV", vertexIndex, uv);
	}

	glm::vec2 VertexData::getVertexUV(size_t vertexIndex) const
	{
		return getVertexAttribValue<float_t, 2>("sp_vertexUV", vertexIndex);
	}

	void VertexData::bind() const
	{
		ObjectWithBuffer::bind();

		glBindVertexArray(VAO);
	}

	const std::map<std::string, std::tuple<size_t, size_t, VertexAttribVector>>& VertexData::getRaw() const
	{
		return values;
	}

	void VertexData::bufferVAO()
	{
		GLuint attribIndex = 0;
		GLuint attribBeginning = 0;

		// Clear the previous vao
		if (VAO)
			glDeleteVertexArrays(1, &VAO);

		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);

		bind();

		for (const auto& unit : layout.getUnits())
		{
			int32_t matrixRows = unit.matrixRows ? unit.matrixRows : unit.innerSize;

			// For each vertex unit
			switch (unit.type)
			{
			case VDT::Ignore:
				// Increase
				for (size_t i = 0; i < unit.size; i++)
				{
					glDisableVertexArrayAttrib(VAO, attribIndex);
					attribIndex++;
				}

				break;
			case VDT::Empty:
				// If empty/ignore, do nothing. The attrib offset and location will be increased anyway after the switch.
				break;

			default:
				// Otherwise, first specify the OpenGL enum form the vertex attrib type...
				GLenum attribType = 0;

				switch (unit.type)
				{
				case VDT::Bool:
					attribType = GL_BOOL;
					break;

				case VDT::Double:
					attribType = GL_DOUBLE;
					break;

				case VDT::Float:
					attribType = GL_FLOAT;
					break;

				case VDT::Int:
					attribType = GL_INT;
					break;

				case VDT::Uint:
					attribType = GL_UNSIGNED_INT;
					break;
				}

				// ... then specify the vertex attribute pointers. If the type is a matrix, we use 4 locations.
				for (size_t i = 0; i < unit.size * matrixRows; i++)
				{
					glVertexAttribPointer(attribIndex, unit.innerSize, attribType, GL_FALSE, layout.bytesize(), (void*)attribBeginning);
					glEnableVertexArrayAttrib(VAO, attribIndex);

					// Every location in glsl has 16 bytes. To get the increase in the location of a unit, we need to get its part size and ceil it.
					attribIndex += unit.locationIncrease();
				}

				break;
			}

			// Increase the offset by the unit's byte size
			attribBeginning += unit.bytesize();
		}
	}

	void VertexData::shift(size_t start, size_t _size, size_t newIndex)
	{
		for (auto it = values.begin(); it != values.end(); it++)
		{
			std::tuple<size_t, size_t, VertexAttribVector>& value = it->second;

			size_t attribSize = std::get<1>(value);
			auto& attribValues = std::get<2>(value);

			for (int32_t i = ((int32_t)start + _size - 1) * attribSize; i >= (int32_t)(start * attribSize); i--)
			{
				size_t index = (newIndex - start) * attribSize + i;

				switch (attribValues.index())
				{
				case 0:
					// Double
					std::get<std::vector<double_t>>(attribValues)[index] = std::get<std::vector<double_t>>(attribValues)[i];
					break;

				case 1:
					// Float
					std::get<std::vector<float_t>>(attribValues)[index] = std::get<std::vector<float_t>>(attribValues)[i];
					break;

				case 2:
					// Bool
					std::get<std::vector<bool>>(attribValues)[index] = std::get<std::vector<bool>>(attribValues)[i];
					break;

				case 3:
					// Int
					std::get<std::vector<int32_t>>(attribValues)[index] = std::get<std::vector<int32_t>>(attribValues)[i];
					break;

				case 4:
					// Uint
					std::get<std::vector<uint32_t>>(attribValues)[index] = std::get<std::vector<uint32_t>>(attribValues)[i];
					break;
				}

				bufferRequests.push_back({ it, index });
			}
		}

		requestBuffer();
	}

	void VertexData::rebuffer()
	{
		ObjectWithBuffer::rebuffer();

		// Fill empty data
		bind();

		char* empty = new char[bytesize()];

		// Fill with zeros
		for (size_t i = 0; i < bytesize(); i++)
		{
			empty[i] = 0;
		}
		
		bufferData(bytesize(), empty);

		delete[] empty;

		// Clear buffer requests, since all the data will be rebuffered anyway.
		bufferRequests.clear();

		// Buffer the values
		size_t layoutSize = layout.bytesize();
		size_t maxAttribSize = 1;

		for (const auto& [key, value] : values)
			if (std::get<1>(value) > maxAttribSize)
				maxAttribSize = std::get<1>(value);

		// Buffers
		// +1 so that the compiler shuts up
		double_t* dbuff = new double_t[maxAttribSize + 1]{ 0 };
		float_t* fbuff = new float_t[maxAttribSize + 1]{ 0 };
		bool* bbuff = new bool[maxAttribSize + 1] { 0 };
		int32_t* ibuff = new int32_t[maxAttribSize + 1]{ 0 };
		uint32_t* ubuff = new uint32_t[maxAttribSize + 1]{ 0 };

		for (const auto& [key,  value] : values)
		{
			size_t offset = std::get<0>(value);
			size_t attribSize = std::get<1>(value);
			const auto& attribValues = std::get<2>(value);
			void* data = 0;

			// Decide which buffer to use
			switch (attribValues.index())
			{
			case 0:
				data = dbuff;
				break;

			case 1:
				data = fbuff;
				break;

			case 2:
				data = bbuff;
				break;

			case 3:
				data = ibuff;
				break;

			case 4:
				data = ubuff;
				break;
			}

			for (size_t i = 0; i < vertexAmount; i++)
			{
				size_t typesize = 0;

				// Buffer the data of the right type
				switch (attribValues.index())
				{
				case 0:
					// Double
					typesize = sizeof(double_t);
					for (size_t j = 0; j < attribSize; j++)
						dbuff[j] = std::get<std::vector<double_t>>(attribValues)[i * attribSize + j];
					break;

				case 1:
					// Float
					typesize = sizeof(float_t);
					for (size_t j = 0; j < attribSize; j++)
						fbuff[j] = std::get<std::vector<float_t>>(attribValues)[i * attribSize + j];
					break;

				case 2:
					// Bool
					typesize = sizeof(bool);
					for (size_t j = 0; j < attribSize; j++)
						bbuff[j] = std::get<std::vector<bool>>(attribValues)[i * attribSize + j];
					break;

				case 3:
					// Int
					typesize = sizeof(int32_t);
					for (size_t j = 0; j < attribSize; j++)
						ibuff[j] = std::get<std::vector<int32_t>>(attribValues)[i * attribSize + j];
					break;

				case 4:
					// Uint
					typesize = sizeof(uint32_t);
					for (size_t j = 0; j < attribSize; j++)
						ubuff[j] = std::get<std::vector<uint32_t>>(attribValues)[i * attribSize + j];
					break;
				}

				bufferSubData(offset, attribSize * typesize, data);

				offset += layoutSize;
			}
		}

		// Free the buffers
		delete[] dbuff, fbuff, bbuff, ibuff, ubuff;

		// Rebuffer the VAO
		bufferVAO();
	}

}