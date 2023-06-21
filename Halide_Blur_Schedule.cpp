#include "Halide.h"
#include "halide_image_io.h"
#include<iostream>
#include <chrono>
#include <numeric>

#include <iomanip>
using namespace std;
using namespace Halide;
using namespace std::chrono;
using namespace Halide::Tools;

void Exec_Timer(clock_t Start, clock_t End) {
    std::cout << std::fixed << std::setprecision(9) << std::left;

    double num1 = 3.12345678;
    cout << fixed << showpoint;
    cout << setprecision(12);
    cout << "Timer:";
    cout << num1;
    cout << " miliseconds!!" << endl;
    printf("max_Time: %0.8f sec \n",
        ((float)End - Start) / CLOCKS_PER_SEC);

}

void brightOriginal(int factor) {
    clock_t start1, end1;
    std::cout << std::fixed << std::setprecision(9) << std::left;



    Halide::Buffer<uint8_t> input = load_image("futurama.jpg");

    //Start timing
    start1 = clock();
    auto start = high_resolution_clock::now();
    Halide::Func brighter;
    Halide::Var x("x"), y("y"), c("c");
    Halide::Expr value = input(x, y, c);
    value = Halide::cast<int>(value);
    value = value + factor;
    value = Halide::min(value, 255.0f);
    value = Halide::cast<uint8_t>(value);
    brighter(x, y, c) = value;
    Halide::Buffer<uint8_t> output =
        brighter.realize({ input.width(), input.height(), input.channels() });

    //Stop timing
    end1 = clock();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Print Timer
    Exec_Timer(start1, end1);

    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Original " << duration.count() << " :microseconds" << std::endl;
    brighter.print_loop_nest();

    std::cout << "-----------------------------------------" << std::endl;
};

void brightReorder(int factor) {
    clock_t start1, end1;

    Halide::Buffer<uint8_t> input = load_image("futurama.jpg");
    //Start timing
    start1 = clock();
    auto start = high_resolution_clock::now();
    Halide::Func brighter;
    Halide::Var x("x"), y("y"), c("c");
    Halide::Expr value = input(x, y, c);
    value = Halide::cast<int>(value);
    value = value + factor;
    value = Halide::min(value, 255.0f);
    value = Halide::cast<uint8_t>(value);
    brighter(x, y, c) = value;

    // ----------- Begin ----------------
    brighter.reorder(c, x, y);
    // -----------End---------------------


    Halide::Buffer<uint8_t> output =
        brighter.realize({ input.width(), input.height(), input.channels() });

    //Stop timing
    end1 = clock();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Print Timer
    Exec_Timer(start1, end1);
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Reorder " << duration.count() << std::endl;
    brighter.print_loop_nest();

    std::cout << "-----------------------------------------" << std::endl;
};

void brightTiled(int factor) {
    clock_t start1, end1;

    Halide::Buffer<uint8_t> input = load_image("futurama.jpg");
    //Start timing
    start1 = clock();
    auto start = high_resolution_clock::now();
    Halide::Func brighter;
    Halide::Var x("x"), y("y"), c("c");

    Halide::Expr value = input(x, y, c);
    value = Halide::cast<int>(value);
    value = value + factor;
    value = Halide::min(value, 255.0f);

    value = Halide::cast<uint8_t>(value);
    Halide::Var x_outer("x_outer"), x_inner("x_inner"), y_outer("y_outer"), y_inner("y_inner");
    brighter(x, y, c) = value;

    // ----------- Begin ----------------
    brighter.tile(x, y, x_outer, y_outer, x_inner, y_inner, 4, 4);
    brighter.reorder(c, x_inner, y_inner, x_outer, y_outer);
    // -----------End---------------------

    Halide::Buffer<uint8_t> output =
        brighter.realize({ input.width(), input.height(), input.channels() });

    //Stop timing
    end1 = clock();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Print Timer
    Exec_Timer(start1, end1);
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Reorder " << duration.count() << std::endl;
    brighter.print_loop_nest();

    std::cout << "-----------------------------------------" << std::endl;
};

