#include "..\include\Spectre2D\Shape.h"
#include "..\include\Spectre2D\RenderTarget.h"

#include <GL/glew.h>

namespace sp
{
	Shape::Shape(VertexData* vdata, Mesh* _mesh, bool lock)
		: Drawable(), Transformable(lock)
	{
		vertexData = vdata;
		mesh = _mesh;
		maintainsMesh = maintainsVData = false;
		shiftOnReisze = true;
	}

	Shape::~Shape()
	{
		if (maintainsMesh && mesh)
			delete mesh;

		if (maintainsVData && vertexData)
			delete vertexData;
	}

	void Shape::create(VertexData* vdata, Mesh* _mesh)
	{
		vertexData = vdata;
		mesh = _mesh;
		maintainsMesh = maintainsVData = false;
	}

	int32_t Shape::createRect(ShapeType type, const glm::vec2& size)
	{
		checkObjects();

		// Lock vertex data and mesh
		mesh->setLocked(true);
		vertexData->setLocked(true);

		// In case type is Line, we can simply use only 4 indices with LineLoop primitive.
		// In case type is Fill, we can use TriangleStrip primitive, again with just 4 indices.
		int32_t diff = adjust(4, 4);
		size_t offset = mesh->getOffset();

		glm::vec2 positions[4] = {
			{ 0.0f, 0.0f },
			{ size.x, 0.0f },
			size,
			{ 0.0f, size.y }
		};

		for (size_t i = 0; i < 4; i++)
		{
			vertexData->setVertexPosition(offset + i, positions[i]);
			vertexData->setVertexColor(offset + i, glm::vec4(1.0f));
		}

		if (type == ShapeType::Fill)
		{
			mesh->setIndices({ 0, 1, 3, 2 });
			mesh->setPrimitive(Primitive::TriangleStrip);
		}
		else
		{
			mesh->setIndices({ 0, 1, 2, 3 });
			mesh->setPrimitive(Primitive::LineLoop);
		}
		
		mesh->setLocked(false);
		vertexData->setLocked(false);

		return diff;
	}

	int32_t Shape::createCircleSegments(ShapeType type, float_t radius, size_t segments, float_t* sideLength)
	{
		return createRegularPolygonRadius(type, radius, segments, 0.0f, sideLength);
	}

	int32_t Shape::createCircle(ShapeType type, float_t radius, float_t maxSideLength, float_t* sideLength, size_t* segments)
	{
		size_t seg = (size_t)(glm::two_pi<float_t>() / (2 * asin(maxSideLength / (2 * radius))) + 0.5f);

		if (segments)
			*segments = seg;

		return createCircleSegments(type, radius, seg, sideLength);
	}

	int32_t Shape::createRegularPolygonRadius(ShapeType type, float_t radius, size_t segments, float_t angleOffset, float_t* sideLength)
	{
		checkObjects();

		// Lock
		mesh->setLocked(true);
		vertexData->setLocked(true);

		int32_t diff = adjust(segments, segments);
		size_t vOffset = mesh->getOffset();
		std::vector<glm::vec2> points;
		glm::vec2 pt(0.0f, radius);
		glm::vec2 topleft(0.0f);
		glm::vec2 bottomright(0.0f);

		float_t angle = glm::two_pi<float_t>() / segments;

		if (sideLength)
			*sideLength = 2 * radius * sin(angle / 2);

		// Create rotation matrices
		glm::mat2 offsetMatrix(glm::vec2(cos(angleOffset), sin(angleOffset)), glm::vec2(-sin(angleOffset), cos(angleOffset)));
		glm::mat2 angleMatrix(glm::vec2(cos(angle), sin(angle)), glm::vec2(-sin(angle), cos(angle)));

		// Offset the starting point.
		pt = offsetMatrix * pt;

		if (type == ShapeType::Fill)
		{
			mesh->setPrimitive(Primitive::TriangleFan);

			// Set indices
			for (size_t i = 0; i < mesh->getVertexAmount(); i++)
			{
				mesh->setVertexIndex(i, i);
			}
		}
		else
		{
			mesh->setPrimitive(Primitive::LineLoop);
		}

		for (size_t i = 0; i < segments; i++)
		{
			// Push pt and rotate it.
			points.push_back(pt);

			if (pt.x < topleft.x)
				topleft.x = pt.x;

			if (pt.x > bottomright.x)
				bottomright.x = pt.x;

			if (pt.y < topleft.y)
				topleft.y = pt.y;

			if (pt.y > bottomright.y)
				bottomright.y = pt.y;

			pt = angleMatrix * pt;
		}

		// Calculate origin offset.
		glm::vec2 originOffset = (bottomright - topleft) / 2.0f;

		for (size_t i = 0; i < points.size(); i++)
		{
			vertexData->setVertexColor(vOffset + i, glm::vec4(1.0f));
			vertexData->setVertexPosition(vOffset + i, points[i] + originOffset);
		}

		// Release
		mesh->setLocked(false);
		vertexData->setLocked(false);

		return diff;
	}

