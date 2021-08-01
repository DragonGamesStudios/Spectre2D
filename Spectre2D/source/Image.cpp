#include "..\include\Spectre2D\Image.h"
#include "..\include\Spectre2D\core.h"

#include <fstream>
#include <iostream>

namespace sp
{
	// Reverse the bytes to change endianness
	void reverseBytes(char* c, size_t size)
	{
		char temp = 0;
		for (size_t i = 0; i < (size / 2); i++)
		{
			temp = c[i];
			c[i] = c[size - i - 1];
			c[size - i - 1] = temp;
		}
	}

	ImageData::ImageData()
	{
		width = height = 0;
	}

	ImageData::ImageData(int32_t w, int32_t h, const std::vector<glm::vec4>& _data)
	{
		width = w;
		height = h;

		data = _data;
		data.resize(w * h);
	}

	ImageData::ImageData(const ImageData& other)
	{
		width = other.getWidth();
		height = other.getHeight();

		data = other.getRaw();
	}

	ImageData::ImageData(ImageData&& other) noexcept
	{
		width = other.getWidth();
		height = other.getHeight();

		data = other.getRaw();
	}

	ImageData::~ImageData()
	{
		data.clear();

		width = height = 0;
	}

	ImageData& ImageData::operator=(const ImageData& other)
	{
		width = other.getWidth();
		height = other.getHeight();

		data = other.getRaw();

		return *this;
	}

	int32_t ImageData::getWidth() const
	{
		return width;
	}

	int32_t ImageData::getHeight() const
	{
		return height;
	}

	const std::vector<glm::vec4>& ImageData::getRaw() const
	{
		return data;
	}

	size_t ImageData::getSize() const
	{
		return data.size();
	}

	const glm::vec4& ImageData::getPixel(int32_t x, int32_t y) const
	{
		// Y rows + X
		return data[y * width + x];
	}

	const glm::vec4& ImageData::getPixel(const glm::ivec2& pos) const
	{
		return getPixel(pos.x, pos.y);
	}

	void ImageData::setPixel(int32_t x, int32_t y, const glm::vec4& color)
	{
		// Y rows + X
		data[y * width + x] = color;
	}

	void ImageData::setPixel(const glm::ivec2& pos, const glm::vec4& color)
	{
		setPixel(pos.x, pos.y, color);
	}

	ImageData GenericImageLoader::load(const std::filesystem::path& path, const std::string& format, bool flip_vertically, bool flip_horizontally)
	{
		std::ifstream file(path, std::ios::binary);

		if (!file.is_open())
			throw Error(SPECTRE_COULD_NOT_OPEN_FILE, "Could not open file " + path.string() + ".");

		return GenericImageLoader::load(file, format, flip_vertically, flip_horizontally);
	}

	ImageData GenericImageLoader::load(std::istream& file, const std::string& _format, bool flip_vertically, bool flip_horizontally)
	{
		std::string format;
		std::transform(_format.begin(), _format.end(), format.begin(), [](unsigned char c) { return std::tolower(c); });

		if (format.empty())
		{
			// If no format is given, check if the file is a valid file of any supported format.
			if (BMPLoader::isBMP(file))
				return BMPLoader::load(file, flip_vertically, flip_horizontally);
			else
				throw Error(SPECTRE_UNSUPPORTED_FORMAT, "Image format was not recognized.");
		}
		else
		{
			if (format == "bmp")
				return BMPLoader::load(file, flip_vertically, flip_horizontally);
			else
				throw Error(SPECTRE_UNSUPPORTED_FORMAT, "Image format was not recognized.");
		}
	}

	ImageData BMPLoader::load(const std::filesystem::path& path, bool flip_vertically, bool flip_horizontally, bool loadColorTable, BMPFile* fileStruct)
	{
		std::ifstream file(path, std::ios::binary);

		if (!file.is_open())
			throw Error(SPECTRE_COULD_NOT_OPEN_FILE, "Could not open file " + path.string() + ".");

		return load(file, flip_vertically, flip_horizontally, loadColorTable, fileStruct);
	}