void brightVector(int factor) {
    clock_t start1, end1;

    Halide::Buffer<uint8_t> input = load_image("futurama.jpg");
    //Start timing
    start1 = clock();
    auto start = high_resolution_clock::now();
    Halide::Func brighter;
    Halide::Var x("x"), y("y"), c("c");
    Halide::Expr value = input(x, y, c);
    value = Halide::cast<int>(value);
    value = value + factor;
    value = Halide::min(value, 255.0f);
    value = Halide::cast<uint8_t>(value);
    Halide::Var x_outer("x_outer"), x_inner("x_inner"), y_outer("y_outer"), y_inner("y_inner");
    brighter(x, y, c) = value;
    Halide::Var x_vo("x_vo"), x_vi("x_vi");

    // ----------- Begin ----------------
    brighter.split(x, x_vo, x_vi, 4).vectorize(x_vi).parallel(c);
    // -----------End---------------------

    Halide::Buffer<uint8_t> output =
        brighter.realize({ input.width(), input.height(), input.channels() });


    //Stop timing
    end1 = clock();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Print Timer
    Exec_Timer(start1, end1);
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Vector " << duration.count() << std::endl;
    brighter.print_loop_nest();

    std::cout << "-----------------------------------------" << std::endl;
};

void brightParallel(int factor) {
    clock_t start1, end1;

    Halide::Buffer<uint8_t> input = load_image("brighter.png");
    //Start timing
    start1 = clock();
    auto start = high_resolution_clock::now();
    Halide::Func brighter;
    Halide::Var x("x"), y("y"), c("c");
    Halide::Expr value = input(x, y, c);
    value = Halide::cast<int>(value);
    value = value + factor;
    value = Halide::min(value, 255.0f);
    value = Halide::cast<uint8_t>(value);
    Halide::Var x_outer("x_outer"), x_inner("x_inner"), y_outer("y_outer"), y_inner("y_inner");
    brighter(x, y, c) = value;
    Halide::Var x_vo("x_vo"), x_vi("x_vi");

    // ----------- Begin ----------------
    brighter
        .reorder(x, c, y)
        .split(x, x_vo, x_vi, 32)
        .parallel(c)
        .vectorize(x_vi);

    // -----------End---------------------


    Halide::Buffer<uint8_t> output =
        brighter.realize({ input.width(), input.height(), input.channels() });


    //Stop timing
    end1 = clock();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Print Timer
    Exec_Timer(start1, end1);
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Parallel " << duration.count() << std::endl;
    brighter.print_loop_nest();

    std::cout << "-----------------------------------------" << std::endl;
};

//---------------------------------------------------------------
void blurOriginal(string image, int factor) {
    clock_t start1, end1;
    std::cout << std::fixed << std::setprecision(9) << std::left;

    Halide::Buffer<uint8_t> input = load_image(image);

    //Start timing
    start1 = clock();
    auto start = high_resolution_clock::now();

    // Algorithm halide_blur
    //------------------------------------
    //Halide::Var x("x"), y("y"), c("c");
   // Halide::Expr value = input(x, y, c);

    Halide::Func bh, bv;
    Halide::Var x, y, xi, yi;
    // The algorithm
    bh(x, y) = (input(x - 1, y) + input(x, y) + input(x + 1, y)) / 3;
    bv(x, y) = (bh(x, y - 1) + bh(x, y) + bh(x, y + 1)) / 3;
    // The schedule
    bv.tile(x, y, xi, yi, 256, 32)
        .vectorize(xi, 8).parallel(y);
    bh.compute_at(bv, x).vectorize(x, 8);

    //------------------------------------
 /*  Halide::Func brighter;
    Halide::Var x("x"), y("y"), c("c");
    Halide::Expr value = input(x, y, c);
    value = Halide::cast<int>(value);
    value = value + factor;
    value = Halide::min(value, 255.0f);
    value = Halide::cast<uint8_t>(value);
    

    // Scheduler
    brighter(x, y, c) = value;
    Halide::Buffer<uint8_t> output =
        brighter.realize({ input.width(), input.height(), input.channels() });
*/


    //Stop timing
    end1 = clock();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Print Timer
    Exec_Timer(start1, end1);

    std::cout << "-----------------------------------------" << std::endl;
    std::cout << "Original " << duration.count() << " :microseconds" << std::endl;
//    brighter.print_loop_nest();

    std::cout << "-----------------------------------------" << std::endl;
};