	int32_t Shape::createRegularPolygonSide(ShapeType type, float_t sideLength, size_t segments, float_t angleOffset, float_t* radius)
	{
		float_t alpha = glm::two_pi<float_t>() / segments;
		float_t rad = (sideLength / 2) / sin(alpha);

		if (radius)
			*radius = rad;

		return createRegularPolygonRadius(type, rad, segments, angleOffset);
	}

	int32_t Shape::createTriangle(ShapeType type, const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2)
	{
		checkObjects();

		// Lock
		mesh->setLocked(true);
		vertexData->setLocked(true);

		int32_t diff = adjust(3, 3);
		size_t vOffset = mesh->getOffset();

		mesh->setIndices({ 0, 1, 2 });

		vertexData->setVertexPosition(0, p0);
		vertexData->setVertexPosition(1, p1);
		vertexData->setVertexPosition(2, p2);

		for (size_t i = 0; i < 3; i++)
		{
			vertexData->setVertexColor(0, glm::vec4(1.0f));
			vertexData->setVertexUV(0, glm::vec2(0.0f));
		}

		if (type == ShapeType::Fill)
			mesh->setPrimitive(Primitive::Triangles);
		else
			mesh->setPrimitive(Primitive::LineLoop);

		// Release
		mesh->setLocked(false);
		vertexData->setLocked(false);

		return diff;
	}

	int32_t Shape::createPoints(const std::vector<glm::vec2>& points)
	{
		checkObjects();

		// Lock
		mesh->setLocked(true);
		vertexData->setLocked(true);

		size_t ptc = points.size();

		int32_t diff = adjust(points.size(), points.size());
		size_t vOffset = mesh->getOffset();

		mesh->setPrimitive(Primitive::Points);

		for (size_t i = 0; i < ptc; i++)
		{
			mesh->setVertexIndex(i, i);
			vertexData->setVertexPosition(i, points[i]);
			vertexData->setVertexColor(vOffset + i, glm::vec4(1.0f));
			vertexData->setVertexUV(vOffset + i, glm::vec2(1.0f));
		}

		// Release
		mesh->setLocked(false);
		vertexData->setLocked(false);

		return diff;
	}

	int32_t Shape::createLine(const std::vector<glm::vec2>& points)
	{
		checkObjects();

		// Lock
		mesh->setLocked(true);
		vertexData->setLocked(true);

		size_t ptc = points.size();

		int32_t diff = adjust(points.size(), points.size());
		size_t vOffset = mesh->getOffset();

		mesh->setPrimitive(Primitive::LineStrip);

		for (size_t i = 0; i < ptc; i++)
		{
			mesh->setVertexIndex(i, i);
			vertexData->setVertexPosition(i, points[i]);
			vertexData->setVertexColor(vOffset + i, glm::vec4(1.0f));
			vertexData->setVertexUV(vOffset + i, glm::vec2(1.0f));
		}

		// Release
		mesh->setLocked(false);
		vertexData->setLocked(false);

		return diff;
	}

