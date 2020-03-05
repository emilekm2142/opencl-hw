// TODO: Add OpenCL kernel code here.
__kernel void boxBlur(__read_only image2d_t inputImage, __write_only image2d_t output_image, int width, int height, sampler_t sampler) {
	int column = get_global_id(0);
	int row = get_global_id(1);


}