//---
void pipeline(void)
{

    Halide::Var x, y, c;
    Halide::Var dx, dy;

    Halide::Func f, g, w;
    Halide::Func w0;

    f(x, y, c) = 0.0f;
    g(x, y, c) = 1.0f;
    w0(x, y, c, dx, dy) = f(x + dx, y + dy, c) - g(dx, dy, c);


    Halide::RDom roi(0, 0, 0, 0);
    w(x, y, c) = 0.0f;
    w(x, y, c) += w0(x, y, c, roi.x, roi.y);


    Halide::Buffer<float> f_im(100, 100, 3);
    Halide::Buffer<float> g_im(100, 100, 3);
    Halide::Buffer<float> w_im(100, 100, 3);



    #define TEST_CASE 1

    // This will work
    #if TEST_CASE == 0
        Halide::Pipeline({f, g}).realize({f_im, g_im});
        std::cout << "Success: TEST_CASE == 0!\n";

        // This will work
     #elif TEST_CASE == 1
        //Halide::Pipeline({w}).realize({w_im});
        Halide::Pipeline({ f, g, w }).realize({ f_im, g_im, w_im });
        std::cout << "Success: TEST_CASE == 1!\n";

       // This will not
      #else:
        Halide::Pipeline({f, g, w}).realize({f_im, g_im, w_im});
      #endif


  std::cout << "Success!\n";
}

//--------------------------------------------------------------


