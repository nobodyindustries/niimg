#ifndef NI_INCLUDE_IMAGE_UTILS
#define NI_INCLUDE_IMAGE_UTILS

#ifndef STBI_INCLUDE_STB_IMAGE_H
#include "stb_image.h"
#endif // STBI_INCLUDE_STB_IMAGE_H

// = DECLARATION =

// clang-format off
/**
 * Macros to do a for each pixel loop
 * 
 * W -> width of the image
 * H -> height of the image
 * 
 * In the body, the x and y of the current pixels are available in the 
 * __x and __y variables.
 */
#define BEGIN_FOREACH_PIXEL(W, H) \
	for(int __y = 0; __y < (H); __y++) { \
		for(int __x = 0; __x < (W); __x++) {
/**
 * Finishes the BEGIN_FOREACH_PIXEL loop.
 */
#define END_FOREACH_PIXEL \
	    } \
	}
// clang-format on

/**
 * Macro to calculate the index of a spefic pixel
 *
 * X -> pixel x position
 * Y -> pixel y position
 * W -> width of the image
 * N -> number of channels
 */
#define PX_IDX(X, Y, W, N) (int)(((Y) * (W)) + (X)) * (N)

/**
 * Macro to determine if a set of coordinates is valid
 *
 * X -> x position to evaluate
 * Y -> y position to evaluate
 * W -> width of the image
 * H -> height of the image
 *
 * returns true if X in [0, W) and Y in [0, H)
 */
#define PX_VALID(X, Y, W, H) \
	((X) >= 0) && ((X) < (W)) && ((Y) >= 0) && ((Y) < (H))

/**
 * Creates a new, empty image.
 *
 * int w -> image width
 * int h -> image height
 * int n_channels -> number of channels
 *
 * returns: pointer to image data array as used in stb_image.
 *
 * note: the data is not initialized so it may be garbage.
 */
stbi_uc *ni_image_create(int w, int h, int n_channels);

/**
 * Creates a new array of doubles to keep image information that requires
 * higher precision, one value per pixel
 *
 * int w -> image width
 * int h -> image height
 * int n_channels -> number of channels of the image
 *
 * returns: pointer to the floating point data array.
 *
 * note: the data is not initialized so it may be garbage.
 */
double *ni_data_create(int w, int h, int n_channels);

// = IMPLEMENTATION =
#ifdef NI_IMAGE_UTILS_IMPLEMENTATION

stbi_uc *
ni_image_create(int w, int h, int n_channels)
{
	const size_t sz = (w * h * n_channels) * sizeof(stbi_uc);
	return STBI_MALLOC(sz);
}

double *
ni_data_create(int w, int h, int n_channels)
{
	const size_t sz = (w * h * n_channels) * sizeof(double);
	return STBI_MALLOC(sz);
}

#endif // NI_IMAGE_UTILS_IMPLEMENTATION

#endif // NI_INCLUDE_IMAGE_UTILS