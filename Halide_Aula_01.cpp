#include "Halide.h"

// Include some support code for loading pngs.
#include "halide_image_io.h"
using namespace Halide::Tools;
//#include "clock.h"
#include<iostream>
#include <string>
using namespace std;

//  Halide tutorials load images. 2022
//#include "Halide.h"

// Include some support code for loading pngs.

// Add this include:
//#include "halide_image_io.h" // for load_image and save_image

//using namespace Halide::Tools;

int main(int argc, char** argv) {

	// Load image into in
	Halide::Runtime::Buffer<float> in = load_image("gray.png");

	// Save imput into out!!
	Halide::Runtime::Buffer<float> out(in.width(), in.height(), in.channels());

	// add the line:   error check!!
	//int error = halide_blur(in, out); //halide_blur is the function defined in aot compiled code

	//saving the output
	//save_image(out, "./Images/Output/output.png");


	// Save the output for inspection. It should look like a bright parrot.
	save_image(out, "brighterTodday.png");


	return 0;

}