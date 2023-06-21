


// SCHEDULER 05
// Localite Scheduler optimatizon (tiles)
void Scheduler_05() {
	result.comput_root()
		.tile(x, y, xi, yi, 128, 24)
		.vectorize(x, 32);

	// Compute_at eanble interlince two Funcs
	// moving loops of the producer Func into the Consumer Func

	blur_x.compute_at(result, x)
		.vectorize(x, 32);

	input_16.compute_at(result, x)
		.vectorize(x, 32);
	return 0;
}