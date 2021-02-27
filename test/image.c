#include "image_processing.h"
#include <stdio.h> /* printf */
#include <stdlib.h> /* malloc, free, rand, srand */
#include <limits.h> /* UCHAR_MAX */
#include <string.h> /* strcpy */
#include <time.h> /* time */
#include <math.h>

#define PRINT(FUNC_PTR, STR) if (FUNC_PTR(STR) == 0) { printf("%s\n", STR); }


static image* image_create(unsigned int height, unsigned int width);
static void image_destroy(image **img);
static image* image_random_create(unsigned int height, unsigned int width);
static void fill_image_values(image *img, const unsigned char *values, size_t size);
static int compare_image_values(const unsigned char *first, const unsigned char *second, size_t size);
void print_image_data(const unsigned char *data, size_t height, size_t width);

int test_min_max(char *test_name);
int test_min_max_null(char *test_name);
int test_min_max_size_1x1(char *test_name);

int test_image_histogram(char *test_name);
int test_image_histogram_null(char *test_name);
void test_image_he_on_photo(char *test_name, const char* photo_path, const char* new_photo_path, size_t height, size_t width);

int test_image_convolution(char *test_name);
int test_image_convolution_identity(char *test_name);
int test_image_convolution_null(char *test_name);
int test_image_convolution_kernel_size(char *test_name);
int test_image_convolution_image_size_zero(char *test_name);
void test_image_convolution_on_photo(char *test_name, const char* photo_path, const char* new_photo_path, size_t height, size_t width);


int main() {
  char test_name[100];
  srand( time(NULL) );
  printf("\n*** Only Failed tests should be printed ***\n\n");


  /* image_find_min_max Function */
  PRINT(test_min_max, test_name)
  PRINT(test_min_max_null, test_name)
  PRINT(test_min_max_size_1x1, test_name)
  
  /* image_he Function */
  PRINT(test_image_histogram, test_name)
  PRINT(test_image_histogram_null, test_name)
  test_image_he_on_photo(test_name, "./hawkes_bay/unequalized_hawkes_bay_1024x683", "./hawkes_bay/he_unequalized_hawkes_bay_1024x683", 1024, 683);
  test_image_he_on_photo(test_name, "./rose/rose_1920x1280", "./rose/he_rose_1920x1280", 1920, 1280);
  test_image_he_on_photo(test_name, "./chess/blurry_chess_1920x1200", "./chess/he_blurry_chess_1920x1200", 1920, 1200);
  test_image_he_on_photo(test_name, "./elvis/unequalized_elvis_800x623", "./elvis/he_unequalized_elvis_800x623", 800, 623);
  
  /* image_convolution Function */
  PRINT(test_image_convolution, test_name)
  PRINT(test_image_convolution_identity, test_name)
  PRINT(test_image_convolution_null, test_name)
  PRINT(test_image_convolution_kernel_size, test_name)
  PRINT(test_image_convolution_image_size_zero, test_name)
  test_image_convolution_on_photo(test_name, "./hawkes_bay/unequalized_hawkes_bay_1024x683", "./hawkes_bay/convolution_unequalized_hawkes_bay_1024x683", 683, 1024);
  test_image_convolution_on_photo(test_name, "./rose/rose_1920x1280", "./rose/convolution_rose_1920x1280", 1280, 1920);
  test_image_convolution_on_photo(test_name, "./chess/blurry_chess_1920x1200", "./chess/convolution_blurry_chess_1920x1200", 1200, 1920);
  test_image_convolution_on_photo(test_name, "./elvis/unequalized_elvis_800x623", "./elvis/convolution_unequalized_elvis_800x623", 623, 800);

  return 0;
}



/* Tests */

/* image_find_min_max Function */

int test_min_max(char *test_name) {
  const unsigned int height = 8, width = 12, image_size = height * width;
  unsigned char min, max;
  size_t i = 0;
  image *img = image_random_create(height, width);
  strcpy(test_name, "test_min_max");
  if (NULL == img) {
    return 0;
  }
  if (Image_Success != image_find_min_max(img, &min, &max)) {
    image_destroy(&img);
    return 0;
  } 
  for ( ; i < image_size ; ++i) {
    if (img->data[i] < min || img->data[i] > max) {
      return 0;
    }
  }
  image_destroy(&img);
  return 1;
}

