#include <Halide.h>
#include <halide_image_io.h>


// Include some support code for loading pngs.
//#include "halide_image_io.h"

#include "clock.h"
#include<iostream>
#include <string>

using namespace std;
using namespace Halide;
using namespace Halide::Tools;

int main(int argc, char* argv[]) {
	/*
	if (argc !3){
	std::cerr << "Usage : "<<argv[0]<< "<input> <output>\n";
	return 1;
	*/

	Buffer<uint8_t> input = load_and_convert_image(argv[1]);

	Var x, y;
	Func input_16, blur_x, blur_y, result;
	/*
	input_16(x, y) = cast<uint16_t>(input(clamp(x, 0, input.width() - 1),
		clamp(y, 0, input.height() - 1));
	*/
	// Repeat Edge add into Halide now	

	Func clamped = BoundaryConditions::repeat_edge(input);

	input_16(x, y) = cast<uint16_t>(clamped(x, y));
	
	blur_x(x, y) = (input_16(x - 1, y) +
					input_16(x, y) + 
					input_16(x + 1, y)) / 3;
	
	blur_y(x, y) = (input_16(x, y - 1) +
					input_16(x, y) + 
					input_16(x, y + 1)) / 3;

	result(x, y) = cast<uint8_t>(blur_y(x, y));

	result.compile_jit();

	Buffer<uint8_t> output = result.realize(input.width(), input.height());
	convert_and_save_image(output, argv[2]);
	return 0;
	
	
	//-------------------------------------------------
	Halide::Func Halide_blur, input;

	// Create Halide Funciron

	Func Halide_blur(Func input) {
		Var x, y;
		Func input_16, blur_x, blur_y, result;
		
		Func clamped = BoundaryConditions::repeat_edge(input);

		input_16(x, y) = cast<uint16_t>(clamped(x, y));

		blur_x(x, y) = (input_16(x - 1, y) +
			input_16(x, y) +
			input_16(x + 1, y)) / 3;

		blur_y(x, y) = (input_16(x, y - 1) +
			input_16(x, y) +
			input_16(x, y + 1)) / 3;

		result(x, y) = cast<uint8_t>(blur_y(x, y));
		return result;
	}
	// SCHEDULER 01
	void Scheduler_01() {

		result.compute_root().reorder(y, x);
		blur_x.compute_root().reorder(y, x);
		input_16.compute_root().reorder(y, x);
		return 0;
		}

	// SCHEDULER 02
	// Halide Reversed Order by default y,x
	void Scheduler_02() {

		result.compute_root();
		blur_x.compute_root();
		input_16.compute_root();
		return 0;
	}

	// SCHEDULER 0
	void Scheduler_00() {
		// ( empty schedule)
		// every Func execute inline 
		return 0;
	}

	// SCHEDULER 03
	// Vectorize Loops 
	void Scheduler_03() {
		result.compute_root().vertorize(x, 32);
		blur_x.compute_root().vertorize(x, 32);
		input_16.compute_root().vertorize(x, 32);
		return 0;
	}

	// SCHEDULER 04
// Localite Scheduler optimatizon (tiles)
	void Scheduler_04() {
		result.comput_root()
			.tile(x, y, xi, yi, 128, 24)
			.vectorize(x, 32);

		blur_x.compute_root()
				.vectorize(x, 32);

		input_16.compute_root()
			.vectorize(x, 32);

		return 0;
	}


	// SCHEDULER 05
// Localite Scheduler optimatizon (tiles)
	void Scheduler_05() {
		result.comput_root()
			.tile(x, y, xi, yi, 128, 24)
			.vectorize(x, 32);

		// Compute_at eanble interlince two Funcs
		// moving loops of the producer Func into the Consumer Func

		blur_x.compute_at(result,x)
					.vectorize(x, 32);

		input_16.compute_at(result, x)
			.vectorize(x, 32);
		return 0;
	}


	// SCHEDULER 06
// Localite Scheduler optimatizon (tiles)
	void Scheduler_06() {
		result.compute_root()
			.tile(x, y, xi, yi, 128, 24)
			.parallel(y)
			.vectorize(x, 32);

		// Compute_at eanble interlince two Funcs
		// moving loops of the producer Func into the Consumer Func

		blur_x.compute_at(result, yi)
			.store_at(result, x)
			.vectorize(x, 32);

		input_16.compute_at(result, x)
			.vectorize(x, 32);
		return 0;
	}
}