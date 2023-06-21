// Halide Aula 00
// 
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

#include <iostream>

// The only Halide header file you need is Halide.h. It includes all of Halide.
#include "Halide.h"

// Resolution	Pixel Size
// Full HD 1920 x 1080
// 2K	2048 x 1152
// 4K 3840 x 2160 pixel
// DCI 4K	4096 x 2160
// An 8 megapixel image is 3456 pixels wide and 2304 pixels tall
// A 12 megapixel image is 4000 pixels wide and 3000 pixels tall

int h = 2048;   //input.height();
int w = 2048;   //input.width();
int c = 3;      //input.channels();




int main()
{
    std::cout << "Hello HALIDE World!\n";

    for (int j = 0;j < h;j++) {
        for (int k = 0;k < w;k++) {
        }

    }

}