	ImageData BMPLoader::load(std::istream& file, bool flipVertically, bool flipHorizontally, bool loadColorTable, BMPFile* fileStruct)
	{
		if (file.good())
		{
			auto pos = file.tellg();
			BMPFile bmp;

			// TODO: The library shouldn't test for Big/little endian. It should test for native/non-native endian.
			if (BIG_ENDIAN)
			{

			}
			else
			{
				// Read the info header
				file.read(reinterpret_cast<char*>(&bmp.fileHeader), 14);

				file.read(reinterpret_cast<char*>(&bmp.infoHeader), 40);
				file.read(reinterpret_cast<char*>(&bmp.colorHeader), static_cast<std::streamsize>(bmp.infoHeader.infoHeaderSize) - 40);

				if (bmp.infoHeader.colorsUsed && (bmp.infoHeader.bitCount < 16 || loadColorTable))
				{
					bmp.colorTable = new BMPColor[bmp.infoHeader.colorsUsed];
					file.read(reinterpret_cast<char*>(bmp.colorTable), static_cast<std::streamsize>(bmp.infoHeader.colorsUsed) * 4);
				}
			}

			// Two first header bytes, BM, when interpreted as little-endian uint16_t, will be equivalent to 0x4d42 (reversed)
			if (bmp.fileHeader.headerSignature != 0x4d42)
			{
				file.seekg(pos);
				throw Error(SPECTRE_INVALID_FILE, "Attempting to load an invalid BMP file.");
				return ImageData();
			}

			// Negative-height images cannot be compressed
			if (bmp.infoHeader.height < 0 && bmp.infoHeader.compression != 0 && bmp.infoHeader.compression != 3)
			{
				throw Error(SPECTRE_INVALID_FILE, "Attempting to load an invalid BMP file.");
			}

			uint32_t redMask = 0, greenMask = 0, blueMask = 0, alphaMask = 0;

			switch (bmp.infoHeader.bitCount)
			{
			case 0:
				// bitCount being 0 means JPEG/PNG is used
				if (bmp.infoHeader.compression != 5 && bmp.infoHeader.compression != 4)
				{
					file.seekg(pos);
					throw Error(SPECTRE_INVALID_FILE, "Attempting to load an invalid BMP file.");
					return ImageData();
				}
				else
				{
					// TODO: Implement the possibility of loading JPEG or PNG compressed files.
					file.seekg(pos);
					throw Error(SPECTRE_INVALID_FILE, "Attempting to load an invalid BMP file.");
					return ImageData();
				}

				break;

			case 1:
			case 4:
			case 8:
				break;

			case 16:
				// Read masks if present, read color table if present and wanted
				if (!loadColorTable || !bmp.infoHeader.colorsUsed)
				{
					bmp.colorTable = nullptr;
				}

				if (bmp.infoHeader.compression == 0)
				{
					// BGR
					redMask = 0x0000f800;
					greenMask = 0x000007c0;
					blueMask = 0x0000003e;
				}
				else if (bmp.infoHeader.compression == 3)
				{
					redMask = bmp.colorHeader.redMask;
					greenMask = bmp.colorHeader.greenMask;
					blueMask = bmp.colorHeader.blueMask;
					alphaMask = bmp.colorHeader.alphaMask;
				}

				break;

			case 24:
				// Read color table if present and wanted
				if (!loadColorTable || !bmp.infoHeader.colorsUsed)
				{
					bmp.colorTable = nullptr;
				}

				// BGR
				redMask = 0x00ff0000;
				greenMask = 0x0000ff00;
				blueMask = 0x000000ff;

				break;

			case 32:
				// Read color table if present and wanted, read color masks if present
				if (!loadColorTable || !bmp.infoHeader.colorsUsed)
				{
					bmp.colorTable = nullptr;
				}

				if (bmp.infoHeader.compression == 0)
				{
					// BGR
					redMask = 0x00ff0000;
					greenMask = 0x0000ff00;
					blueMask = 0x000000ff;
				}
				else
				{
					// BGRA
					redMask = bmp.colorHeader.redMask;
					greenMask = bmp.colorHeader.greenMask;
					blueMask = bmp.colorHeader.blueMask;
					alphaMask = bmp.colorHeader.alphaMask;
				}

				break;
			}

			// Load pixel data
			auto datapos = pos;
			datapos += bmp.fileHeader.pixelDataOffset;

			file.seekg(datapos);
			std::vector<glm::vec4> data;

			switch (bmp.infoHeader.bitCount)
			{
			case 32:
			case 24:
			case 16:
				switch (bmp.infoHeader.compression)
				{
				case 0:
				case 3:
					// 32, 24 and 16bit are uncompressed
					loadUncompressed(file, data, bmp.infoHeader.bitCount / 8, bmp.infoHeader.width, bmp.infoHeader.height, redMask, greenMask, blueMask, alphaMask, flipVertically, flipHorizontally);
					break;

				default:
					file.seekg(pos);
					throw Error(SPECTRE_INVALID_FILE, "Attempting to load an invalid BMP file.");
					return ImageData();
				}

				break;

			case 8:
				// 8bit are rle8 compressed or uncompressed
				switch (bmp.infoHeader.compression)
				{
				case 0:
					loadUncompressed(
						file, data, bmp.colorTable, bmp.infoHeader.colorsUsed, static_cast<uint8_t>(bmp.infoHeader.bitCount),
						bmp.infoHeader.width, bmp.infoHeader.height, flipVertically, flipHorizontally
					);
					break;

				case 1:
					loadRLE8Compressed(file, data, bmp.colorTable, bmp.infoHeader.colorsUsed, bmp.infoHeader.width, bmp.infoHeader.height, flipVertically, flipHorizontally);
					break;

				default:
					file.seekg(pos);
					throw Error(SPECTRE_INVALID_FILE, "Attempting to load an invalid BMP file.");
					return ImageData();
				}

				break;

			case 4:
				// 4bit are rle4 compressed or uncompressed
				switch (bmp.infoHeader.compression)
				{
				case 0:
					loadUncompressed(
						file, data, bmp.colorTable, bmp.infoHeader.colorsUsed, static_cast<uint8_t>(bmp.infoHeader.bitCount),
						bmp.infoHeader.width, bmp.infoHeader.height, flipVertically, flipHorizontally
					);
					break;

				case 2:
					loadRLE4Compressed(file, data, bmp.colorTable, bmp.infoHeader.colorsUsed, bmp.infoHeader.width, bmp.infoHeader.height, flipVertically, flipHorizontally);
					break;

				default:
					file.seekg(pos);
					throw Error(SPECTRE_INVALID_FILE, "Attempting to load an invalid BMP file.");
					return ImageData();
				}

				break;

			case 1:
				// 1bit are uncompressed
				switch (bmp.infoHeader.compression)
				{
				case 0:
					load1Bit(file, data, bmp.colorTable, bmp.infoHeader.colorsUsed, bmp.infoHeader.width, bmp.infoHeader.height, flipVertically, flipHorizontally);
					break;

				default:
					file.seekg(pos);
					throw Error(SPECTRE_INVALID_FILE, "Attempting to load an invalid BMP file.");
					return ImageData();
				}

				break;
			}

			bmp.data = ImageData(bmp.infoHeader.width, bmp.infoHeader.height, data);

			if (fileStruct)
			{
				*fileStruct = bmp;
			}

			return bmp.data;
		}

		throw Error(SPECTRE_INVALID_FILE, "Input stream was not good.");
	}

