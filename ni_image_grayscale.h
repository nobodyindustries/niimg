#ifndef NI_INCLUDE_GRAYSCALE
#define NI_INCLUDE_GRAYSCALE

#include "math.h"

#ifndef NI_INCLUDE_IMAGE_UTILS
#define NI_IMAGE_UTILS_IMPLEMENTATION
#include "ni_image_utils.h"
#endif

// = DECLARATION =

/**
 * Types of algorithms that can be used to convert to grayscale
 */
typedef enum __NI_IMAGE_GRAYSCALE_STD {
	NI_ITU_BT_601, // ITU BT.601
	NI_ITU_BT_709, // ITU BT.709
	NI_SMPTE_240M, // SMPTE 240M
} NI_IMAGE_GRAYSCALE_STD;

/**
 * Converts the image to monochrome and returns the data
 * on a new image, that needs to be freed afterwards.
 *
 * w -> width of the original image
 * h -> height of the original image
 * n_channels -> number of channels of the original image
 *
 * Note: n_channels needs to be three for now, as it only supports
 * RGB images, but maybe in the future there's more options.
 */
stbi_uc *ni_image_grayscale_convert(const stbi_uc *img_data, int w, int h, int n_channels, NI_IMAGE_GRAYSCALE_STD type);

/**
 * Creates a representation of a grayscale image (1 channel, 1 byte per
 * channel) into a normalized array of doubles.
 *
 * const stbi_uc *img_data -> original image data
 * int w -> original image width
 * int h -> original image height
 *
 * returns: pointer to a newly created normalized floating point array
 * with equivalent data to the original image. This needs to be freed
 * outside.
 */
double *ni_grayscale_fp_convert(const stbi_uc *img_data, int w, int h);

/**
 * Creates a representation of a data array (1 double per pixel) into a
 * stbi_uc array which has values in [0, 255]
 *
 * const double *img_data -> original floating point image data
 * int w -> original image width
 * int h -> original image height
 *
 * returns: pointer to a newly created array of stbi_uc with equivalent
 * data to the original floating point array. This needs to be freed
 * outside.
 */
stbi_uc *ni_fp_grayscale_convert(const double *img_data, int w, int h);

/**
 * Converts and normalizes a stbi_uc value to a double. The ranges are
 * converted as follows: [0, 255] -> [0.0, 1.0]
 *
 * stbi_uc val -> value to be normalized
 *
 * Returns the normalized value in the range [0, 1] as a double
 */
static inline double gray_normalize(stbi_uc val);

/**
 * Converts a normalized double value to a stbi_uc value. The ranges are
 * converted as follows [0.0, 1.0] -> [0, 255]
 *
 * double val -> value to be normalized
 *
 * Returns the un-normalized value in the range [0, 255] as a stbi_uc
 */
static inline stbi_uc gray_unnormalize(double val);

// = IMPLEMENTATION =
#ifdef NI_GRAYSCALE_IMPLEMENTATION

/**
 * Note: this function is intended only for internal usage
 *
 * Calculates the luma value for an RGB tuple according to ITU BT.601
 *
 * r -> red value (stbi_uc)
 * g -> green value (stbi_uc)
 * b -> blue value (stbi_uc)
 *
 * Returns the luma value for the rgb tuple
 */
static inline stbi_uc
__ni_image_grayscale_itu_bt_601(stbi_uc r, stbi_uc g, stbi_uc b)
{
	// LUMA = 0.299 R + 0.587 G + 0.114 B
	double l =
		(0.299 * ((double)r)) + (0.587 * ((double)g)) + (0.114 * ((double)b));
	return (stbi_uc)round(l);
}

/**
 * Note: this function is intended only for internal usage
 *
 * Calculates the luma value for an RGB tuple according to ITU BT.709
 *
 * r -> red value (stbi_uc)
 * g -> green value (stbi_uc)
 * b -> blue value (stbi_uc)
 *
 * Returns the luma value for the rgb tuple
 */
static inline stbi_uc
__ni_image_grayscale_itu_bt_709(stbi_uc r, stbi_uc g, stbi_uc b)
{
	// LUMA = 0.2126 R + 0.7152 G + 0.0722 B
	double l =
		(0.2126 * ((double)r)) + (0.7152 * ((double)g)) + (0.0722 * ((double)b));
	return (stbi_uc)round(l);
}

/**
 * Note: this function is intended only for internal usage
 *
 * Calculates the luma value for an RGB tuple according to SMPTE 240M
 *
 * r -> red value (stbi_uc)
 * g -> green value (stbi_uc)
 * b -> blue value (stbi_uc)
 *
 * Returns the luma value for the rgb tuple
 */
static inline stbi_uc
__ni_image_grayscale_smpte_240m(stbi_uc r, stbi_uc g, stbi_uc b)
{
	// LUMA = 0.212 R + 0.701 G + 0.087 B
	double l =
		(0.212 * ((double)r)) + (0.701 * ((double)g)) + (0.087 * ((double)b));
	return (stbi_uc)round(l);
}

static inline double
gray_normalize(stbi_uc val)
{
	return ((double)val) / ((double)UCHAR_MAX);
}

static inline stbi_uc
gray_unnormalize(double val)
{
	return (stbi_uc)round(val * UCHAR_MAX);
}

stbi_uc *
ni_image_grayscale_convert(const stbi_uc *img_data, int w, int h, int n_channels, NI_IMAGE_GRAYSCALE_STD type)
{
	assert(n_channels == 3);
	stbi_uc *ret_img =
		ni_image_create(w, h, 1); // This image is grayscale -> 1 channel

	int idx, n_idx;
	stbi_uc r, g, b, l;

	BEGIN_FOREACH_PIXEL(w, h)
	idx = PX_IDX(__x, __y, w, n_channels);
	r = img_data[idx];
	g = img_data[idx + 1];
	b = img_data[idx + 2];
	switch(type) {
	case(NI_ITU_BT_601):
		l = __ni_image_grayscale_itu_bt_601(r, g, b);
		break;
	case(NI_ITU_BT_709):
		l = __ni_image_grayscale_itu_bt_709(r, g, b);
		break;
	case(NI_SMPTE_240M):
		l = __ni_image_grayscale_smpte_240m(r, g, b);
		break;
	default:
		l = 0; // Should never reach this default
		break;
	}
	n_idx = PX_IDX(__x, __y, w, 1);
	ret_img[n_idx] = l;
	END_FOREACH_PIXEL

	return ret_img;
}

double *
ni_grayscale_fp_convert(const stbi_uc *img_data, int w, int h)
{
	double *data = ni_data_create(w, h, 1);

	int idx;
	stbi_uc value;
	double new_value;
	BEGIN_FOREACH_PIXEL(w, h)
	idx = PX_IDX(__x, __y, w, 1);
	value = img_data[idx];
	new_value = gray_normalize(value);
	data[idx] = new_value;
	END_FOREACH_PIXEL

	return data;
}

stbi_uc *
ni_fp_grayscale_convert(const double *img_data, int w, int h)
{
	stbi_uc *img = ni_image_create(w, h, 1);

	int idx;
	double value;
	stbi_uc new_value;
	BEGIN_FOREACH_PIXEL(w, h)
	idx = PX_IDX(__x, __y, w, 1);
	value = img_data[idx];
	new_value = gray_unnormalize(value);
	img[idx] = new_value;
	END_FOREACH_PIXEL

	return img;
}

#endif // NI_GRAYSCALE_IMPLEMENTATION

#endif // NI_INCLUDE_GRAYSCALE