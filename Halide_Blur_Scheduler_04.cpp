



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
