
#include "Halide.h"

// Include some support code for loading pngs.
#include "halide_image_io.h"
using namespace Halide::Tools;
#include "clock.h"
#include<iostream>
#include <string>
using namespace std;

void brightManual(int factor) {
    Halide::Buffer<uint8_t> input = load_image("futurama.jpg");
    int h = input.height();
    int w = input.width();
    int c = input.channels();
    Halide::Buffer<uint8_t> output(w, h, c);
    uint8_t* inbuf = input.get()->data();
    uint8_t* outbuf = output.get()->data();


    for (int j = 0;j < h;j++) {
        for (int k = 0;k < w;k++) {

            *(outbuf + 3 * j * w + 3 * k) = std::clamp(*(inbuf + 3 * j * w + 3 * k) + factor, 0, 255);
            *(outbuf + 3 * j * w + 3 * k + 1) = std::clamp(*(inbuf + 3 * j * w + 3 * k + 1) + factor, 0, 255);
            *(outbuf + 3 * j * w + 3 * k + 2) = std::clamp(*(inbuf + 3 * j * w + 3 * k + 2) + factor, 0, 255);
            // *(outbuf + j*w + k) = std::clamp(*(inbuf + j*w + k)+factor,0,255);
            // *(outbuf + (h*w)+ j*w + k ) =  std::clamp(*(inbuf + (h*w)+ j*w + k)+factor,0,255);
            // *(outbuf + (2*h*w) + j*w + k) =  std::clamp(*(inbuf + (2*h*w) + j*w + k)+factor,0,255);

        }
    }

    string filename = to_string(factor);
    filename += "_brighterManual.png";

    cout << filename << endl;


    save_image(output, filename);
    //save_image(output, "manual.jpg");

};

void brightHalide(int factor) {
    Halide::Buffer<uint8_t> input = load_image("futurama.jpg");
    Halide::Func brighter;
    Halide::Var x, y, c;

    Halide::Expr value = input(x, y, c);

    value = Halide::cast<int>(value);


    value = value + factor;
    value = Halide::min(value, 255.0f);

    value = Halide::cast<uint8_t>(value);

    brighter(x, y, c) = value;
    Halide::Buffer<uint8_t> output =
        brighter.realize({ input.width(), input.height(), input.channels() });

    string filename = to_string(factor);
    filename += "_brighterHalide.png";

    cout << filename << endl;


    save_image(output, filename);
    //save_image(output,"brighterHalide.png");


};
void Image_blur_3x3(void) {
    Halide::Buffer<uint8_t> input = load_image("futurama.jpg");
    //Halide::Func blur_x;
    //Halide::Var x, y, c;
    
    Halide::Func blur_x, blur_y;
    Halide::Var x, y, xi, yi;

    // The algorithm - no storage or order
    blur_x(x, y) = (input(x - 1, y) + input(x, y) + input(x + 1, y)) / 3;
    blur_y(x, y) = (blur_x(x, y - 1) + blur_x(x, y) + blur_x(x, y + 1)) / 3;

    // The schedule - defines order, locality; implies storage
    blur_y.tile(x, y, xi, yi, 256, 32).vectorize(xi, 8).parallel(y);
    blur_x.compute_at(blur_y, x).vectorize(x, 8);
    //Halide::Buffer<uint8_t> output = blur_y;
    
    save_image(blur_y,"Blur_Halide_01.png");


    //return blur_y;
   // return 0;
}



int main(int argc, char** argv) {

    // This program defines a single-stage imaging pipeline that
    // brightens an image.
    double t1 = current_time();
 //   brightManual(-121);
    double t2 = current_time();
 //   std::cout << "Image Blur Manual Time: " << (t2 - t1) << "\n";
    double t3 = current_time();
    brightHalide(160);
    double t4 = current_time();
    std::cout << "Image Blur Halide Time: " << (t2 - t1) << "\n";
    //Halide::Buffer<uint8_t> input = load_image("futurama.jpg");
    Image_blur_3x3();

    printf("Image Blur Success 2022!\n");
    return 0;

}