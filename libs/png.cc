#include "png.h"
#include <iostream>
#include <cstdio>
#include <png.h>

using namespace std;

PNG::PNG() {
    w = h = 0;
}

PNG::PNG(int width, int height, vector<float> &pixels_image, bool rotated_pixels) {
    w = width;
    h = height;

    if (rotated_pixels) {
        vector<float> finals_pixels_image(pixels_image.size());
        for (int i = h - 1; i >= 0; --i) {
            for (int j = w - 1; j >= 0; --j) {
                float pixel_0 = pixels_image[i * w * 3 + j * 3 + 0];
                float pixel_1 = pixels_image[i * w * 3 + j * 3 + 1];
                float pixel_2 = pixels_image[i * w * 3 + j * 3 + 2];

                finals_pixels_image[(h - i - 1) * w * 3 + j * 3 + 0] = pixel_0;
                finals_pixels_image[(h - i - 1) * w * 3 + j * 3 + 1] = pixel_1;
                finals_pixels_image[(h - i - 1) * w * 3 + j * 3 + 2] = pixel_2;
            }
        }
        px = finals_pixels_image;
    } else
        px = pixels_image;
}

PNG::PNG(const string &name) {
    load(name);
}

void PNG::load(const string &name) {
    w = h = 0;

    FILE *f = fopen(name.c_str(), "rb");
    if (!f)
        return;

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!png) {
        fclose(f);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, 0, 0);
        fclose(f);
        return;
    }

    png_init_io(png, f);
    png_read_info(png, info);

    w = png_get_image_width(png, info);
    h = png_get_image_height(png, info);

    uint32_t mode = png_get_color_type(png, info);
    uint32_t bits = png_get_bit_depth(png, info);

    switch (mode) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(png);
            break;
        case PNG_COLOR_TYPE_GRAY:
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            if (bits < 8)
                png_set_expand_gray_1_2_4_to_8(png);
            png_set_gray_to_rgb(png);
            break;
    }
    if (bits == 16)
#if PNG_LIBPNG_VER >= 10504
        png_set_scale_16(png);
#else
    png_set_strip_16(png);
#endif
    if (bits < 8)
        png_set_packing(png);
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    png_read_update_info(png, info);

    uint32_t chns = png_get_channels(png, info);
    uint32_t rb = png_get_rowbytes(png, info);

    px.resize(3 * w * h);

    png_byte *buffer = new png_byte[rb];
    const float ik = 1.0 / 255.0;
    for (int row = 0; row < h; row++) {
        png_read_row(png, buffer, 0);
        png_byte *src = buffer;
        float *dst = &px[3 * row * w];
        for (int col = 0; col < w; col++) {
            dst[0] = ik * src[0];
            dst[1] = ik * src[1];
            dst[2] = ik * src[2];
            src += chns;
            dst += 3;
        }
    }
    png_read_end(png, 0);

    png_destroy_read_struct(&png, &info, 0);
    fclose(f);
}

void PNG::save(const string &name) const {
    /* create file */
    FILE *fp = fopen(name.c_str(), "wb");
    if (!fp) return;

    /* initialize stuff */
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr) return;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) return;

    if (setjmp(png_jmpbuf(png_ptr))) return;

    png_init_io(png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(png_ptr))) return;

    png_set_IHDR(png_ptr, info_ptr, w, h,
                 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);


    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr))) return;

    png_bytep row = (png_bytep) malloc(3 * w * sizeof(png_byte));
    for (int i = 0; i < h; i++) {
        for (int d = 0; d < w * 3; d++) {
            float pixel = px[i * w * 3 + d];
            row[d] = (pixel < 0.0f) ? 0.0f : ((pixel > 1.0f) ? 255.0f : (255.0f * pixel));
        }
        png_write_row(png_ptr, row);
    }
    free(row);


    /* end write */
    if (setjmp(png_jmpbuf(png_ptr))) return;

    png_write_end(png_ptr, NULL);

    fclose(fp);
}