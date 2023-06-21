// Halide tutorial lesson 00: Load images

// This lesson demonstrates how to Load an input images and save out without manipulate them.

// On linux, you can compile and run it like so:
// g++ lesson_02*.cpp -g -I <path/to/Halide.h> -I <path/to/tools/halide_image_io.h> -L <path/to/libHalide.so> -lHalide `libpng-config --cflags --ldflags` -ljpeg -lpthread -ldl -o lesson_02 -std=c++17
// LD_LIBRARY_PATH=<path/to/libHalide.so> ./lesson_02

// On os x:
// g++ lesson_02*.cpp -g -I <path/to/Halide.h> -I <path/to/tools/halide_image_io.h> -L <path/to/libHalide.so> -lHalide `libpng-config --cflags --ldflags` -ljpeg -o lesson_02 -std=c++17
// DYLD_LIBRARY_PATH=<path/to/libHalide.dylib> ./lesson_02

// If you have the entire Halide source tree, you can also build it by
// running:
//    make tutorial_lesson_02_input_image
// in a shell with the current directory at the top of the halide
// source tree.

// The only Halide header file you need is Halide.h. It includes all of Halide.
#include "Halide.h"

#include "HalideRuntime.h"

// Include some support code for loading pngs.
#include "halide_image_io.h"
#include <stdio.h>
using namespace Halide;
using namespace Halide::Tools;
using namespace Halide::Runtime;

int main(int argc, char** argv) {

    // This program defines a Load image and Save

    // First we'll load the input image we wish to brighten.
    Halide::Buffer<uint8_t> input_1 = load_image(".//Images//gray.jpg");



      // Next we define our Func object that represents our one pipeline
      // stage.
    Halide::Func InputOutput;

    // Our Func will have three arguments, representing the position
    // in the image and the color channel. Halide treats color
    // channels as an extra dimension of the image.
    Halide::Var x, y, c;

    // Normally we'd probably write the whole function definition on
    // one line. Here we'll break it apart so we can explain what
    // we're doing at every step.

    // For each pixel of the input image.
    Halide::Expr value_1 = input_1(x, y, c);
    // Cast it to a floating point value.
    //value_1 = Halide::cast<float>(value_1);
    // Multiply it by 1.5 to brighten it. Halide represents real

    //value_1 = value_1;

    // Clamp it to be less than 255, so we don't get overflow when we
    // cast it back to an 8-bit unsigned int.
   // value_1 = Halide::min(value_1, 255.0f);

    // Cast it back to an 8-bit unsigned integer.
   // value_1 = Halide::cast<uint8_t>(value_1);

    // Define the function.
    InputOutput(x, y, c) = value_1;

    Halide::Buffer<uint8_t> output_1 =
        InputOutput.realize({ input_1.width(), input_1.height(), input_1.channels() });

    // Save the output for inspection. It should look like a bright parrot.
    save_image(output_1, ".//Images//Output//InputOutput_01.png");


    // See figures/lesson_02_output.jpg for a small version of the output.

    printf("Halide - Aula 01 - Success - Func = InputOutput 01!\n");
    return 0;
}
