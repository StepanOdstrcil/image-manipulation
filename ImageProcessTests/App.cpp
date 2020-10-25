#include <iostream>

#include "Image.h"

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

	im->SaveAsBlackWhite();

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
	//std::getchar();
	return 0;
}
