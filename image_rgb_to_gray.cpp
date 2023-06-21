//----------------------------------------------------------------------
#include <Halide.h>
#include <cstdio>

using namespace Halide;
//#include "image_io.h"

// Include some support code for loading pngs.
#include "halide_image_io.h"
using namespace Halide::Tools;

#include <time.h>

//----------------------------------------------------------------------
// Timer
double currentTime() {
    timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000.0 + t.tv_usec / 1000.0f;
}


//----------------------------------------------------------------------
// Merge RGB channels to one gray channel.
// This also demonstrates how to load/save png images.
void image_rgb_to_gray() {
    Func gray, gray2;
    Var x, y, c;

    Image<float> image = load<float>("rgb.png");

    const int w = image.width();
    const int h = image.height();
    const int ch = image.channels();

    //ALGORITHM (the same for gray and gray2)
    gray(x, y, c) = 0.299f * image(x, y, 0) + 0.587f * image(x, y, 1) + 0.114f * image(x, y, 2);

    gray2(x, y, c) = gray(x, y, c);

    Image<float> out(w, h, 1);  //the 3rd param is needed to output png.

    //SCHEDULE
    Var xi, yi;
    gray2.tile(x, y, xi, yi, 4, 4).vectorize(xi, 4).parallel(y);

    double t1 = benchmark(gray, out, 10, "image_rgb_to_gray");
    double t2 = benchmark(gray2, out, 10, "image_rgb_to_gray, par");
    printf("%.1fx speedup.\n", t1 / t2);

    save(out, "out_image_rgb_to_gray.png");
}
//----------------------------------------------------------------------
//Point-wise operation on a generated image with a known fixed size.
void pointwise() {
    Func input, input2;
    Var x, y, c;
    Var xi, yi;

    //Prepare fixed-size images
    const int w = 1024;
    const int h = 1024;
    Image<uint16_t> im(w, h, 1);

    //ALGORITHMS (the same for input and input2)
    input(x, y, c) = Halide::cast<uint16_t>(min(65535 * (x + y) / (w + h), 65535));
    input2(x, y, c) = input(x, y, c);

    // SCHEDULES
    input2.tile(x, y, xi, yi, 4, 4).vectorize(xi, 4).parallel(y);

    double t1 = benchmark(input, im, 1000, "pointwise");
    double t2 = benchmark(input2, im, 1000, "pointwise,parallel");
    printf("%.1fx speedup.\n", t1 / t2);

    save(im, "out_pointwise.png");
}

//----------------------------------------------------------------------
//3x3 mean filter. A simple example of stencil opretaions.
void blur() {
    Func gray, clamped, blur_x, blur_x2, blur, blur2;
    Var x, y, c;

    Image<float> image = load<float>("rgb.png");

    const int w = image.width();
    const int h = image.height();
    const int ch = image.channels();

    //ALGORITHM (the same for blur and blur2)

    //First, image is converted to grayscale. (the same as image_rgb_to_gray() above.)
    gray(x, y, c) = 0.299f * image(x, y, 0) + 0.587f * image(x, y, 1) + 0.114f * image(x, y, 2);

    Image<float> grayimage(w, h, 1);
    //grayimage is calculated immediately(?).
    gray.realize(grayimage);

    //Image has to be clamped before a stencil operation.
    clamped(x, y, c) = grayimage(clamp(x, 1, w - 1), clamp(y, 1, h - 1), c);

    blur_x(x, y, c) = 0.333f * clamped(x - 1, y, c) + 0.333f * clamped(x, y, c) + 0.333f * clamped(x + 1, y, c);
    blur(x, y, c) = 0.333f * blur_x(x, y - 1, c) + 0.333f * blur_x(x, y, c) + 0.333f * blur_x(x, y + 1, c);

    blur2(x, y, c) = blur(x, y, c);

    // SCHEDULE
    Var xi, yi;
    blur2.tile(x, y, xi, yi, 128, 32).vectorize(yi, 8).parallel(x);

    Image<float> out(w, h, 1);  //the 3rd param is needed to output png.
    double t1 = benchmark(blur, out, 10, "blur");
    double t2 = benchmark(blur2, out, 10, "blur,par");
    printf("%.1fx speedup.\n", t1 / t2);

    save(out, "out_blur.png");
}




//----------------------------------------------------------------------
// 
//Utility function for benchmark.
template <typename T>
//double benchmark(Func& input, Image<T>& im, const int repeat = 100, const char* label = "") {
double benchmark(Func & input, Buffer<uint8_t> im, const int repeat = 100, const char* label = "") {
    //    printf("Image: %d x %d x %d\n",im.width(),im.height(),im.channels());

        //Func is realized first, so that JIT is done before time measurement.
    input.realize(im);

    double t1 = currentTime();
    for (int i = 0; i < repeat; i++) {
        input.realize(im);
    }
    double t2 = currentTime();

    double mean = (t2 - t1) / repeat;
    printf("%.3f ms (%d times average): %s\n", mean, repeat, label);
    return mean;
}

//----------------------------------------------------------------------
// Run examples.
// All examples are independent from each other.
int main() {
    pointwise();
    image_rgb_to_gray();
    blur();
    return 0;
}