void BLUR_01(string image) {
    // First we'll declare some Vars to use below.
    Var x("x"), y("y"), c("c");

    // Now we'll express a multi-stage pipeline that blurs an image
    // first horizontally, and then vertically.
    {
        // Take a color 8-bit input
        Buffer<uint8_t> input = load_image("rgb.png");

        // Upgrade it to 16-bit, so we can do math without it overflowing.
        Func input_16("input_16");
        input_16(x, y, c) = cast<uint16_t>(input(x, y, c));

        // Blur it horizontally:
        Func blur_x("blur_x");
        blur_x(x, y, c) = (input_16(x - 1, y, c) +
            2 * input_16(x, y, c) +
            input_16(x + 1, y, c)) / 4;

        // Blur it vertically:
        Func blur_y("blur_y");
        blur_y(x, y, c) = (blur_x(x, y - 1, c) +
            2 * blur_x(x, y, c) +
            blur_x(x, y + 1, c)) / 4;

        // Convert back to 8-bit.
        Func output("output");
        output(x, y, c) = cast<uint8_t>(blur_y(x, y, c));

        // Each Func in this pipeline calls a previous one using
        // familiar function call syntax (we've overloaded operator()
        // on Func objects). A Func may call any other Func that has
        // been given a definition. This restriction prevents
        // pipelines with loops in them. Halide pipelines are always
        // feed-forward graphs of Funcs.

        // Now let's realize it...

        // Buffer<uint8_t> result = output.realize({input.width(), input.height(), 3});

        // Except that the line above is not going to work. Uncomment
        // it to see what happens.

        // Realizing this pipeline over the same domain as the input
        // image requires reading pixels out of bounds in the input,
        // because the blur_x stage reaches outwards horizontally, and
        // the blur_y stage reaches outwards vertically. Halide
        // detects this by injecting a piece of code at the top of the
        // pipeline that computes the region over which the input will
        // be read. When it starts to run the pipeline it first runs
        // this code, determines that the input will be read out of
        // bounds, and refuses to continue. No actual bounds checks
        // occur in the inner loop; that would be slow.
        //
        // So what do we do? There are a few options. If we realize
        // over a domain shifted inwards by one pixel, we won't be
        // asking the Halide routine to read out of bounds. We saw how
        // to do this in the previous lesson:
        Buffer<uint8_t> result(input.width() - 2, input.height() - 2, 3);
        result.set_min(1, 1);
        output.realize(result);

        // Save the result. It should look like a slightly blurry
        // parrot, and it should be two pixels narrower and two pixels
        // shorter than the input image.
        save_image(result, "/Images/Output/blur/blurry_parrot_1.png");

        // This is usually the fastest way to deal with boundaries:
        // don't write code that reads out of bounds :) The more
        // general solution is our next example.
    }

    // The same pipeline, with a boundary condition on the input.
    {
        // Take a color 8-bit input
        Buffer<uint8_t> input = load_image("rgb.png");

        // This time, we'll wrap the input in a Func that prevents
        // reading out of bounds:
        Func clamped("clamped");

        // Define an expression that clamps x to lie within the
        // range [0, input.width()-1].
        Expr clamped_x = clamp(x, 0, input.width() - 1);
        // clamp(x, a, b) is equivalent to max(min(x, b), a).

        // Similarly clamp y.
        Expr clamped_y = clamp(y, 0, input.height() - 1);
        // Load from input at the clamped coordinates. This means that
        // no matter how we evaluated the Func 'clamped', we'll never
        // read out of bounds on the input. This is a clamp-to-edge
        // style boundary condition, and is the simplest boundary
        // condition to express in Halide.
        clamped(x, y, c) = input(clamped_x, clamped_y, c);

        // Defining 'clamped' in that way can be done more concisely
        // using a helper function from the BoundaryConditions
        // namespace like so:
        //
        // clamped = BoundaryConditions::repeat_edge(input);
        //
        // These are important to use for other boundary conditions,
        // because they are expressed in the way that Halide can best
        // understand and optimize. When used correctly they are as
        // cheap as having no boundary condition at all.

        // Upgrade it to 16-bit, so we can do math without it
        // overflowing. This time we'll refer to our new Func
        // 'clamped', instead of referring to the input image
        // directly.
        Func input_16("input_16");
        input_16(x, y, c) = cast<uint16_t>(clamped(x, y, c));

        // The rest of the pipeline will be the same...

        // Blur it horizontally:
        Func blur_x("blur_x");
        blur_x(x, y, c) = (input_16(x - 1, y, c) +
            2 * input_16(x, y, c) +
            input_16(x + 1, y, c)) / 4;

        // Blur it vertically:
        Func blur_y("blur_y");
        blur_y(x, y, c) = (blur_x(x, y - 1, c) +
            2 * blur_x(x, y, c) +
            blur_x(x, y + 1, c)) / 4;

        // Convert back to 8-bit.
        Func output("output");
        output(x, y, c) = cast<uint8_t>(blur_y(x, y, c));

        // This time it's safe to evaluate the output over the same
        // domain as the input, because we have a boundary condition.
        Buffer<uint8_t> result = output.realize({ input.width(), input.height(), 3 });

        // Save the result. It should look like a slightly blurry
        // parrot, but this time it will be the same size as the
        // input.
        save_image(result, "/Images/Output/blur/blurry_parrot_2.png");
    }

    printf("Success BLUR ANTONIO!\n");
    return 0;
}

//--------------------------------------------------------------