int test_min_max_null(char *test_name) {
  const unsigned int height = 8, width = 12;
  unsigned char min, max;
  image *img = image_create(height, width);
  strcpy(test_name, "test_min_max_null");
  if (NULL == img) {
    return 0;
  }
  if (Image_Uninitialized_Error != image_find_min_max(NULL, &min, &max)) {
    return 0;
  }
  if (Image_Uninitialized_Error != image_find_min_max(img, NULL, &max)) {
    return 0;
  }
  if (Image_Uninitialized_Error != image_find_min_max(img, &min, NULL)) {
    return 0;
  }
  image_destroy(&img);
  return 1;
}

int test_min_max_size_1x1(char *test_name) {
  const unsigned int height = 1, width = 1;
  unsigned char min, max;
  image* img = NULL;
  strcpy(test_name, "test_min_max_size_1x1");
  if (NULL == (img = image_random_create(height, width))) {
    return 0;
  }
  if (Image_Success != image_find_min_max(img, &min, &max)) {
    image_destroy(&img);
    return 0;
  }
  if (min != max || min != img->data[0]) {
    return 0;
  }
  image_destroy(&img);
  return 1;
}



/* image_he Function */

int test_image_histogram(char *test_name) {
  const size_t height = 8, width = 8;
  image *src = NULL, *dst = NULL;

  /* The values were taken from Wikipedia - "Histogram equalization" */
  unsigned char image_pixels_before_he[] = { 52, 55, 61, 59, 79, 61, 76, 61, 62, 59, 55, 104, 94, 85, 59, 71, 63, 65, 66, 113, 144, 104, 63, 72, 64, 70, 70, 126, 154, 109, 71, 69, 67, 73, 68, 106, 122, 88, 68, 68, 68, 79, 60, 70, 77, 66, 58, 75, 69, 85, 64, 58, 55, 61, 65, 83, 70, 87, 69, 68, 65, 73, 78, 90 };
  unsigned char image_pixels_after_he[] = { 0, 12, 53, 32, 190, 53, 174, 53, 57, 32, 12, 227, 219, 202, 32, 154, 65, 85, 93, 239, 251, 227, 65, 158, 73, 146, 146, 247, 255, 235, 154, 130, 97, 166, 117, 231, 243, 210, 117, 117, 117, 190, 36, 146, 178, 93, 20, 170, 130, 202, 73, 20, 12, 53, 85, 194, 146, 206, 130, 117, 85, 166, 182, 215 };

  strcpy(test_name, "test_image_histogram");
  if (NULL == (src = image_create(height, width))) {
    return 0;
  }
  if (NULL == (dst = image_create(height, width))) {
    image_destroy(&src);
    return 0;
  }
  fill_image_values(src, image_pixels_before_he, sizeof(image_pixels_before_he));
  if (Image_Success != image_he(dst, src)) {
    image_destroy(&src);
    image_destroy(&dst);
    return 0;
  }
  if (0 == compare_image_values(dst->data, image_pixels_after_he, sizeof(image_pixels_after_he))) {
    image_destroy(&src);
    image_destroy(&dst);
    return 0;
  }
  image_destroy(&dst);
  image_destroy(&src);
  return 1;
}

int test_image_histogram_null(char *test_name) {
  const unsigned int height = 8, width = 12;
  image *img = image_create(height, width);
  strcpy(test_name, "test_image_histogram_null");
  if (NULL == img) {
    return 0;
  }
  if (Image_Uninitialized_Error != image_he(img, NULL)) {
    image_destroy(&img);
    return 0;
  }
  if (Image_Uninitialized_Error != image_he(NULL, img)) {
    image_destroy(&img);
    return 0;
  }
  image_destroy(&img);
  return 1;
}


void test_image_he_on_photo(char *test_name, const char* photo_path, const char* new_photo_path, size_t height, size_t width) {
	FILE *file_ptr = NULL;
	size_t size = height * width;
	image *src = NULL, *dst = NULL;
	if (NULL == (src = image_create(height, width))) {
    return;
  }
	file_ptr = fopen(photo_path, "r");
	fread(src->data, sizeof(unsigned char), size, file_ptr);
  fclose(file_ptr);

  if (NULL == (dst = image_create(height, width))) {
    image_destroy(&src);
    return;
  }
	if (Image_Success != image_he(dst, src)) {
		image_destroy(&src);
    image_destroy(&dst);
		return;
	}
	file_ptr = fopen(new_photo_path, "w");
	fwrite(dst->data, sizeof(unsigned char), size, file_ptr);
	fclose(file_ptr);
  image_destroy(&src);
  image_destroy(&dst);
	return;
}



