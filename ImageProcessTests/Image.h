#pragma once

#include <fstream>

class Image
{
private:
	static uint32_t ReadBigEndianUIntFromFile(std::ifstream& ifs);
	static uint16_t ReadBigEndianUShortFromFile(std::ifstream& ifs);

public:
	// RGB structure, i.e. a pixel 
	struct RGB
	{
		uint8_t r, g, b;

		RGB() : r(0), g(0), b(0) {}
		RGB(int c) : r(c), g(c), b(c) {}
		RGB(int _r, int _g, int _b) : r(_r), g(_g), b(_b) {}

		uint8_t GetLightness() const
		{
			// 0.94117 = 240 / 255 (conversion to max value of lightness 240)
			return (std::max(std::max(r, g), b) + std::min(std::min(r, g), b)) / 2 * 0.94117f;
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

	RGB* pixels; // 1D array of pixels
	uint32_t width, height; // Image resolution
	std::string originalHeader;

	Image();
	Image(const uint32_t& w, const uint32_t& h, const std::string header);
	~Image();
	
	const RGB& operator [] (const unsigned int& i) const;
	RGB& operator [] (const unsigned int& i);

	void SaveAsBlackWhite() const;

	static Image* FromBMP(const char* fileName);
};
