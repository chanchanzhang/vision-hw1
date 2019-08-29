#include <math.h>
#include "image.h"

int round1(float x) {
	if ((x - (int)x) <= 0.5)
		return (int)x;
	else
		return (int)x + 1;
}
float nn_interpolate(image im, float x, float y, int c)
{
    // TODO Fill in
    return get_pixel(im, round(x), round(y), c);
}

image nn_resize(image im, int w, int h)
{
    // TODO Fill in (also fix that first line)
    image resize = make_image(w, h, im.c);
    for (int y = 0; y < h; y++) {
    	for (int x = 0; x < w; x++) {
    		for (int ch = 0; ch < im.c; ch++) {
    			resize.data[ch * h * w + y * w + x] = 
    				nn_interpolate(im, (float)x * (float)im.w / (float)w , (float)y * (float)im.h / (float)h, ch);
    		}
    	}
    }
    return resize;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    // TODO
   	float v1 = get_pixel(im, (int)x, (int)y, c);
   	float v2 = get_pixel(im, (int)x + 1, (int)y, c);
   	float v3 = get_pixel(im, (int)x, (int)y + 1, c);
   	float v4 = get_pixel(im, (int)x + 1, (int)y + 1, c);
   	float d1 = x - (int)x;
   	float d2 = 1.0 - d1;
   	float d3 = y - (int)y;
   	float d4 = 1.0 - d3;
   	float q1 = v1 * d2 + v2 * d1;
   	float q2 = v3 * d2 + v4 * d1;
   	return q1 * d4 + q2 * d3;
}

image bilinear_resize(image im, int w, int h)
{
    // TODO
    image resize = make_image(w, h, im.c);
    for (int y = 0; y < h; y++) {
    	for (int x = 0; x < w; x++) {
    		for (int ch = 0; ch < im.c; ch++) {
    			resize.data[ch * h * w + y * w + x] = 
    				bilinear_interpolate(im, (float)x * (float)im.w / (float)w , (float)y * (float)im.h / (float)h, ch);
    		}
    	}
    }
    return resize;
}

