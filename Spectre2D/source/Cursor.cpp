#include "..\include\Spectre2D\Cursor.h"
#include "..\include\Spectre2D\Image.h"

#include "..\extlibs\glfw\include\GLFW\glfw3.h"

namespace sp
{
	Cursor::Cursor(int32_t shape)
	{
		cursor = 0;

		create(shape);
	}

	Cursor::Cursor(const std::filesystem::path& path, const glm::ivec2& hotspot)
	{
		cursor = 0;

		create(path, hotspot);
	}

	Cursor::Cursor(const ImageData& data, const glm::ivec2& hotspot)
	{
		cursor = 0;

		create(data, hotspot);
	}

	Cursor::~Cursor()
	{
		glfwDestroyCursor(cursor);
		cursor = 0;
	}

	void Cursor::create(const std::filesystem::path& path, const glm::ivec2& hotspot)
	{
		create(GenericImageLoader::load(path), hotspot);
	}

	void Cursor::create(const ImageData& data, const glm::ivec2& hotspot)
	{
		// Destroy previous cursor if necessary
		if (cursor)
			glfwDestroyCursor(cursor);

		// Convert the imagedata to GLFWimage
		GLFWimage image{};

		image.width = data.getWidth();
		image.height = data.getHeight();

		const auto& rawData = data.getRaw();


		// +1 so that the compiler does not output a warning. Just in case. There should be no problem with this snippet, but you can't be too careful.
		unsigned char* pdata = new unsigned char[rawData.size() * 4 + 4];

		for (size_t i = 0; i < rawData.size(); i++)
		{
			pdata[i * 4 + 0] = (unsigned char)(rawData[i].r * 255);
			pdata[i * 4 + 1] = (unsigned char)(rawData[i].g * 255);
			pdata[i * 4 + 2] = (unsigned char)(rawData[i].b * 255);
			pdata[i * 4 + 3] = (unsigned char)(rawData[i].a * 255);
		}

		image.pixels = pdata;

		// Create GLFWcursor from GLFWimage
		cursor = glfwCreateCursor(&image, hotspot.x, hotspot.y);

		// Delete pixel buffer
		delete[] image.pixels;
	}

	void Cursor::create(int32_t shape)
	{
		// Destroy previous cursor if necessary
		if (cursor)
			glfwDestroyCursor(cursor);

		// Crate a new cursor
		cursor = glfwCreateStandardCursor(shape);
	}

}