	void BMPLoader::loadUncompressed(
		std::istream& input, std::vector<glm::vec4>& data, uint8_t bytesPerPixel, int32_t width, int32_t height, uint32_t redMask, uint32_t greenMask, uint32_t blueMask, uint32_t alphaMask,
		bool flipVertically, bool flipHorizontally
	)
	{
		// Resize data, save stream position and calculate stride, starting pixel index and pixel advancement
		data.resize(width * height);
		auto curPos = input.tellg();
		int32_t stride = ((((width * bytesPerPixel * 8) + 31) & ~31) >> 3);

		size_t pixelI = (flipVertically ? 0 : (height - 1) * width) + (flipHorizontally ? (width - 1) : 0);
		int8_t pixelAdvance = flipHorizontally ? -1 : 1;

		for (int32_t y = 0; y < height; y++)
		{
			// For each row
			curPos += stride;

			for (int32_t x = 0; x < width; x++)
			{
				// For each pixel

				// Load BGRA
				uint32_t rgba = 0;
				input.read(reinterpret_cast<char*>(&rgba), bytesPerPixel);

				if (BIG_ENDIAN)
				{
					reverseBytes(reinterpret_cast<char*>(&rgba), 4);
				}

				// Since channel mask is at the same time its biggest possible value, we can use it convert uints to floats in range 0.0 - 1.0
				if (redMask)
					data[pixelI].r = float(rgba & redMask) / redMask;
				else
					data[pixelI].r = 0.0f;

				if (greenMask)
					data[pixelI].g = float(rgba & greenMask) / greenMask;
				else
					data[pixelI].g = 0.0f;

				if (blueMask)
					data[pixelI].b = float(rgba & blueMask) / blueMask;
				else
					data[pixelI].b = 0.0f;

				if (alphaMask)
					data[pixelI].a = float(rgba & alphaMask) / alphaMask;
				else
					data[pixelI].a = 1.0f;

				pixelI += pixelAdvance;
			}

			// Note: vertical flip is performed by default. if `flipVertically` `true`, tha flip is not performed.
			// If vertical flip (flipVertically == false):
			//		If horizontal flip: pixelAdvance is -1, meaning that when row ends, pixelI goes to the end of the previous row. Do nothing
			//		If no horizontal flip: pixelAdvance is 1, meaning that when row ends, pixelI to the of the next row. Go back two rows
			// If no vertical flip (flipVertically == true):
			//		If horizontal flip: pixelAdvance is -1, meaning that when row ends, pixelI goes to the end of the previous row. Go forward two rows
			//		If no horizontal flip: pixelAdvance is 1, meaning that when row ends, pixelI goes to the beginning of the next row. Do nothing
			if (!flipVertically && !flipHorizontally)
			{
				pixelI -= 2 * width;
			}
			else if (flipVertically && flipHorizontally)
			{
				pixelI += 2 * width;
			}

			// Go to the next row of data (with the correct stride)
			input.seekg(curPos);
		}
	}

