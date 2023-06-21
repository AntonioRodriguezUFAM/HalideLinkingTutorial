#include "Halide.h"
using namespace Halide;

//class Blur :public Generator<Blur> {
	Input < Buffer<uint8_t>> input{ "input",2 };
	Output < Buffer<uint8_t>> result{ "result",2 };

	//void generate() {
		Var x, y;

	Func clamped = BoundaryConditions::repeat_edge(input);

	Func input_16;

	input_16(x, y) = cast<uint16_t>(clamped(x, y));

		Func blur_x;
		blur_x(x, y) = (input_16(x - 1, y) +
			input_16(x, y) +
			input_16(x + 1, y)) /
			3;


		Func blur_y;
		blur_y(x, y) = (input_16(x, y - 1) +
			input_16(x, y) +
			input_16(x, y + 1)) /
			3;

		result(x, y) = cast<uint8_t>(blur_y(x, y));

		// Scheduler 00------------------
		// Inline everthing : 2ms
		// Without Scheduler
		// 
		// Scheduler 01------------------
		// Compute_root: 10ms
		clamped.compute_root();
		input_16.compute_root();
		blur_x.compute_root();
		blur_y.compute_root();
		result.compute_root();

		// Scheduler 02------------------
		// Fusing things : 7.2ms
		input_16.compute_root();
		blur_x.compute_root();
		result.compute_root();

		//	Scheduler 03------------------
		// Better locality by tiles: 1.4ms

		Var xt, yt;
		result.compute_root()
			.tile(x,y,xt,yt,256,32);
		blur_x.compute_at(result,x);
		input_16.compute_at(result,x);

		//	Scheduler 04------------------
		// Add vetorations = 1.6ms
		const int vec = 32;
		Var xl, yl;
		result
			.compute_root()
			.tile(x, y, xl, yl, 256, 32)
			.vectorize(xl, vec);
		blur_x
			.compute_at(result, x)
			.vectorize(x, vec);

		input_16
			.compute_at(result, x)
			.vectorize(x, vec);

		//	Scheduler 05------------------
		// without vetorations = 1.4ms
		const int vec = 64;
		Var xl, yl;
		result
			.compute_root()
			.tile(x, y, xl, yl, 256, 32);
			//.vectorize(xl, vec);
		blur_x
			.compute_at(result, x);
			//.vectorize(x, vec);

		input_16
			.compute_at(result, x);
			//.vectorize(x, vec);

	//	Scheduler 06------------------
	// With vectorize and Storage
		// sliding windows = 1.3ms

		const int vec = 32;
		Var xl, yl;
		result
			.compute_root()
			.tile(x, y, xl, yl, 256, 32)
			.vectorize(xl, vec);
		blur_x
			.store_at (result, x)
			.compute_at(result, yl)
			.vectorize(x, vec);

		input_16
			.store_at(result, x)
			.compute_at(result, yl)
			.vectorize(x, vec);
		//	Scheduler 07------------------
	// With vectorize and Storage 
		// and Parallel = 0.3ms

		const int vec = 32;
		Var xl, yl;
		result
			.compute_root()
			.tile(x, y, xl, yl, 256, 32)
			.vectorize(xl, vec)
			.parallel (y);
		blur_x
			.store_at(result, x)
			.compute_at(result, yl)
			.vectorize(x, vec);

		input_16
			.store_at(result, x)
			.compute_at(result, yl)
			.vectorize(x, vec);
	
	//}
//};

int main(int argc, char** argv) {

	return 1;
}