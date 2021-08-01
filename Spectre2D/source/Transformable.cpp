#include "..\include\Spectre2D\Transformable.h"

namespace sp
{
	TransformSeries::TransformSeries(size_t _size, bool lock)
		: LockableObject(lock)
	{
		matrices.resize(_size, glm::mat4(1.0f));
		size = _size;
		updateNeeded = false;
		transformed = inverse = glm::mat4(1.0f);
	}

	size_t TransformSeries::getSize() const
	{
		return size;
	}

	void TransformSeries::setSize(size_t _size)
	{
		size = _size;
		matrices.resize(_size, glm::mat4(1.0f));
	}

	void TransformSeries::setMatrix(size_t index, const glm::mat4& matrix)
	{
		matrices[index] = matrix;

		updateNeeded = true;
		if (!getLocked())
			evaluateTransform();
	}

	void TransformSeries::setMatrix(size_t index, const glm::mat3& matrix)
	{
		glm::mat4 full(1.0f);

		for (size_t x = 0; x < 2; x++)
			for (size_t y = 0; y < 2; y++)
				full[2 * x][2 * y] = matrix[x][y];

		full[3] = glm::vec4(matrix[2].x, matrix[2].y, 0.0f, matrix[2].z);

		matrices[index] = full;

		updateNeeded = true;
		if (!getLocked())
			evaluateTransform();
	}

	const glm::mat4& TransformSeries::getMatrix(size_t index) const
	{
		return matrices[index];
	}

	void TransformSeries::setTranslation(size_t index, const glm::vec2& vec)
	{
		glm::mat4 trans(1.0f);

		trans[3] = glm::vec4(vec, 0.0f, 1.0f);

		setMatrix(index, trans);
	}

	void TransformSeries::setScale(size_t index, const glm::vec2& vec)
	{
		glm::mat4 sca(1.0f);

		sca[0][0] = vec.x;
		sca[1][1] = vec.y;

		setMatrix(index, sca);
	}

	void TransformSeries::setRotation(size_t index, float_t angle)
	{
		glm::mat4 rot(1.0f);

		float_t cang = cos(angle);
		float_t sang = sin(angle);

		rot[0][0] = rot[1][1] = cang;
		rot[0][1] = sang;
		rot[1][0] = -sang;

		setMatrix(index, rot);
	}

	void TransformSeries::setShearX(size_t index, float_t factor)
	{
		glm::mat4 sh(1.0f);

		sh[1][0] = factor;

		setMatrix(index, sh);
	}

	void TransformSeries::setShearY(size_t index, float_t factor)
	{
		glm::mat4 sh(1.0f);

		sh[0][1] = factor;

		setMatrix(index, sh);
	}

	const glm::mat4& TransformSeries::evaluateTransform()
	{
		if (updateNeeded)
		{
			updateNeeded = false;

			transformed = glm::mat4(1.0f);

			for (const auto& matrix : matrices)
			{
				transformed = matrix * transformed;
			}
		}

		inverse = glm::inverse(transformed);

		return transformed;
	}

	const glm::mat4& TransformSeries::getInverse()
	{
		evaluateTransform();
		return inverse;
	}

	const std::vector<glm::mat4>& TransformSeries::getMatrices() const
	{
		return matrices;
	}

	void TransformSeries::setLocked(bool value)
	{
		LockableObject::setLocked(value);

		if (!value)
			evaluateTransform();
	}

	Transformable::Transformable(bool lock)
		: TransformSeries(6, lock)
	{
		position = glm::vec2(0.0f);
		origin = glm::vec2(0.0f);
		rotation = 0.0f;
		scale = glm::vec2(0.0f);
		shear = glm::vec2(0.0f);
	}

	void Transformable::setPosition(const glm::vec2& vec)
	{
		position = vec;
		setTranslation(5, vec);
	}

	const glm::vec2& Transformable::getPosition() const
	{
		return position;
	}

	void Transformable::setOrigin(const glm::vec2& vec)
	{
		origin = vec;
		setTranslation(0, vec);
	}

	const glm::vec2& Transformable::getOrigin() const
	{
		return origin;
	}

	void Transformable::setScale(const glm::vec2& vec)
	{
		scale = vec;
		TransformSeries::setScale(3, vec);
	}

	const glm::vec2& Transformable::getScale() const
	{
		return scale;
	}

	void Transformable::setRotation(float_t angle)
	{
		rotation = angle;
		TransformSeries::setRotation(4, angle);
	}

	float_t Transformable::getRotation() const
	{
		return rotation;
	}

	void Transformable::setShear(const glm::vec2& vec)
	{
		shear = vec;

		setLocked(true);
		setShearX(1, vec.x);
		setShearY(2, vec.y);
		setLocked(false);
	}

	const glm::vec2& Transformable::getShear() const
	{
		return shear;
	}

}