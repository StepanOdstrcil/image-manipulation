#include "Image.hpp"

#include <algorithm>
#include <vector>

#include <iostream>


Image::Image() : width(0), height(0), pixels(nullptr), pixelCount(0)
{
}

Image::Image(const uint32_t& w, const uint32_t& h, const std::string header) : width(w), height(h), pixelCount(w * h), originalHeader(header)
{
	uint32_t pixelCount = w * h;
	pixels = new RGB[pixelCount];
	for (RGB* p = pixels; p < pixels + pixelCount; p++)
		*p = Image::RGB(0);
}

Image::~Image()
{
	if (pixels != nullptr)
		delete[] pixels;
}

const Image::RGB& Image::operator[](const unsigned int& i) const
{
	return pixels[i];
}

Image::RGB& Image::operator[](const unsigned int& i)
{
	return pixels[i];
}

void Image::SaveAsBlackWhite(const char* fileName) const
{
	RGB* newPixels = nullptr;

	std::ofstream ofs;
	ofs.open(fileName, std::ios::binary | std::ios::out);

	try
	{
		if (pixels == nullptr)
			throw("No image loaded");

		if (ofs.fail())
			throw("Cannot open file");

		std::vector<uint8_t> lig = std::vector<uint8_t>(pixelCount);
		for (int i = 0; i < pixelCount; i++)
		{
			lig[i] = pixels[i].lightness;
		}

		std::sort(lig.begin(), lig.end());

		std::vector<uint8_t>::iterator ligtnessPixelsMultisetIterator = lig.begin();
		std::advance(ligtnessPixelsMultisetIterator, (uint32_t)(pixelCount * 0.70f));

		uint8_t treshHold = *ligtnessPixelsMultisetIterator;

		newPixels = new RGB[pixelCount];
		for (int i = 0; i < pixelCount; i++)
		{
			if (pixels[i].lightness > treshHold)
			{
				newPixels[i] = RGB(255);
			}
			else
			{
				newPixels[i] = RGB(0);
			}
		}

		//for (int i = 0; i < pixelCount; i++)
		//{
		//	if (pixels[i].lightness > 200)
		//	{
		//		newPixels[i] = RGB(255);
		//	}

		//	//// Pastel red
		//	//else if (pixels[i].lightness > 150)
		//	//{
		//	//	newPixels[i] = RGB(255, 41, 38);
		//	//}

		//	//else if (pixels[i].lightness > 160)
		//	//{
		//	//	newPixels[i] = RGB(200);
		//	//}
		//	//else if (pixels[i].lightness > 120)
		//	//{
		//	//	newPixels[i] = RGB(150);
		//	//}
		//	//else if (pixels[i].lightness > 80)
		//	//{
		//	//	newPixels[i] = RGB(100);
		//	//}
		//	//else if (pixels[i].lightness > 40)
		//	//{
		//	//	newPixels[i] = RGB(50);
		//	//}
		//	else
		//	{
		//		newPixels[i] = RGB(0);
		//	}
		//}

		// Write to file
		ofs << originalHeader;

		// Ignore bytes to round up to 4 bytes per row
		int ignore = 4 - (width * 3 % 4);
		ignore = ignore == 4 ? 0 : ignore;

		float onePixelPercentageIncrement = 101.f / pixelCount;

		float progress = 0.f;
		uint32_t progressPercentage = 0;

		WritePixels(ofs, newPixels, ignore);

		ofs.close();
	}
	catch (const std::exception& ex)
	{
		fprintf(stderr, "%s\n", ex.what());
		ofs.close();
	}

	if (newPixels != nullptr)
		delete[] newPixels;
}

void Image::SaveAsQuantized(const char* fileName) const
{
	std::ofstream ofs;
	ofs.open(fileName, std::ios::binary | std::ios::out);

	RGB* newPixels = nullptr;
	try
	{
		if (pixels == nullptr)
			throw("No image loaded");

		if (ofs.fail())
			throw("Cannot open file");

		// USE:
		// https://stackoverflow.com/questions/29244307/effective-gif-image-color-quantization
		RGB* pix = nullptr;
		newPixels = new RGB[pixelCount];
		for (int i = 0; i < pixelCount; i++)
		{
			pix = &(pixels[i]);
			newPixels[i] = RGB(pix->r - pix->r % 32, pix->g - pix->g % 32, pix->b - pix->b % 32);
		}

		// Write to file
		ofs << originalHeader;

		// Ignore bytes to round up to 4 bytes per row
		int ignore = 4 - (width * 3 % 4);
		ignore = ignore == 4 ? 0 : ignore;

		WritePixels(ofs, newPixels, ignore);

		ofs.close();
	}
	catch (const std::exception& ex)
	{
		fprintf(stderr, "%s\n", ex.what());
		ofs.close();
	}

	if (newPixels != nullptr)
	{
		delete[] newPixels;
	}
}

