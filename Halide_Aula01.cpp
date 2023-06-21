// Halide tutorial lesson 2: Processing images

// This lesson demonstrates how to pass in input images and manipulate
// them.

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

    // This program defines a single-stage imaging pipeline that
    // brightens an image.

    // First we'll load the input image we wish to brighten.
    Halide::Buffer<uint8_t> input_1 = load_image(".//Images//brighter.png");
    Halide::Buffer<uint8_t> input_2 = load_image(".//Images//gray.jpg");
    //  Halide::Buffer<uint8_t> input = load_image("images/rgb.png");

     // Buffer<uint8_t> left_im = load_image();


      // See figures/lesson_02_input.jpg for a smaller version.

      // Next we define our Func object that represents our one pipeline
      // stage.
    Halide::Func brighter_1;
    Halide::Func brighter_2;

    // Our Func will have three arguments, representing the position
    // in the image and the color channel. Halide treats color
    // channels as an extra dimension of the image.
    Halide::Var x, y, c;

    // Normally we'd probably write the whole function definition on
    // one line. Here we'll break it apart so we can explain what
    // we're doing at every step.

    // For each pixel of the input image.
    Halide::Expr value_1 = input_1(x, y, c);
    Halide::Expr value_2 = input_2(x, y, c);
    // Cast it to a floating point value.
    value_1 = Halide::cast<float>(value_1);
    value_2 = Halide::cast<float>(value_2);
    // Multiply it by 1.5 to brighten it. Halide represents real
    // numbers as floats, not doubles, so we stick an 'f' on the end
    // of our constant.
    value_1 = value_1 * 1.5f;
    value_2 = value_2 * -1.5f;

    // Clamp it to be less than 255, so we don't get overflow when we
    // cast it back to an 8-bit unsigned int.
    value_1 = Halide::min(value_1, 255.0f);
    value_2 = Halide::min(value_2, 255.0f);

    // Cast it back to an 8-bit unsigned integer.
    value_1 = Halide::cast<uint8_t>(value_1);
    value_2 = Halide::cast<uint8_t>(value_2);
    // Define the function.
    brighter_1(x, y, c) = value_1;
    brighter_2(x, y, c) = value_2;

    // The equivalent one-liner to all of the above is:
    //
    // brighter(x, y, c) = Halide::cast<uint8_t>(min(input(x, y, c) * 1.5f, 255));
    //
    // In the shorter version:
    // - I skipped the cast to float, because multiplying by 1.5f does
    //   that automatically.
    // - I also used an integer constant as the second argument in the
    //   call to min, because it gets cast to float to be compatible
    //   with the first argument.
    // - I left the Halide:: off the call to min. It's unnecessary due
    //   to Koenig lookup.

    // Remember, all we've done so far is build a representation of a
    // Halide program in memory. We haven't actually processed any
    // pixels yet. We haven't even compiled that Halide program yet.

    // So now we'll realize the Func. The size of the output image
    // should match the size of the input image. If we just wanted to
    // brighten a portion of the input image we could request a
    // smaller size. If we request a larger size Halide will throw an
    // error at runtime telling us we're trying to read out of bounds
    // on the input image.
    Halide::Buffer<uint8_t> output_1 =
        brighter_1.realize({ input_1.width(), input_1.height(), input_1.channels() });
    Halide::Buffer<uint8_t> output_2 =
        brighter_2.realize({ input_2.width(), input_2.height(), input_2.channels() });

    // Save the output for inspection. It should look like a bright parrot.
    save_image(output_1, ".//Images//Output//brighter_1-02.png");
    save_image(output_2, ".//Images//Output//brighter_2-0111.png");

    // See figures/lesson_02_output.jpg for a small version of the output.

    printf("Halide - Aula 01 - Success - Func = brighter_1!\n");
    return 0;
}
