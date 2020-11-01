#define WIN32_LEAN_AND_MEAN

#include <iostream>

#include "Image.hpp"

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif


int main()
{
	Image* im = Image::FromBMP("obrazek.bmp");

	if (im == nullptr)
		return 1;

	//im->SaveAsBlackWhite("obrazek-BaW.bmp");

	im->SaveAsQuantized("obrazek-Q.bmp");

	//Image::RGB* pix = nullptr;
	//for (int h = 0; h < im->h; h++)
	//{
	//	for (int w = 0; w < im->w; w++)
	//	{
	//		pix = &(im->pixels[h * im->w + w]);
	//		printf("| %3hi;%3hi;%3hi | ", pix->r, pix->g, pix->b);
	//	}
	//	printf("\n");
	//}

	// Konec
	delete im;

	//std::cout << "End... ";
	//std::cin.get();
	return 0;
}
