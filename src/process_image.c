#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"
#define EPS1 .00000001

int within_eps1(float a, float b){
    return a-EPS1<b && b<a+EPS1;
}

#define CLAMP(x, min, max) (x < min? min : (x > max? max: x))
float get_pixel(image im, int x, int y, int c)
{
    // TODO Fill this in
    c = CLAMP(c, 0, im.c - 1);
    y = CLAMP(y, 0, im.h - 1);
    x = CLAMP(x, 0, im.w - 1);
    return im.data[c * im.h * im.w + y * im.w + x];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    if (c > im.c || c < 0 || y > im.h || y < 0 || x > im.w || x < 0) {
        printf("%s: invalid args\n", __FUNCTION__);
        return;
    }
    im.data[c * im.h * im.w + y * im.w + x] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    // TODO Fill this in
    memcpy(copy.data, im.data, sizeof(float) * im.w * im.h * im.c);
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    // TODO Fill this in
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            gray.data[y * gray.w + x] = 0.299 * im.data[y * im.w + x] +
                0.587 * im.data[im.w * im.h + y * im.w + x] +
                0.114 * im.data[im.w * im.h * 2 + y * im.w + x];
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    // TODO Fill this in
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            im.data[c * im.h * im.w + y * im.w + x] += v;
        }
    }
}

void clamp_image(image im)
{
    // TODO Fill this in
    for (int i = 0; i < im.c; i++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                im.data[i * im.h * im.w + y * im.w + x] = CLAMP(im.data[i * im.h * im.w + y * im.w + x], 0, 1);
            }
        }
    }
}

void dump_image(image im, char * name, float min, float max)
{
    // TODO Fill this in
    for (int i = 0; i < im.c; i++) {
        for (int y = 0; y < im.h; y++) {
            for (int x = 0; x < im.w; x++) {
                float d = im.data[i * im.h * im.w + y * im.w + x];
                if (d >= min && d <= max)
                    printf("%s ch %d, y %d, x %d: %f\n", name, i, y, x, d);
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    // TODO Fill this in
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float R = im.data[0 * im.h * im.w + y * im.w + x];
            float G = im.data[1 * im.h * im.w + y * im.w + x];
            float B = im.data[2 * im.h * im.w + y * im.w + x];
            float V = three_way_max(R, G, B);
            float m = three_way_min(R, G, B);
            float C = V - m;
            float S = 0;
            if (V != 0.0f)
                S = C / V;
            // now calculate H
            float Hh = 0;
            if (C != 0.0f) {
                if (V == R) {
                    Hh = (G - B) / C;
                } else if (V == G) {
                    Hh = (B - R) / C + 2.0f;
                } else {
                    Hh = (R - G) / C + 4.0f;
                }
            }
            float H;
            if (Hh < 0) {
                H = Hh / 6.0f + 1.0f;
            } else {
                H = Hh / 6.0f;
            }
            im.data[0 * im.h * im.w + y * im.w + x] = H;
            im.data[1 * im.h * im.w + y * im.w + x] = S;
            im.data[2 * im.h * im.w + y * im.w + x] = V;
            // if (y == 8 && x == 88)
            //     printf("%s: R %f, G %f, B %f, Hh %f, H %f, S %f, V %f, m %f, C %f\n", __FUNCTION__, R, G, B, Hh, H, S, V, m, C);
        }
    }
}

float fabs1(float x) {
    return x>0? x: -x;
}

float fmod1(float a, int b) {
    return (int)a % b + a - (int)a;
}
void hsv_to_rgb(image im)
{
    // TODO Fill this in
    for (int y = 0; y < im.h; y++) {
        for (int x = 0; x < im.w; x++) {
            float H = im.data[0 * im.h * im.w + y * im.w + x];
            float S = im.data[1 * im.h * im.w + y * im.w + x];
            float V = im.data[2 * im.h * im.w + y * im.w + x];
            float Hh = H * 6;
            float C = V * S;
            float m = V - C;
            float X1 = (1 - fabs1(fmod1(Hh, 2) - 1.0));
            float X = C * X1;
            // printf("%s: X %f\n", __FUNCTION__, X);
            float R1 = 0, G1 = 0, B1 = 0;
            if (Hh <= 1) {
                R1 = C;
                G1 = X;
                B1 = 0;
            } else if (Hh <= 2) {
                R1 = X;
                G1 = C;
                B1 = 0;
            } else if (Hh <= 3) {
                R1 = 0;
                G1 = C;
                B1 = X;
            } else if (Hh <= 4) {
                R1 = 0;
                G1 = X;
                B1 = C;
            } else if (Hh <= 5) {
                R1 = X;
                G1 = 0;
                B1 = C;
            } else if (Hh <= 6) {
                R1 = C;
                G1 = 0;
                B1 = X;
            }

            // if (y == 8 && x == 88)
            //     printf("%s: R %f, G %f, B %f, Hh %f, H %f, S %f, V %f, m %f, C %f\n", __FUNCTION__, R1, G1, B1, Hh, H, S, V, m, C);
            im.data[0 * im.h * im.w + y * im.w + x] = R1 + m;
            im.data[1 * im.h * im.w + y * im.w + x] = G1 + m;
            im.data[2 * im.h * im.w + y * im.w + x] = B1 + m;
        }
    }
}

void scale_image(image im, int c, float v) {
    if (!im.data || !im.h || !im.w)
        return;
    for (int i = c * im.h * im.w, count = 0; count < im.h * im.w; count++) {
        im.data[i + count] *= v;
    }
}