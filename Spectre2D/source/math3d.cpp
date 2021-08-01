#include "..\include\Spectre2D\math3d.h"

namespace sp::m3d
{

	vec::vec(size_t size_, value_type x, value_type y, value_type z, value_type w)
	{
		size = size_;
		components = new value_type[size_];

		switch (size)
		{
		case 4:
			components[3] = w;

			[[fallthrough]];

		case 3:
			components[2] = z;

			[[fallthrough]];

		case 2:
			components[1] = y;

			[[fallthrough]]

		case 1:
			components[0] = x;

			break;
		}
	}

	vec::vec(const vec& v)
	{
		size = v.size;
		components = new value_type[size];
		
		for (size_t i = 0; i < size; i++)
		{
			components[i] = v.components[i];
		}
	}

	vec::vec(vec&& v)
	{
		size = v.size;
		components = new value_type[size];

		for (size_t i = 0; i < size; i++)
		{
			components[i] = v.components[i];
		}
	}

	vec::~vec()
	{
		delete[] components;
	}

	void vec::from_f_f(value_type x, value_type y)
	{
		setComponent(0, x);
		setComponent(1, y);
	}

	void vec::from_vec2(const vec& xy)
	{
		setComponent(0, xy.getComponent(0));
		setComponent(1, xy.getComponent(1));
	}

	void vec::from_vec2_f(const vec& xy, value_type z)
	{
		setComponent(0, xy.getComponent(0));
		setComponent(1, xy.getComponent(1));
		setComponent(2, z);
	}

	void vec::from_f_f_f(value_type x, value_type y, value_type z)
	{
		setComponent(0, x);
		setComponent(1, y);
		setComponent(2, z);
	}

	void vec::from_f_vec2(value_type x, const vec& yz)
	{
		setComponent(0, x);
		setComponent(1, yz.getComponent(0));
		setComponent(2, yz.getComponent(1));
	}

	void vec::from_vec3(const vec& xyz)
	{
		setComponent(0, xyz.getComponent(0));
		setComponent(1, xyz.getComponent(1));
		setComponent(2, xyz.getComponent(2));
	}

	void vec::from_vec2_f_f(const vec& xy, value_type z, value_type w)
	{
		setComponent(0, xy.getComponent(0));
		setComponent(1, xy.getComponent(1));
		setComponent(2, z);
		setComponent(3, w);
	}

	void vec::from_f_f_f_f(value_type x, value_type y, value_type z, value_type w)
	{
		setComponent(0, x);
		setComponent(1, y);
		setComponent(2, z);
		setComponent(3, w);
	}

	void vec::from_f_vec2_f(value_type x, const vec& yz, value_type w)
	{
		setComponent(0, x);
		setComponent(1, yz.getComponent(0));
		setComponent(2, yz.getComponent(1));
		setComponent(3, w);
	}

	void vec::from_f_f_vec2(value_type x, value_type y, const vec& zw)
	{
		setComponent(0, x);
		setComponent(1, y);
		setComponent(2, zw.getComponent(0));
		setComponent(3, zw.getComponent(1));
	}

	void vec::from_vec2_vec2(const vec& xy, const vec& zw)
	{
		setComponent(0, xy.getComponent(0));
		setComponent(1, xy.getComponent(1));
		setComponent(2, zw.getComponent(0));
		setComponent(3, zw.getComponent(1));
	}

	void vec::from_vec3_f(const vec& xyz, value_type w)
	{
		setComponent(0, xyz.getComponent(0));
		setComponent(1, xyz.getComponent(1));
		setComponent(2, xyz.getComponent(2));
		setComponent(3, w);
	}

	void vec::from_f_vec3(value_type x, const vec& yzw)
	{
		setComponent(0, x);
		setComponent(1, yzw.getComponent(0));
		setComponent(2, yzw.getComponent(1));
		setComponent(3, yzw.getComponent(2));
	}

	void vec::from_vec4(const vec& xyzw)
	{
		setComponent(0, xyzw.getComponent(0));
		setComponent(1, xyzw.getComponent(1));
		setComponent(2, xyzw.getComponent(2));
		setComponent(3, xyzw.getComponent(3));
	}

	void vec::setComponent(size_t componenti, value_type value)
	{
		components[componenti] = value;
	}

	vec::value_type vec::getComponent(size_t componenti) const
	{
		return components[componenti];
	}

	vec vec::getSwizzled(const char* code, size_t size) const
	{
		vec result(size);

		for (size_t i = 0; i < size; i++)
		{
			switch (code[i])
			{
			case 'x':
			case 'r':
				result.setComponent(i, getComponent(0));

				break;

			case 'y':
			case 'g':
				result.setComponent(i, getComponent(1));

				break;

			case 'z':
			case 'b':
				result.setComponent(i, getComponent(2));

				break;

			case 'w':
			case 'a':
				result.setComponent(i, getComponent(3));

				break;

			default:
				result.setComponent(i, 0.0f);
			}
		}

		return result;
	}

}