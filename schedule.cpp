#include "Halide.h"
#include "halide_image_io.h"
#include<iostream>
#include <chrono>
#include <numeric>

#include <iomanip>
using namespace std;

using namespace std::chrono;
using namespace Halide::Tools;

void Exec_Timer (clock_t Start, clock_t End){
    std::cout << std::fixed << std::setprecision(9) << std::left;

    double num1 = 3.12345678;
    cout << fixed << showpoint;
    cout << setprecision(12);
    cout << "Timer:";
    cout << num1  ;
    cout << " miliseconds!!" << endl;
    printf("max_Time: %0.8f sec \n",
        ((float) End - Start) / CLOCKS_PER_SEC);

}

void brightOriginal(int factor){
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
        brighter.realize({input.width(), input.height(), input.channels()});

    //Stop timing
    end1 = clock();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Print Timer
    Exec_Timer(start1, end1);

    std::cout<<"-----------------------------------------"<<std::endl;
    std::cout <<"Original "<< duration.count() << " :microseconds" << std::endl;
    brighter.print_loop_nest();
    
    std::cout<<"-----------------------------------------"<<std::endl;
};

void brightReorder(int factor){
    clock_t start1, end1;
    
    Halide::Buffer<uint8_t> input = load_image("futurama.jpg");
    //Start timing
    start1 = clock();
    auto start = high_resolution_clock::now();
    Halide::Func brighter("brighter_reorder");
    Halide::Var x("x"), y("y"), c("c");
    Halide::Expr value = input(x, y, c);
    value = Halide::cast<int>(value);
    value = value + factor;
    value = Halide::min(value, 255.0f);
    value = Halide::cast<uint8_t>(value);
    brighter(x, y, c) = value;
    
    // ----------- Begin ----------------
    brighter.reorder(c,x,y);
    // -----------End---------------------
    
    
    Halide::Buffer<uint8_t> output =
        brighter.realize({input.width(), input.height(), input.channels()});
    
    //Stop timing
    end1 = clock();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Print Timer
    Exec_Timer(start1, end1);
    std::cout<<"-----------------------------------------"<<std::endl;
    std::cout <<"Reorder "<< duration.count() << std::endl;
    brighter.print_loop_nest();
    
    std::cout<<"-----------------------------------------"<<std::endl;
};

void brightTiled(int factor){
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
    brighter.reorder(c,x_inner,y_inner,x_outer,y_outer);
    // -----------End---------------------
    brighter.print_loop_nest();

    Halide::Buffer<uint8_t> output =
        brighter.realize({input.width(), input.height(), input.channels()});
    
    //Stop timing
    end1 = clock();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Print Timer
    Exec_Timer(start1, end1);
    std::cout<<"-----------------------------------------"<<std::endl;
    std::cout <<"Reorder "<< duration.count() << std::endl;
    brighter.print_loop_nest();
    
    std::cout<<"-----------------------------------------"<<std::endl;
};

void brightVector(int factor){
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
        brighter.realize({input.width(), input.height(), input.channels()});
    

    //Stop timing
    end1 = clock();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // Print Timer
    Exec_Timer(start1, end1);
    std::cout<<"-----------------------------------------"<<std::endl;
    std::cout <<"Vector "<< duration.count() << std::endl;
    brighter.print_loop_nest();
    
    std::cout<<"-----------------------------------------"<<std::endl;
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



int main(int argc, char **argv) {

    brightOriginal(11);
    brightReorder(11);
    brightTiled(11);
    brightVector(11);
    brightParallel(11);
     
    return 0;
     
}
