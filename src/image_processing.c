#include "image_processing.h"
#include <stdio.h>
#include <stdlib.h> /* malloc, free */
#include <limits.h> /* UCHAR_MAX */
#include <math.h>   /* round */


#define CENTRAL_KERNEL_INDEX(size) ((size)*((size)/2) + ((size)/2))
#define IMAGE_MATRIX_SIZE(image) ((image)->height * (image)->width)

#define CHECK_MIN_MAX(value, min, max) \
  if (value < min) {                   \
    min = value;                       \
  }                                    \
  else if (value > max) {              \
    max = value;                       \
  }


static Image_Result convolution_validation_checking(const image *dst, const image *src, const double *kernel, int kernel_size);
static int image_size_compare(const image *first, const image *second);
static double pixel_convolution_center(const image *img, const double *kernel, int kernel_size, size_t image_index);
static void pixel_extend_right_left_sides(image *dst, int kernel_size, int first_col, int last_col, int first_in_col_to_extend);
static void pixel_extend_top_bottom_sides(image *dst, int kernel_size, int first_row, int last_row, int first_in_row_to_extend);

static void image_intensity_counting(size_t *intensity_table, const image *src, size_t table_size, unsigned char min);
static void image_cumulative_distribution(size_t *intensity_table, size_t table_size);
static void image_histogram_equalization(size_t *intensity_table, const image *src, size_t table_size);
static void image_dst_populate(image *dst, const image *src, size_t *intensity_table, unsigned char min);


Image_Result image_convolution(image *dst, const image *src, const double *kernel, int kernel_size) {
  size_t row, col, image_index;
  Image_Result status = convolution_validation_checking(dst, src, kernel, kernel_size);
  if (Image_Success != status) {
    return status;
  }

  /* convolution inner squre */
  for (row = kernel_size/2 ; row < dst->height - (kernel_size/2) ; ++row) { 
    for (col = kernel_size/2 ; col < dst->width - (kernel_size/2) ; ++col) {
      image_index = row * dst->width + col;
      dst->data[image_index] = pixel_convolution_center(src, kernel, kernel_size, image_index);
    }
  }

  /* extend top & bottom */
  pixel_extend_top_bottom_sides(dst, kernel_size, 0, kernel_size/2, kernel_size/2 * dst->width);
  pixel_extend_top_bottom_sides(dst, kernel_size, dst->height - kernel_size/2, dst->height, (dst->height - kernel_size/2 - 1) * dst->width);
  
  /* extend left & right, incluing corners */
  pixel_extend_right_left_sides(dst, kernel_size, 0, kernel_size/2, kernel_size/2);
  pixel_extend_right_left_sides(dst, kernel_size, dst->width - kernel_size/2, dst->width, dst->width - kernel_size/2 - 1);
  
  return Image_Success;
}


Image_Result image_he(image *dst, const image *src) {
  unsigned char min, max;
  size_t *intensity_table = NULL, intensity_table_size;
  if (NULL == dst || NULL == src) {
    return Image_Uninitialized_Error;
  }
  if (image_size_compare(dst, src) == 0 || IMAGE_MATRIX_SIZE(src) == 0) {
    return Image_Size_Error;
  }
  image_find_min_max(src, &min, &max);
  intensity_table_size = max - min + 1;
  if (NULL == (intensity_table = (size_t*)calloc(intensity_table_size, sizeof(size_t)))) {
    return Image_Allocation_Error;
  }
  image_intensity_counting(intensity_table, src, IMAGE_MATRIX_SIZE(src), min);
  image_cumulative_distribution(intensity_table, intensity_table_size);
  image_histogram_equalization(intensity_table, src, intensity_table_size);
  image_dst_populate(dst, src, intensity_table, min);
  free(intensity_table);
  return Image_Success;
}


