#include <stdio.h>
#include <time.h>


#include <Halide.h>
#include <cstdio>

using namespace Halide;
//#include "image_io.h"

// Include some support code for loading pngs.
#include "halide_image_io.h"
using namespace Halide::Tools;


//----------------------------------------------------------------------
// Buffer<uint8_t> input
//Utility function for benchmark.
template <typename T>
//double benchmark(Func& input, Image<T>& im, const int repeat = 100, const char* label = "") {
double benchmark(Func& input, Buffer<uint8_t> im, const int repeat = 100, const char* label = "") {
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


//---------------------------------------------------------------------------------------


//----------------------------------------------------------------------
// Merge RGB channels to one gray channel.
// This also demonstrates how to load/save png images.
void image_rgb_to_gray() {
	Func gray, gray2;
	Var x, y, c;


	Buffer<uint8_t> image = load_image("rgb.png");

	//Image<float> image = load<float>("rgb.png");

	const int w = image.width();
	const int h = image.height();
	const int ch = image.channels();

	//ALGORITHM (the same for gray and gray2)
	gray(x, y, c) = 0.299f * image(x, y, 0) + 0.587f * image(x, y, 1) + 0.114f * image(x, y, 2);

	gray2(x, y, c) = gray(x, y, c);
	Buffer<uint8_t> out(w, h, 1);
	//Image<float> out(w, h, 1);  //the 3rd param is needed to output png.

	Image<Buffer> out(w, h, 1);  //the 3rd param is needed to output png.





	//SCHEDULE
	Var xi, yi;
	gray2.tile(x, y, xi, yi, 4, 4).vectorize(xi, 4).parallel(y);

	double t1 = benchmark(gray, out, 10, "image_rgb_to_gray");
	double t2 = benchmark(gray2, out, 10, "image_rgb_to_gray, par");
	printf("%.1fx speedup.\n", t1 / t2);

	save(out, "out_image_rgb_to_gray.png");
}

//---------------------------------------------------------------------------------------



int main()
{
	time_t rawtime;
	struct tm* timeinfo;

	// Used to store the time
	// returned by localetime() function
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80,
		"Time is %I:%M%p.",
		timeinfo);

	// strftime() function stores the
	// current time as Hours : Minutes
	//%I %M and %p-> format specifier
	// of Hours minutes and am/pm respectively*/

	// prints the formatted time
	puts(buffer);

	return 0;
}
