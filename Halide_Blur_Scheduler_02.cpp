
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
   // Halide::Buffer<uint8_t> input = load_image("rgb.png");

	// First we'll load the input image we wish to brighten.
	Halide::Buffer<uint8_t> input = load_image(".//Images//futurama.jpg");


	// Our Func will have three arguments, representing the position
	// in the image and the color channel. Halide treats color
	// channels as an extra dimension of the image.
	Halide::Var x, y, c;

	// For each pixel of the input image.
	Halide::Expr input_Cast = input(x, y, c);

	// Cast it to a floating point value.
	input_Cast = Halide::cast<float>(input_Cast);



	Func input_16, blur_x, blur_y, result;
	/*
	input_16(x, y) = cast<uint16_t>(input(clamp(x, 0, input.width() - 1),
		clamp(y, 0, input.height() - 1));
	*/
	// Repeat Edge add into Halide now	

	  // Define the function - Clamped
	Func clamped = Halide::BoundaryConditions::repeat_edge(input_Cast);

	// Define the function - Input_16
	input_16(x, y) = cast<uint16_t>(clamped(x, y));

	// Define the function - Blur_x
	blur_x(x, y) = (input_16(x - 1, y) +
		input_16(x, y) +
		input_16(x + 1, y)) / 3;
	
	// Define the function - Blur_y
	blur_y(x, y) = (input_16(x, y - 1) +
		input_16(x, y) +
		input_16(x, y + 1)) / 3;
	
	// Define the function - Result
	result(x, y) = cast<uint8_t>(blur_y(x, y));

	result.compile_jit();

	// on the Output image.
	Halide::Buffer<uint8_t> output = 
		result.realize({ input.width(), input.height() });
	 convert_and_save_image(output, argv[2]);



	// Save the output for inspection. It should look like a bright parrot.
	save_image(output, ".//Images//Output//Blur_Sch_02.png");


	printf("Halide - Aula 01 - Success - Func = Blur SCHEDULER 02!\n");
	return 0;

}

/*
// SCHEDULER 02
// Halide Reversed Order by default y,x
void Scheduler_02() {

	result.compute_root();
	blur_x.compute_root();
	input_16.compute_root();
	return 0;
}
*/