/* image_convolution Function */

int test_image_convolution(char *test_name) {
  const size_t height = 5, width = 5;
  image *src = NULL, *dst = NULL;

  unsigned char image_values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25 };
  unsigned char image_after_convolution[] = { 6, 6, 8, 9, 9, 6, 6, 8, 9, 9, 12, 12, 13, 14, 14, 17, 17, 18, 19, 19, 17, 17, 18, 19, 19 };
  double kernel[] = { 1.0/9, 1.0/9, 1.0/9, 1.0/9, 1.0/9, 1.0/9, 1.0/9, 1.0/9, 1.0/9 };

  strcpy(test_name, "test_image_convolution");
  if (NULL == (src = image_create(height, width))) {
    return 0;
  }
  if (NULL == (dst = image_create(height, width))) {
    image_destroy(&src);
    return 0;
  }
  fill_image_values(src, image_values, sizeof(image_values));
  
  if(Image_Success != image_convolution(dst, src, kernel, 3)) {
    image_destroy(&src);
    image_destroy(&dst);
    return 0;
  }

  if (0 == compare_image_values(dst->data, image_after_convolution, dst->height * dst->width)) {
    image_destroy(&src);
    image_destroy(&dst);
    return 0;
  }
  image_destroy(&src);
  image_destroy(&dst);
  return 1;
}

