#include "halide.h"
using namespace std;
using namespace Halide;

int main(int argc, char** argv) {

	Halide::Func Soma;

	Halide::Var a, b;

	Halide::Expr res = a + b;

	Soma(a, b) = res;

	Soma.realize();
	Soma.print_loop_nest();

}