	void BMPLoader::loadUncompressed(
		std::istream& input, std::vector<glm::vec4>& data, BMPColor* colorTable, size_t colorTableSize, uint8_t bpp, int32_t width, int32_t height, bool flipVertically, bool flipHorizontally
	)
	{
		// TODO: This loading function WILL NOT ALWAYS WORK. REDO and DOCUMENT in Image Manipulation Update
		// Resize data, save stream position and calculate stride, starting pixel index and pixel advancement
		data.resize(width * height);
		auto curPos = input.tellg();
		int32_t stride = ((((width * bpp) + 31) & ~31) >> 3);
		BMPColor color1{0, 0, 0, 0};
		BMPColor color2{0, 0, 0, 0};

		size_t pixelI = (flipVertically ? 0 : (height - 1) * width) + (flipHorizontally ? (width - 1) : 0);
		int8_t pixelAdvance = flipHorizontally ? -1 : 1;
		int32_t xAdvance = 8 / bpp;

		if (bpp == 4)
		{
			pixelAdvance *= 2;
		}

		for (int32_t y = 0; y < height; y++)
		{
			// For each row
			curPos += stride;

			for (int32_t x = 0; x < width; x += xAdvance)
			{
				// For each pixel

				// Load index/indices
				uint8_t index = 0;
				input.read(reinterpret_cast<char*>(&index), 1);

				// If bpp is 8, the byte contains only one index
				if (bpp == 8)
				{
					if (index >= colorTableSize)
					{
						throw Error(SPECTRE_INVALID_FILE, "Attempt to load an invalid BMP file.");
						return;
					}

					color1 = colorTable[index];
				}
				// Otherwise it containes two indices
				else if (bpp == 4)
				{
					uint8_t index1 = index & 0xf0;
					uint8_t index2 = index & 0x0f;

					if (index1 >= colorTableSize || index2 >= colorTableSize)
					{
						throw Error(SPECTRE_INVALID_FILE, "Attempt to load an invalid BMP file.");
						return;
					}

					color1 = colorTable[index1];
					color2 = colorTable[index2];
				}

				// Set the respective pixels
				data[pixelI].r = float(color1.r) / 255;
				data[pixelI].g = float(color1.g) / 255;
				data[pixelI].b = float(color1.b) / 255;
				data[pixelI].a = 1.0f;

				if (bpp == 4 && x != width - 1)
				{
					data[pixelI + 1].r = float(color2.r) / 255;
					data[pixelI + 1].g = float(color2.g) / 255;
					data[pixelI + 1].b = float(color2.b) / 255;
					data[pixelI + 1].a = 1.0f;
				}

				if (x != width - 1 || bpp != 4)
					pixelI += pixelAdvance;
				else
					pixelI += pixelAdvance / 2;
			}

			if (!flipVertically && !flipHorizontally)
			{
				pixelI -= 2 * width;
			}
			else if (flipVertically && flipHorizontally)
			{
				pixelI += 2 * width;
			}

			input.seekg(curPos);
		}
	}

