


// SCHEDULER 01
void Scheduler_01() {

	result.compute_root().reorder(y, x);
	blur_x.compute_root().reorder(y, x);
	input_16.compute_root().reorder(y, x);
	return 0;
}