int test_image_convolution_identity(char *test_name) {
  const size_t height = 8, width = 8;
  image *src = NULL, *dst = NULL;

  unsigned char image_values[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64 };
  unsigned char image_after_convolution[] = { 19, 19, 19, 20, 21, 22, 22, 22, 19, 19, 19, 20, 21, 22, 22, 22, 19, 19, 19, 20, 21, 22, 22, 22, 27, 27, 27, 28, 29, 30, 30, 30, 35, 35, 35, 36, 37, 38, 38, 38, 43, 43, 43, 44, 45, 46, 46, 46, 43, 43, 43, 44, 45, 46, 46, 46, 43, 43, 43, 44, 45, 46, 46, 46 };
  double kernel[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  strcpy(test_name, "test_image_convolution_identity");
  if (NULL == (src = image_create(height, width))) {
    return 0;
  }
  if (NULL == (dst = image_create(height, width))) {
    image_destroy(&src);
    return 0;
  }
  fill_image_values(src, image_values, sizeof(image_values));

  if(Image_Success != image_convolution(dst, src, kernel, 5)) {
    image_destroy(&src);
    image_destroy(&dst);
    return 0;
  }

  if (0 == compare_image_values(dst->data, image_after_convolution, dst->height * dst->width)) {
    image_destroy(&src);
    image_destroy(&dst);
    return 0;
  }
  image_destroy(&src);
  image_destroy(&dst);
  return 1;
}

int test_image_convolution_null(char *test_name) {
  const size_t height = 8, width = 8;
  image *img = NULL;
  double kernel[] = { 0, 0, 0, 0, 1, 0, 0, 0 ,0 };

  strcpy(test_name, "test_image_convolution_null");
  if (NULL == (img = image_create(height, width))) {
    return 0;
  }

  if(Image_Uninitialized_Error != image_convolution(NULL, img, kernel, 3)) {
    image_destroy(&img);
    return 0;
  }
  if(Image_Uninitialized_Error != image_convolution(img, NULL, kernel, 3)) {
    image_destroy(&img);
    return 0;
  }
  if(Image_Uninitialized_Error != image_convolution(img, img, NULL, 3)) {
    image_destroy(&img);
    return 0;
  }
  image_destroy(&img);
  return 1;
}

int test_image_convolution_kernel_size(char *test_name) {
  const size_t height = 8, width = 8;
  image *img = NULL;
  double kernel[] = { 0, 0, 0, 0, 1, 0, 0, 0 ,0 };

  strcpy(test_name, "test_image_convolution_kernel_size");
  if (NULL == (img = image_create(height, width))) {
    return 0;
  }

  if(Image_KernelSize_Error != image_convolution(img, img, kernel, 20)) {
    image_destroy(&img);
    return 0;
  }
  if(Image_KernelSize_Error != image_convolution(img, img, kernel, -5)) {
    image_destroy(&img);
    return 0;
  }
  image_destroy(&img);
  return 1;
}

int test_image_convolution_different_image_size(char *test_name) {
  const size_t height = 8, width = 8;
  image *src = NULL, *dst = NULL;
  double kernel[] = { 0, 0, 0, 0, 1, 0, 0, 0 ,0 };

  strcpy(test_name, "test_image_convolution_different_image_size");
  if (NULL == (src = image_create(height + 1, width))) {
    return 0;
  }
  if (NULL == (dst = image_create(height, width))) {
    image_destroy(&src);
    return 0;
  }

  if(Image_Size_Error != image_convolution(dst, src, kernel, 3)) {
    image_destroy(&src);
    image_destroy(&dst);
    return 0;
  }

  image_destroy(&src);
  image_destroy(&dst);
  return 1;
}

int test_image_convolution_image_size_zero(char *test_name) {
  const size_t width = 8;
  image *img = NULL;
  double kernel[] = { 0, 0, 0, 0, 1, 0, 0, 0 ,0 };

  strcpy(test_name, "test_image_convolution_image_size_zero");
  if (NULL == (img = image_create(0 , width))) {
    return 0;
  }

  if(Image_Size_Error != image_convolution(img, img, kernel, 3)) {
    image_destroy(&img);
    return 0;
  }

  image_destroy(&img);
  return 1;
}

void test_image_convolution_on_photo(char *test_name, const char* photo_path, const char* new_photo_path, size_t height, size_t width) {
	FILE *file_ptr = NULL;
	image *src = NULL, *dst = NULL;
	size_t i, size = height * width, kernel_size = 9;
  double *kernel = (double*)malloc(sizeof(double) * kernel_size * kernel_size);
  if (NULL == kernel) {
    return;
  }
  /* kernel - box blur */
  for (i = 0 ; i < kernel_size * kernel_size ; ++i) {
    kernel[i] = (double)1.0 / (kernel_size * kernel_size);
  }
	if (NULL == (src = image_create(height, width))) {
    return;
  }
	file_ptr = fopen(photo_path, "r");
	fread(src->data, sizeof(unsigned char), size, file_ptr);
  fclose(file_ptr);

  if (NULL == (dst = image_create(height, width))) {
    image_destroy(&src);
    return;
  }
	if (Image_Success != image_convolution(dst, src, kernel, kernel_size)) {
		image_destroy(&src);
    image_destroy(&dst);
		return;
	}
	file_ptr = fopen(new_photo_path, "w");
	fwrite(dst->data, sizeof(unsigned char), size, file_ptr);
	fclose(file_ptr);
  image_destroy(&src);
  image_destroy(&dst);
  free(kernel);
	return;
}


/* static function */

static image* image_random_create(unsigned int height, unsigned int width) {
  unsigned int i = 0, size = height * width;
  image* img = NULL;
  if (size == 0) {
    return NULL;
  }
  if (NULL == (img = image_create(height, width))) {
    return NULL;
  }
  for ( ; i < size ; ++i) {
    img->data[i] = (rand() % UCHAR_MAX);
  }
  return img;
}

static image* image_create(unsigned int height, unsigned int width) {
  image *img = NULL;
  if (NULL == (img = (image*)malloc(sizeof(image)))) {
    return NULL;
  }
  if (NULL == (img->data = (unsigned char*)calloc(height * width, sizeof(unsigned char)))) {
    free(img);
    return NULL;
  }
  img->height = height;
  img->width = width;
  return img;
}

static void image_destroy(image **img) {
  if (NULL == img || NULL == *img) {
    return;
  }
  free((*img)->data);
  free(*img);
  *img = NULL;
}

static void fill_image_values(image *img, const unsigned char *values, size_t size) {
  size_t i = 0;
  if (NULL == img || NULL == values) {
    return;
  }
  for ( ; i < size ; ++i) {
    img->data[i] = values[i];
  }
}

static int compare_image_values(const unsigned char *first, const unsigned char *second, size_t size) {
  size_t i = 0;
  for ( ; i < size ; ++i) {
    if (first[i] != second[i]) {
      return 0;
    }
  }
  return 1;
}

void print_image_data(const unsigned char *data, size_t height, size_t width) {
  size_t j, i = 0;
  for ( ; i < height ; ++i) {
    for (j = 0 ; j < width ; ++j) {
      printf("%3u ", data[i*width +j]);
    }
    putchar('\n');
  }
  putchar('\n');
}
