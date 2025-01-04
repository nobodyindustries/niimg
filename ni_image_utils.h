#ifndef NI_INCLUDE_IMAGE_UTILS
#define NI_INCLUDE_IMAGE_UTILS

#ifndef STBI_INCLUDE_STB_IMAGE_H
#include "stb_image.h"
#endif // STBI_INCLUDE_STB_IMAGE_H

#include <errno.h>

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

// clang-format off
/**
 * Macros to do a for each channel loop
 * 
 * N -> number of channels
 * 
 * In the body, the current channel is available in the __c variable.
 */
#define BEGIN_FOREACH_CHANNEL(N) \
	for(int __c = 0; __c < (N); __c++) {
/**
 * Finishes the BEGIN_FOREACH_CHANNEL loop.
 */
#define END_FOREACH_CHANNEL \
	}
// clang-format on

// clang-format off
/**
 * Macros to do a for each over a convolution
 * 
 * R -> radius of the convolution (floor of kernel_size divided by 2)
 * 
 * In the body, the current steps are available in the __sx and __sy variables.
 */
#define BEGIN_FOREACH_STEP(R) \
	for(int __sy = -(R); __sy <= (R); __sy++) { \
		for(int __sx = -(R); __sx <= (R); __sx++) {

/**
 * Finishes the BEGIN_FOREACH_STEP loop.
 */
#define END_FOREACH_STEP \
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

/**
 * Normalizes a byte value from the range [0, 255] into the range [0, 1]
 * as a double.
 * 
 * stbi_uc val -> value to be normalized
 * 
 * Returns the normalized value in the range [0, 1] as a double
 */
static inline double ni_stbi_uc_normalize(stbi_uc value);

/**
 * Converts a normalized value from the range [0, 1] into the ranfe [0, 255] 
 * as a stbi_uc.
 * 
 * double val -> value to be un-normalized
 * 
 * Returns the un-normalized value in the range [0, 255] as a stbi_uc
 */
static inline stbi_uc ni_stbi_uc_unnormalize(double value);

/**
 * Clamps a double value to keep it in the interval [0.0, 1.0].
 *
 * double val -> the value to clamp
 *
 * returns the value clamped to the interval [0.0, 1.0] as a double
 */
static inline double ni_image_data_clamp(double val);

/**
 * Prints an array of doubles to stdout, usually for debug purposes.
 *
 * const double *data -> the array of data to be printed
 * int w -> width of the array
 * int h -> height of the array
 * int n_channels -> number of channels of the array
 *
 */
void printad(const double *data, int w, int h, int n_channels);

/**
 * Enum to represent the possible results of a conversion of string
 * to a numeric value
 */
typedef enum __NI_IMAGE_NUMERIC_CONVERSION_RESULT {
	NI_IMG_NUMERIC_CONVERSION_OK,
	NI_IMG_NUMERIC_CONVERSION_ERR
} NI_IMAGE_NUMERIC_CONVERSION_RESULT;

/**
 * Converts a string to an int reporting errors. Uses strtol internally
 *
 * const char* str -> the string to convert to number
 * int* n -> contains the integer result of the conversion
 *
 * returns a value of the NI_IMAGE_CONVERSION_RESULT that indicates whether
 * it has been successful or not
 */
NI_IMAGE_NUMERIC_CONVERSION_RESULT ni_image_str2int(const char *str, int *n);

/**
 * Converts a string to a double reporting errors. Uses strtol internally
 *
 * const char* str -> the string to convert to number
 * double* n -> contains the integer result of the conversion
 *
 * returns a value of the NI_IMAGE_CONVERSION_RESULT that indicates whether
 * it has been successful or not
 */
NI_IMAGE_NUMERIC_CONVERSION_RESULT ni_image_str2double(const char *str, double *n);

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

static inline double
ni_stbi_uc_normalize(stbi_uc val)
{
	return ((double)val) / ((double)UCHAR_MAX);
}

static inline stbi_uc
ni_stbi_uc_unnormalize(double val)
{
	return (stbi_uc)round(val * UCHAR_MAX);
}

static inline double
ni_image_data_clamp(double val)
{
	if(val >= 1.0)
		return 1.0;
	if(val <= 0.0)
		return 0.0;
	return val;
}

void
printad(const double *data, int w, int h, int n_channels)
{
	int idx;
	double total = 0.0;
	fprintf(stdout, "[");
	BEGIN_FOREACH_PIXEL(w, h)
	if(__x == 0) fprintf(stdout, "\n");
	idx = PX_IDX(__x, __y, w, n_channels);
	fprintf(stdout, "%.8lf, ", data[idx]);
	total += data[idx];
	END_FOREACH_PIXEL
	fprintf(stdout, "\n]\nTOTAL: %.8lf\n", total);
	fflush(stdout);
}

NI_IMAGE_NUMERIC_CONVERSION_RESULT
ni_image_str2int(const char *str, int *n)
{
	char *end;
	long int num;

	errno = 0;

	num = strtol(str, &end, 10);
	if(end == str)
		goto error;
	else if(*end != '\0')
		goto error;
	else if(num > INT_MAX)
		goto error;
	else if(num < INT_MIN)
		goto error;
	else if(num == 0 && errno == EINVAL)
		goto error;
	*n = (int)num;
	return NI_IMG_NUMERIC_CONVERSION_OK;

error:
	n = NULL;
	return NI_IMG_NUMERIC_CONVERSION_ERR;
}

NI_IMAGE_NUMERIC_CONVERSION_RESULT
ni_image_str2double(const char *str, double *n)
{
	char *end;
	double num;

	errno = 0;

	num = strtod(str, &end);
	if(end == str)
		goto error;
	else if(*end != '\0')
		goto error;
	else if(num == HUGE_VAL && errno == ERANGE)
		goto error;
	else if(num == HUGE_VALF && errno == ERANGE)
		goto error;
	else if(num == HUGE_VALL && errno == ERANGE)
		goto error;
	else if(num == NAN)
		goto error;
	*n = num;
	return NI_IMG_NUMERIC_CONVERSION_OK;

error:
	n = NULL;
	return NI_IMG_NUMERIC_CONVERSION_ERR;
}

#endif // NI_IMAGE_UTILS_IMPLEMENTATION

#endif // NI_INCLUDE_IMAGE_UTILS