#ifndef NI_INCLUDE_BLUR
#define NI_INCLUDE_BLUR

#include "math.h"

#ifndef NI_INCLUDE_IMAGE_UTILS
#define NI_IMAGE_UTILS_IMPLEMENTATION
#include "ni_image_utils.h"
#endif

/**
 * Applies Gaussian blur to an image and returns the result on a new image, that
 * needs to be freed outside.
 *
 * const stbi_uc *img_data -> data of the original image
 * int w -> original image width
 * int h -> original image height
 * int n_channels -> number of channels of the original image
 * int kernel_size -> size of the gaussian kernel to be used for the
 * convolution.
 * double sigma -> standard deviation of the gaussian distribution.
 *
 * returns a new stbi_uc array which represents the blurred image or NULL on error.
 * 
 * Error conditions:
 *  -> kernel_size % 2 == 0
 */
stbi_uc *ni_image_blur_gaussian(const stbi_uc *img_data, int w, int h, int n_channels, int kernel_size, double sigma);

#ifdef NI_BLUR_IMPLEMENTATION

/**
 * Calculates the value of a specific coordinate in the Gaussian kernel, intended
 * only for internal usage.
 * 
 * int x -> x-coordinate inside of kernel
 * int y -> y-coordinate inside of kernel
 * double sigma -> standard deviation of the distribution
 * 
 * returns double containing the value
 */
static inline double
__ni_image_gaussian_kernel_value(int x, int y, double radius, double sigma)
{
	const double x_dist = ((double)x) - radius;
	const double y_dist = ((double)y) - radius;
	const double d_sigma_squared = 2 * sigma * sigma;
	const double dist = (x_dist * x_dist) + (y_dist * y_dist);
	const double factor = 1 / (M_PI * d_sigma_squared);
	return factor * exp(-(dist / d_sigma_squared));
}

/**
 * Creates a Gaussian Kernel as an array of doubles. The kernel size needs to be
 * odd. This function is meant to be used only internally and the resulting
 * array needs to be freed outside.
 *
 * int kernel_size -> size of the kernel to be generated. Needs to be odd.
 * double sigma -> standard deviation of the gaussian distribution.
 *
 * returns a double array of the right size for the kernel. It is always
 * 1-dimensional, and the coordinates can be accessed the same way you would
 * access the coordinates of an image. Returns NULL on error.
 *
 * Error conditions:
 *  -> kernel_size % 2 == 0
 */
double *
__ni_image_get_gaussian_blur_kernel(int kernel_size, double sigma)
{
	// ERROR: the kernel size is even
	if(kernel_size % 2 == 0)
		return NULL;

	double radius = floor(((double)kernel_size) / 2.0);
	//double sigma = radius / 2.0;
	//double sigma = 0.84089642;

	double *kernel = ni_data_create(kernel_size, kernel_size, 1);

	double sum = 0.0;
	int idx;

	// Calculate values
	BEGIN_FOREACH_PIXEL(kernel_size, kernel_size)
	idx = PX_IDX(__x, __y, kernel_size, 1);
	kernel[idx] = __ni_image_gaussian_kernel_value(__x, __y, radius, sigma);
	sum += kernel[idx];
	END_FOREACH_PIXEL

	// Normalize in the range [0, 1]
	BEGIN_FOREACH_PIXEL(kernel_size, kernel_size)
	idx = PX_IDX(__x, __y, kernel_size, 1);
	kernel[idx] /= sum;
	END_FOREACH_PIXEL

	return kernel;
}

stbi_uc *
ni_image_blur_gaussian(const stbi_uc *img_data, int w, int h, int n_channels, int kernel_size, double sigma)
{
	// ERROR: the kernel size is even
	if(kernel_size % 2 == 0)
		return NULL;

	double *kernel = __ni_image_get_gaussian_blur_kernel(kernel_size, sigma);
	// Kernel could not be created
	if(kernel == NULL)
		return NULL;

#ifdef NI_BLUR_DEBUG
	fprintf(stdout, "[DEBUG] - ni_image_blur_gaussian kernel\n");
	printad(kernel, kernel_size, kernel_size, 1);
#endif

	int convolution_radius = (int)floor(((double)kernel_size) / 2.0);

	// -- CONVERT TO DATA --
	double *data = ni_data_create(w, h, n_channels);
	int idx;
	double nd_val;
	BEGIN_FOREACH_CHANNEL(n_channels)
	BEGIN_FOREACH_PIXEL(w, h)
	idx = PX_IDX(__x, __y, w, n_channels);
	nd_val = ni_stbi_uc_normalize(img_data[idx + __c]);
	data[idx + __c] = nd_val;
	END_FOREACH_PIXEL
	END_FOREACH_CHANNEL

	// -- CONVOLVE --
	double *new_data = ni_data_create(w, h, n_channels);
	int kernel_idx, c_idx;
	double sum, n_value;
	BEGIN_FOREACH_CHANNEL(n_channels)
	BEGIN_FOREACH_PIXEL(w, h)
	idx = PX_IDX(__x, __y, w, n_channels);
	sum = 0.0;
	BEGIN_FOREACH_STEP(convolution_radius)
	if(PX_VALID(__x + __sx, __y + __sy, w, h)) {
		c_idx = PX_IDX(__x + __sx, __y + __sy, w, n_channels);
		kernel_idx = PX_IDX(convolution_radius + __sx, convolution_radius + __sy, kernel_size, 1);
		n_value = data[c_idx + __c] * kernel[kernel_idx];
		sum += n_value;
	}
	END_FOREACH_STEP
	new_data[idx + __c] = ni_image_data_clamp(sum);
	END_FOREACH_PIXEL
	END_FOREACH_CHANNEL

	// -- CONVERT BACK TO IMAGE --
	stbi_uc *img = ni_image_create(w, h, n_channels);
	stbi_uc nuc_val;
	BEGIN_FOREACH_CHANNEL(n_channels)
	BEGIN_FOREACH_PIXEL(w, h)
	idx = PX_IDX(__x, __y, w, n_channels);
	nuc_val = ni_stbi_uc_unnormalize(new_data[idx + __c]);
	img[idx + __c] = nuc_val;
	END_FOREACH_PIXEL
	END_FOREACH_CHANNEL

	free(kernel);
	free(data);
	free(new_data);
	return img;
}

#endif // NI_BLUR_IMPLEMENTATION

#endif // NI_INCLUDE_BLUR