	void BMPLoader::loadRLE8Compressed(
		std::istream& input, std::vector<glm::vec4>& data, BMPColor* colorTable, size_t colorTableSize, int32_t width, int32_t height, bool flipVertically, bool flipHorizontally
	)
	{
	}

	void BMPLoader::loadRLE4Compressed(
		std::istream& input, std::vector<glm::vec4>& data, BMPColor* colorTable, size_t colorTableSize, int32_t width, int32_t height, bool flipVertically, bool flipHorizontally
	)
	{
	}

	void BMPLoader::load1Bit(std::istream& input, std::vector<glm::vec4>& data, BMPColor* colorTable, size_t colorTableSize, int32_t width, int32_t height, bool flipVertically, bool flipHorizontally)
	{
		// TODO: Make sure this function works.

		// Resize data, save stream position and calculate stride, starting pixel index and pixel advancement
		data.resize(width * height);
		auto curPos = input.tellg();
		int32_t stride = ((((width * 8) + 31) & ~31) >> 3);

		size_t pixelI = (flipVertically ? 0 : (height - 1) * width) + (flipHorizontally ? (width - 1) : 0);
		int8_t pixelAdvance = flipHorizontally ? -1 : 1;

		if (colorTableSize < 2)
		{
			throw Error(SPECTRE_INVALID_FILE, "Attempt to load an invalid BMP file.");
			return;
		}
		
		for (int32_t y = 0; y < height; y++)
		{
			// For each row
			curPos += stride;

			for (int32_t x = 0; x < width; x += 8)
			{
				// For each pixel

				// Load the packed indices
				uint8_t rgba = 0;
				input.read(reinterpret_cast<char*>(&rgba), 1);

				for (size_t i = 0; i < 8; i++)
				{
					// Every bit of the loaded byte contains one index. Thus 8 indices per byte.
					data[pixelI + i * pixelAdvance].r = float(colorTable[1 << i].r) / 255;
					data[pixelI + i * pixelAdvance].g = float(colorTable[1 << i].g) / 255;
					data[pixelI + i * pixelAdvance].b = float(colorTable[1 << i].b) / 255;
					data[pixelI + i * pixelAdvance].a = 1.0;

					pixelI += pixelAdvance * 8;
				}
			}

			// Note: vertical flip is performed by default. if `flipVertically` `true`, tha flip is not performed.
			// If vertical flip (flipVertically == false):
			//		If horizontal flip: pixelAdvance is -1, meaning that when row ends, pixelI goes to the end of the previous row. Do nothing
			//		If no horizontal flip: pixelAdvance is 1, meaning that when row ends, pixelI to the of the next row. Go back two rows
			// If no vertical flip (flipVertically == true):
			//		If horizontal flip: pixelAdvance is -1, meaning that when row ends, pixelI goes to the end of the previous row. Go forward two rows
			//		If no horizontal flip: pixelAdvance is 1, meaning that when row ends, pixelI goes to the beginning of the next row. Do nothing
			if (!flipVertically && !flipHorizontally)
			{
				pixelI -= 2 * width;
			}
			else if (flipVertically && flipHorizontally)
			{
				pixelI += 2 * width;
			}

			input.seekg(curPos);
		}
	}

	bool BMPLoader::isBMP(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::binary);
		
		if (file.is_open())
		{
			char h[2];
			file.read(h, 2);

			file.close();

			// Check header
			if (h[0] == 'B' && h[1] == 'M')
				return true;
		}

		std::cerr << "Could not open file " << path << std::endl;
		return false;
	}

	bool BMPLoader::isBMP(std::istream& file)
	{
		if (file.good())
		{
			auto pos = file.tellg();

			char h[2];
			file.read(h, 2);

			file.seekg(pos);

			// Check header
			if (h[0] == 'B' && h[1] == 'M')
				return true;
		}

		return false;
	}

}