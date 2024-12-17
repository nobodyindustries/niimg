#ifndef NI_INCLUDE_DITHER
#define NI_INCLUDE_DITHER

#include "math.h"

#define NI_IMAGE_UTILS_IMPLEMENTATION
#include "ni_image_utils.h"

// = DECLARATION =

/**
 * Creates and returns a monochromatic (black and white) image that is the
 * result of the application of the Floyd-Steinberg dithering algorithm to the
 * original image, which should be grayscale (1 channel).
 *
 * img_data -> pointer to the data of the original image
 * w -> width of the original image
 * h -> height of the original image
 *
 * returns a new, dithered image which needs to be freed separately.
 */
stbi_uc *ni_image_dither_floydsteinberg_gray2mono(const stbi_uc *img_data,
                                                  int w, int h);

// = IMPLEMENTATION =

#ifdef NI_DITHER_IMPLEMENTATION

/**
 * Returns the closest monochrome color for a data point. Only intended for
 * internal usage.
 *
 * double datapx -> the value to analyze
 *
 * returns the floating point value of the closest color (1.0 or 0.0)
 */
static inline double __ni_image_closest_mono(double datapx) {
  if (datapx >= 0.5)
    return 1.0;
  return 0.0;
}

/**
 * Clamps a double value to keep it in the interval [0.0, 1.0]. Only intended
 * for internal usage.
 *
 * double val -> the value to clamp
 *
 * returns the value clamped to the interval [0.0, 1.0] as a double
 */
static inline double __ni_image_data_clamp(double val) {
  if (val >= 1.0)
    return 1.0;
  if (val <= 0.0)
    return 0.0;
  return val;
}

stbi_uc *ni_image_dither_floydsteinberg_gray2mono(const stbi_uc *img_data,
                                                  int w, int h) {
  double *data = ni_grayscale_fp_convert(img_data, w, h);

  int idx;
  double oldpx, newpx, err;
  BEGIN_FOREACH_PIXEL(w, h)
  idx = PX_IDX(__x, __y, w, 1);
  oldpx = data[idx];
  newpx = __ni_image_closest_mono(oldpx);
  data[idx] = newpx;
  err = oldpx - newpx;
  if (PX_VALID(__x + 1, __y, w, h)) {
    idx = PX_IDX(__x + 1, __y, w, 1);
    data[idx] = __ni_image_data_clamp(data[idx] + (err * 7 / 16));
  }
  if (PX_VALID(__x - 1, __y + 1, w, h)) {
    idx = PX_IDX(__x - 1, __y + 1, w, 1);
    data[idx] = __ni_image_data_clamp(data[idx] + (err * 3 / 16));
  }
  if (PX_VALID(__x, __y + 1, w, h)) {
    idx = PX_IDX(__x, __y + 1, w, 1);
    data[idx] = __ni_image_data_clamp(data[idx] + (err * 5 / 16));
  }
  if (PX_VALID(__x + 1, __y + 1, w, h)) {
    idx = PX_IDX(__x + 1, __y + 1, w, 1);
    data[idx] = __ni_image_data_clamp(data[idx] + (err * 1 / 16));
  }
  END_FOREACH_PIXEL

  stbi_uc *ret_img = ni_fp_grayscale_convert(data, w, h);

  free((void *)data);
  return ret_img;
}

#endif // NI_DITHER_IMPLEMENTATION

#endif // NI_INCLUDE_DITHER