Image_Result image_find_min_max(const image *img, unsigned char *min, unsigned char *max) {
  int i = 1, image_size;
  if (NULL == img || NULL == min || NULL == max) {
    return Image_Uninitialized_Error;
  }
  *min = img->data[0];
  *max = img->data[0];
  image_size = img->height * img->width;
  for ( ; i < image_size - 3 ; i += 4) {
    CHECK_MIN_MAX(img->data[i], *min, *max)
    CHECK_MIN_MAX(img->data[i+1], *min, *max)
    CHECK_MIN_MAX(img->data[i+2], *min, *max)
    CHECK_MIN_MAX(img->data[i+3], *min, *max)
  }

  /* check for last elements */
  for ( ; i < image_size ; ++i) {
    CHECK_MIN_MAX(img->data[i], *min, *max)
  }

  return Image_Success;
}




/* static functions */

static double pixel_convolution_center(const image *img, const double *kernel, int kernel_size, size_t image_index) {
  double retval = 0;
  size_t current_image_index, current_kernel_index;
  int i, j;
  for (i = -(kernel_size / 2) ; i <= kernel_size / 2 ; ++i) {
    for (j = -(kernel_size / 2) ; j <= kernel_size / 2 ; ++j) {
      current_image_index = image_index + (i * img->width) + j;
      current_kernel_index = CENTRAL_KERNEL_INDEX(kernel_size) - (i * kernel_size) - j;
      retval += img->data[current_image_index] * kernel[current_kernel_index];
    }
  }
  return retval > UCHAR_MAX ? UCHAR_MAX : retval < 0 ? 0 : retval;
}


/* not include corners */
static void pixel_extend_top_bottom_sides(image *dst, int kernel_size, int first_row, int last_row, int first_in_row_to_extend) {
  int row, col;
  for (row = first_row ; row < last_row ; ++row) { 
    for (col = kernel_size/2 ; col < dst->width - kernel_size/2 ; ++col) {
      dst->data[row * dst->width + col] = dst->data[first_in_row_to_extend + col];
    }
  }
}


/* include corners */
static void pixel_extend_right_left_sides(image *dst, int kernel_size, int first_col, int last_col, int first_in_col_to_extend) {
  int row, col;
  for (row = 0 ; row < dst->height ; ++row) { 
    for (col = first_col ; col < last_col ; ++col) {
      dst->data[row * dst->width + col] = dst->data[row * dst->width + first_in_col_to_extend];
    }
  }
}


static int image_size_compare(const image* first, const image* second) {
  return (first->height == second->height && first->width == second->width);
}


static void image_intensity_counting(size_t *intensity_table, const image *src, size_t table_size, unsigned char min) {
  unsigned int i = 0;
  for ( ; i < table_size ; ++i) {
    ++(intensity_table[src->data[i] - min]);
  }
}


static void image_cumulative_distribution(size_t *intensity_table, size_t table_size) {
  unsigned int i = 1;
  for ( ; i < table_size ; ++i) {
    intensity_table[i] += intensity_table[i-1];
  }
}


static void image_histogram_equalization(size_t *intensity_table, const image *src, size_t table_size) {
  unsigned int i = 0;
  unsigned char cdf_min = intensity_table[0];
  for ( ; i < table_size ; ++i) {
    intensity_table[i] = round((((double)intensity_table[i] - cdf_min) * (UCHAR_MAX)) / (IMAGE_MATRIX_SIZE(src) - cdf_min));
  }
}


static void image_dst_populate(image *dst, const image *src, size_t *intensity_table, unsigned char min) {
  unsigned int i = 0;
  size_t size = IMAGE_MATRIX_SIZE(dst);
  for (i = 0 ; i < size ; ++i) {
    dst->data[i] = intensity_table[src->data[i] - min];
  }
}

static Image_Result convolution_validation_checking(const image *dst, const image *src, const double *kernel, int kernel_size) {
  if (kernel_size % 2 == 0 || kernel_size < 0) {
    return Image_KernelSize_Error;
  }
  if (NULL == dst || NULL == src || NULL == kernel) {
    return Image_Uninitialized_Error;
  }
  if (image_size_compare(dst, src) == 0 || IMAGE_MATRIX_SIZE(src) == 0) {
    return Image_Size_Error;
  }
  return Image_Success;
}