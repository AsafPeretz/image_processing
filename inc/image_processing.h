#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

typedef struct image {
  int height;           /* height in pixels */
  int width;            /* width in pixels */
  unsigned char *data;  /* height * width pixels, row major order */
} image;

typedef enum Image_Result {
	Image_Success,
	Image_Uninitialized_Error,
  Image_Allocation_Error,
  Image_Size_Error,
	Image_KernelSize_Error,
} Image_Result;

/**
 * @brief Performs convolution on @src, using @kernel, and writes the result to @dst.
 * 
 * @param[in] src - source image (must have same dimensios as @dst)
 * @param[in] kernel - kernel for convolution (squre, with odd dimensions, row major order)
 * @param[in] kernel_size - size of the kernel.
 * @param[out] dst - destination image (must have same dimensios as @src)
 *
 * @return success or error code
 * @return Image_Success on success
 * @return Image_Uninitialized_Error if input @dst, @src or @kernel are not initialized
 * @return Image_Size_Error if input @dst and @src have different dimensions
 * @return Image_KernelSize_Error if input @kernel_size is equal to zero or even size
**/
Image_Result image_convolution(struct image *dst, const struct image *src, const double *kernel, int kernel_size);


/**
 * @brief This function equalization @src's histogram. It then applies the equalized
 *        histogram to @src and writes the modified image to @dst.
 * 
 * @param[in] src - source image (must have same dimensios as @dst)
 * @param[out] dst - destination image (must have same dimensios as @src)
 *
 * @return success or error code
 * @return Image_Success on success
 * @return Image_Uninitialized_Error if input @dst or @src are not initialized
 * @return Image_Allocation_Error if comulative distribution table's allocation failed
 * @return Image_Size_Error if input @dst and @src have different dimensions
**/
Image_Result image_he(struct image *dst, const struct image *src);


/**
 * @brief This function finds @img's minimum and maximum pixel values.
 * 
 * @param[in] img - image to be analyzed
 * @param[out] min - minimum pixel value in @img
 * @param[out] max - maximum pixel value in @img
 *
 * @return success or error code
 * @return Image_Success on success
 * @return Image_Uninitialized_Error if input @img is not initialized
**/
Image_Result image_find_min_max(const struct image *img, unsigned char *min, unsigned char *max);

#endif /* IMAGE_PROCESSING_H */