	int32_t Shape::createPolygonConvex(ShapeType type, const std::vector<glm::vec2>& points)
	{
		checkObjects();

		// Lock
		mesh->setLocked(true);
		vertexData->setLocked(true);

		size_t ptc = points.size();

		int32_t diff = adjust(points.size(), points.size());
		size_t vOffset = mesh->getOffset();

		if (type == ShapeType::Fill)
			mesh->setPrimitive(Primitive::TriangleFan);
		else
			mesh->setPrimitive(Primitive::LineLoop);

		for (size_t i = 0; i < ptc; i++)
		{
			mesh->setVertexIndex(i, i);
			vertexData->setVertexPosition(i, points[i]);
			vertexData->setVertexColor(vOffset + i, glm::vec4(1.0f));
			vertexData->setVertexUV(vOffset + i, glm::vec2(1.0f));
		}

		// Release
		mesh->setLocked(false);
		vertexData->setLocked(false);

		return diff;
	}

	void Shape::createMesh(Primitive primitive, DataUsage usage, size_t vertexAmount, size_t offset, const std::vector<size_t>& _indices, bool lock)
	{
		mesh = new Mesh(primitive, usage, vertexAmount, offset, _indices, lock);
		maintainsMesh = true;
	}

	void Shape::createVertexData(DataUsage usage, size_t vertexAmount, bool lock)
	{
		vertexData = new VertexData(usage, vertexAmount, lock);
		maintainsVData = true;
	}

	void Shape::setMesh(Mesh* _mesh)
	{
		if (maintainsMesh)
			delete mesh;

		mesh = _mesh;
		maintainsMesh = false;
	}

	void Shape::setVertexData(VertexData* vdata)
	{
		if (maintainsVData)
			delete vertexData;

		vertexData = vdata;
		maintainsVData = false;
	}

	Mesh* Shape::getMesh()
	{
		return mesh;
	}

	const Mesh* Shape::getMesh() const
	{
		return mesh;
	}

	VertexData* Shape::getVertexData()
	{
		return vertexData;
	}

	const VertexData* Shape::getVertexData() const
	{
		return vertexData;
	}

	void Shape::setDataUsage(DataUsage usage)
	{
		mesh->setDataUsage(usage);
		vertexData->setDataUsage(usage);
	}

	bool Shape::getMaintainsMesh() const
	{
		return maintainsMesh;
	}

	void Shape::setMaintainsMesh(bool value)
	{
		maintainsMesh = value;
	}

	bool Shape::getMaintainsVertexData() const
	{
		return maintainsVData;
	}

	void Shape::setMaintainsVertexData(bool value)
	{
		maintainsVData = value;
	}

	void Shape::draw(RenderTarget* target)
	{
		mesh->bind();
		vertexData->bind();

		target->setModel(evaluateTransform());
		target->setTint(getTint());

		glDrawElements(mesh->getOpenGLPrimitive(mesh->getPrimitive()), mesh->getVertexAmount(), GL_UNSIGNED_INT, nullptr);
	}

	int32_t Shape::adjust(size_t vertices, size_t indices)
	{
		// For now: If we don't have enough space, we just resize.

		checkObjects();

		size_t oldVertices = vertexData->getVertexAmount();

		// Resize
		vertexData->setVertexAmount(vertices, false);

		if (shiftOnReisze)
		{
			size_t oldIndex = mesh->getOffset() + oldVertices;
			size_t newIndex = mesh->getOffset() + vertices;

			vertexData->shift(oldIndex, oldVertices - oldIndex, newIndex);
		}

		return mesh->setVertexAmount(indices);
	}

	void Shape::setShiftOnResize(bool value)
	{
		shiftOnReisze = value;
	}

	bool Shape::getShiftOnResize() const
	{
		return shiftOnReisze;
	}

	void Shape::checkObjects()
	{
		if (!mesh)
			throw Error(SPECTRE_MISSING_OBJECT, "Missing mesh when adjusting object sizes.");

		if (!vertexData)
			throw Error(SPECTRE_MISSING_OBJECT, "Missing vertex data when adjusting object sizes.");
	}

}