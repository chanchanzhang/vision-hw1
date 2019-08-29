#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include "image.h"

#define TWOPI 6.2831853

void l1_normalize(image im)
{
    // TODO
    float sum = 0.0;
    for (int i = 0; i < im.c * im.h * im.w; i++) {
        sum += im.data[i];
    }
    for (int i = 0; i < im.c * im.h * im.w; i++) {
        im.data[i] /= sum;
    }
}

image make_box_filter(int w)
{
    // TODO
    image im = make_image(w, w, 1);
    for (int i = 0; i < w * w; i ++) {
        im.data[i] = 1.0 / ((float)w * w);
    }
    return im;
}

image convolve_image(image im, image filter, int preserve)
{
    // TODO
    assert(im.c == filter.c || filter.c == 1);
    image conv_im;
    if (preserve == 1)
        conv_im = make_image(im.w, im.h, im.c);
    else
        conv_im = make_image(im.w, im.h, 1);
    for (int ch = 0; ch < im.c; ch++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                float sum = 0;
                for (int fy = 0; fy < filter.h; fy++) {
                    for (int fx = 0; fx < filter.w; fx++) {
                        int fc = 0;
                        if (im.c == filter.c) {
                            fc = ch;
                        }
                        sum += filter.data[fc * filter.h * filter.w + fy * filter.w + fx] *
                            get_pixel(im, x + fx - (filter.w / 2), y + fy - (filter.h / 2), ch);
                    }
                }
                if (preserve == 1)
                    conv_im.data[ch * conv_im.h * conv_im.w + y * conv_im.w + x] = sum;
                else
                    conv_im.data[y * conv_im.w + x] += sum;
            }
        }
    }
    // for (int y = 0; y < conv_im.h; y++) {
    //     for (int x = 0; x < conv_im.w; x++) {
    //         printf("conv_im: x %d, y %d: %f\n", x, y, get_pixel(conv_im, x, y, 1));
    //     }
    // }
    return conv_im;
}

image make_highpass_filter()
{
    // TODO
    image im = make_image(3, 3, 1);
    float filter[] = { 0, -1, 0, -1, 4, -1, 0, -1, 0};
    memcpy(im.data, filter, sizeof(filter));
    return im;
}

image make_sharpen_filter()
{
    // TODO
    image im = make_image(3, 3, 1);
    float filter[] = { 0, -1, 0, -1, 5, -1, 0, -1, 0};
    memcpy(im.data, filter, sizeof(filter));
    return im;
}


image make_emboss_filter()
{
    // TODO
    image im = make_image(3, 3, 1);
    float filter[] = { -2, -1, 0, -1, 1, 1, 0, 1, 2};
    memcpy(im.data, filter, sizeof(filter));
    return im;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: highpass - no preserve, just want to get edge, others preserve

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: Clamp

image make_gaussian_filter(float sigma)
{
    // TODO
    int fk = 6 * sigma;
    int k = fk % 2 ? fk : (fk - 1);
    if (k < 1)
        k = 1;
    printf("%s: k = %d\n", __FUNCTION__, k);
    image im = make_image(k, k, 1);
    double sum = 0;
    for (int y = 0; y < k; y++) {
        for (int x = 0; x < k; x++) {
            double r = (x - k / 2) * (x - k / 2)
                + (y - k / 2) * (y - k / 2);
            double d = exp(-(r) / 2.0 / sigma /sigma) / TWOPI / sigma / sigma;
            im.data[y * k + x] = d;
            sum += d;
        }
    }
    for (int i = 0; i < k * k; i++) {
        im.data[i] /= sum;
    }
    return im;
}

image add_image(image a, image b)
{
    // TODO
    assert(a.c == b.c && a.w == b.w && a.h == b.h);
    image im = make_image(a.w, a.h, a.c);
    for (int i = 0; i < a.c * a.h * a.w; i++) {
        im.data[i] = a.data[i] + b.data[i];
    }
    return im;
}

image sub_image(image a, image b)
{
    assert(a.c == b.c && a.w == b.w && a.h == b.h);
    image im = make_image(a.w, a.h, a.c);
    for (int i = 0; i < a.c * a.h * a.w; i++) {
        im.data[i] = a.data[i] - b.data[i];
    }
    return im;
}

image make_gx_filter()
{
    image im = make_image(3, 3, 1);
    float filter[] = { -1, 0, 1, -2, 0, 2, -1, 0, 1};
    memcpy(im.data, filter, sizeof(filter));
    return im;
}

image make_gy_filter()
{
    image im = make_image(3, 3, 1);
    float filter[] = { -1, -2, -1, 0, 0, 0, 1, 2, 1};
    memcpy(im.data, filter, sizeof(filter));
    return im;
}

void feature_normalize(image im)
{
    float min = FLT_MAX, max =FLT_MIN;
    for (int i = 0; i < im.c * im.h * im.w; i++) {
        if (im.data[i] < min)
            min = im.data[i];
        if (im.data[i] > max)
            max = im.data[i];
    }
    for (int i = 0; i < im.c * im.h * im.w; i++) {
        if ((max - min) == 0)
            im.data[i] = 0;
        im.data[i] = (im.data[i] - min) / (max - min);
    }
}

image *sobel_image(image im)
{
    image gx_filter = make_gx_filter();
    image gy_filter = make_gy_filter();

    image gx = convolve_image(im, gx_filter, 0);
    image gy = convolve_image(im, gy_filter, 0);

    image magnitude = make_image(gx.w, gx.h, gx.c);
    image direction = make_image(gx.w, gx.h, gx.c);
    for (int i = 0; i < gx.c * gx.h * gx.w; i++) {
        magnitude.data[i] = sqrt(gx.data[i] * gx.data[i] + gy.data[i] * gy.data[i]);
        double rad = 0;
        if (gx.data[i] == 0)
            rad = 0;
        else
            rad = gy.data[i] / gx.data[i];
        direction.data[i] = atan(rad);
    }
    image *pimage = calloc(2, sizeof(image));
    memcpy(pimage, &magnitude, sizeof(magnitude));
    memcpy(pimage + 1, &direction, sizeof(direction));
    return pimage;
}

image colorize_sobel(image im)
{
    image *pimage = sobel_image(im);
    image magnitude = pimage[0];
    feature_normalize(magnitude);
    image direction = pimage[1];
    // clamp_image(direction);
    image color = make_image(im.w, im.h, 3);
    for (int i = 0; i < im.h * im.w; i++) {
        // hue
        color.data[0 * im.w * im.h + i] = direction.data[i];
        // saturation
        color.data[1 * im.w * im.h + i] = magnitude.data[i];
        // value
        color.data[2 * im.w * im.h + i] = magnitude.data[i];
    }
    free_image(magnitude);
    free_image(direction);
    hsv_to_rgb(color);
    return color;
}
