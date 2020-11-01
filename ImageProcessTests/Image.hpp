#pragma once

#include <fstream>

class Image
{
public:
	// RGB structure, i.e. a pixel 
	struct RGB
	{
		uint8_t r, g, b;
		uint8_t lightness;

		RGB() : r(0), g(0), b(0), lightness(0) {}
		RGB(int c) : r(c), g(c), b(c), lightness(c * 0.94117f) {}
		RGB(int _r, int _g, int _b) : r(_r), g(_g), b(_b), lightness(GetLightness(_r, _g, _b)) {}

		static uint8_t GetLightness(const uint8_t& _r, const uint8_t& _g, const uint8_t& _b)
		{
			// 0.470585 => 240 / 255 = 0.94117 / 2 (division is used in equation)
			// (max(r, g, b) + min(r, g, b)) / 2 = [lightness] * (240 / 255) [conversion to max lightness 240]
			// eq: (max(r, g, b) + min(r, g, b)) / 2 * 240 / 255
			return (std::max(std::max(_r, _g), _b) + std::min(std::min(_r, _g), _b)) * 0.470585f;
		}

		bool operator == (const RGB& c) const
		{
			return c.r == r && c.g == g && c.b == b;
		}
		bool operator != (const RGB& c) const
		{
			return c.r != r || c.g != g || c.b != b;
		}
		RGB& operator *= (const RGB& RGB)
		{
			r *= RGB.r, g *= RGB.g, b *= RGB.b;
			return *this;
		}
		RGB& operator += (const RGB& RGB)
		{
			r += RGB.r, g += RGB.g, b += RGB.b;
			return *this;
		}
		friend uint8_t& operator += (uint8_t& f, const RGB RGB)
		{
			f += (RGB.r + RGB.g + RGB.b) / 3.f;
			return f;
		}
		friend int& operator += (int& f, const RGB RGB)
		{
			f += (RGB.r + RGB.g + RGB.b) / 3.f;
			return f;
		}
	};

private:
	static uint32_t ReadBigEndianUIntFromFile(std::ifstream& ifs);
	static uint16_t ReadBigEndianUShortFromFile(std::ifstream& ifs);

	void WritePixels(std::ofstream& ofs, RGB* const writePixels, const int ignore) const;

public:
	const std::string fileName;
	RGB* pixels; // 1D array of pixels
	const uint32_t width, height; // Image resolution
	const uint32_t pixelCount;
	std::string originalHeader;

	Image();
	Image(const uint32_t& w, const uint32_t& h, const std::string header);
	~Image();
	
	const RGB& operator [] (const unsigned int& i) const;
	RGB& operator [] (const unsigned int& i);

	void SaveAsBlackWhite(const char* fileName) const;
	void SaveAsQuantized(const char* fileName) const;

	static Image* FromBMP(const char* fileName);
};
