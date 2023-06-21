


// SCHEDULER 03
// Vectorize Loops 
void Scheduler_03() {
	result.compute_root().vertorize(x, 32);
	blur_x.compute_root().vertorize(x, 32);
	input_16.compute_root().vertorize(x, 32);
	return 0;
}