void Image::WritePixels(std::ofstream& ofs, RGB* const writePixels, const int ignore) const
{
	uint32_t bufferIndex = 0;
	std::string buffer = std::string((size_t)3 * (size_t)pixelCount + (size_t)height * (size_t)ignore, '0');

	RGB* pix;
	for (uint32_t h = 0; h < height; h++)
	{
		for (uint32_t w = 0; w < width; w++)
		{
			pix = writePixels + h * width + w;
			buffer[bufferIndex++] = pix->b;
			buffer[bufferIndex++] = pix->g;
			buffer[bufferIndex++] = pix->r;
		}

		bufferIndex += ignore;
	}

	ofs << buffer;
}

Image* Image::FromBMP(const char* fileName)
{
	std::ifstream ifs;
	ifs.open(fileName, std::ios::binary | std::ios::in);

	Image* src = nullptr;
	try
	{
		if (ifs.fail())
			throw("Can't open input file");

		// Read file into string
		std::string fileType(2, ' ');
		ifs.read(&fileType[0], 2);

		// Starts with
		if (fileType.rfind("BM", 0) == std::string::npos)
			throw("Can't read input file");

		uint32_t fileSize, pixelArrayOffset, width, height;
		uint16_t b;

		// File size
		fileSize = ReadBigEndianUIntFromFile(ifs);

		// Application specific
		ifs.ignore(4);

		// Pixel array offset
		pixelArrayOffset = ReadBigEndianUIntFromFile(ifs);

		// Number of bytes in the DIB header (from this point)
		ifs.ignore(4);

		// Width/Height
		width = ReadBigEndianUIntFromFile(ifs);
		height = ReadBigEndianUIntFromFile(ifs);
		ifs.ignore(2); // Number of color planes being used
		b = ReadBigEndianUShortFromFile(ifs);

		// Load header into file
		ifs.seekg(0);
		std::string header(pixelArrayOffset, ' ');
		ifs.read(&header[0], pixelArrayOffset);

		// Create new image instance
		src = new Image(width, height, header); // this throw an exception if bad_alloc

		ifs.seekg(pixelArrayOffset);

		// Ignore bytes to round up to 4 bytes per row
		int ignore = 4 - (width * 3 % 4);
		ignore = ignore == 4 ? 0 : ignore;

		int i = 0;
		uint8_t pix[3]; // Read each pixel one by one and convert bytes to floats 
		for (int h = 0; h < height; h++)
		{
			for (int w = 0; w < width; w++)
			{
				i = h * width + w;
				ifs.read(reinterpret_cast<char*>(pix), 3);
				src->pixels[i].r = pix[2];
				src->pixels[i].g = pix[1];
				src->pixels[i].b = pix[0];
				src->pixels[i].lightness = Image::RGB::GetLightness(pix[2], pix[1], pix[0]);
			}

			ifs.ignore(ignore);
		}

		ifs.close();
	}
	catch (const char* err)
	{
		src = nullptr;
		fprintf(stderr, "%s\n", err);
		ifs.close();
	}

	return src;
}

uint32_t Image::ReadBigEndianUIntFromFile(std::ifstream& ifs)
{
	char intBuffer[4];
	ifs.read(intBuffer, 4);

	return (uint8_t)intBuffer[0] | (uint8_t)intBuffer[1] << 8 | (uint8_t)intBuffer[2] << 16 | (uint8_t)intBuffer[3] << 24;
}

uint16_t Image::ReadBigEndianUShortFromFile(std::ifstream& ifs)
{
	char intBuffer[2];
	ifs.read(intBuffer, 2);

	return (uint8_t)intBuffer[0] | (uint8_t)intBuffer[1] << 8;
}