//--------------------------------------------------------------
void generate(string image) {

    //Input<Buffer<float>> input{ "input", 3 };
    Buffer<float> output{"output", 3 };

    Halide::Buffer<uint8_t> input = load_image(image);
    float sigma = 1.5f;

    Var x("x"), y("y"), c("c");

    const float kPi = 3.14159265358979310000f;

    Func kernel("kernel");
    kernel(x) = exp(-x * x / (2 * sigma * sigma)) / (sqrtf(2 * kPi) * sigma);

    Func input_bounded = Halide::BoundaryConditions::repeat_edge(input);

    Func gray("gray");
    gray(x, y) = (0.299f * input_bounded(x, y, 0) +
        0.587f * input_bounded(x, y, 1) +
        0.114f * input_bounded(x, y, 2));

    Func blur_y("blur_y");
    blur_y(x, y) = (kernel(0) * gray(x, y) +
        kernel(1) * (gray(x, y - 1) +
            gray(x, y + 1)) +
        kernel(2) * (gray(x, y - 2) +
            gray(x, y + 2)) +
        kernel(3) * (gray(x, y - 3) +
            gray(x, y + 3)));

    Func blur_x("blur_x");
    blur_x(x, y) = (kernel(0) * blur_y(x, y) +
        kernel(1) * (blur_y(x - 1, y) +
            blur_y(x + 1, y)) +
        kernel(2) * (blur_y(x - 2, y) +
            blur_y(x + 2, y)) +
        kernel(3) * (blur_y(x - 3, y) +
            blur_y(x + 3, y)));

    Func sharpen("sharpen");
    sharpen(x, y) = 2 * gray(x, y) - blur_x(x, y);

    Func ratio("ratio");
    ratio(x, y) = sharpen(x, y) / gray(x, y);

    output(x, y, c) = ratio(x, y) * input(x, y, c);
    /*
                // Estimates (for autoscheduler; ignored otherwise)
                {
                    input.dim(0).set_estimate(0, 1536);
                    input.dim(1).set_estimate(0, 2560);
                    input.dim(2).set_estimate(0, 3);
                    output.dim(0).set_estimate(0, 1536);
                    output.dim(1).set_estimate(0, 2560);
                    output.dim(2).set_estimate(0, 3);
                }
    */
    /*  // Schedule
      if (!auto_schedule) {
          // Some Intel Mac Minis have GPUs that require tile sizes smaller than 32x32
          // for this pipeline because they have too few registers. Drop to 16x16 to
          // avoid unexpected crashes in CI.
          const int tile_size = get_target().has_feature(Target::Metal) ? 16 : 32;

          if (get_target().has_gpu_feature()) {
              // The timing of this schedule is oddly noisy. Runs
              // from 0.4ms to 0.5ms on a 2060 RTX.  Oddly, the
              // better runtimes occur when running under nvprof.
              Var xi, yi;
              output.compute_root()
                  .reorder(c, x, y)
                  .gpu_tile(x, y, xi, yi, tile_size, tile_size)
                  .bound(c, 0, 3)
                  .unroll(c);
              ratio.compute_at(output, xi);
              gray.compute_at(output, x)
                  .tile(x, y, xi, yi, 2, 2)
                  .unroll(xi)
                  .unroll(yi)
                  .gpu_threads(x, y);
              blur_y.compute_at(output, x)
                  .unroll(x, 2)
                  .gpu_threads(x, y);
          }
          else {
              // 1.93ms on an Intel i9-9960X using 16 threads
              Var yo, yi;
              const int vec = natural_vector_size<float>();

              output.split(y, yo, yi, 32)
                  .vectorize(x, vec)
                  .parallel(yo)
                  .reorder(x, c, yi, yo);
              gray.compute_at(output, yi)
                  .store_at(output, yo)
                  .vectorize(x, vec);
              blur_y.compute_at(output, yi)
                  .store_at(output, yo)
                  .vectorize(x, vec);
              ratio.compute_at(output, yi)
                  .store_at(output, yo)
                  .vectorize(x, vec);
          }
      }*/
}
//---------------------------------------------------------------

int main(int argc, char** argv) {

    //brightOriginal(11);
    //brightReorder(11);
    //brightTiled(11);
    //brightVector(11);
    //brightParallel(11);
    //--------------------------------------------
    //blurOriginal("futurama.jpg", 9);
    //generate("futurama.jpg");
    BLUR_01("futurama.jpg");
    //--------------------------------------------
    //pipeline();
    return 0;

}




 