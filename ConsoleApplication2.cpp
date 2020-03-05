#pragma comment(lib, "OpenCL.lib")
#define _CRT_SECURE_NO_DEPRECATE
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <CL/cl.hpp>
#include <cstring>
#include "lodepng.h"
#include <fstream>
#include <iostream>
using namespace cl;
std::vector<unsigned char>* decodeOneStep(const char* filename, std::vector<unsigned char>* image) {
	unsigned width, height;

	//decode
	unsigned error = lodepng::decode(*image, width, height, filename);

	//if there's an error, display it
	if (error)
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	return image;
	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
}
void main() {
	int W = 10; int H = 10;

	std::vector<cl::Platform> platforms;
	Platform::get(&platforms);

	cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
	cl::Context context(CL_DEVICE_TYPE_ALL, cps);

	std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
	CommandQueue queue = CommandQueue(context, devices[0], 0);

	std::vector<unsigned char> ip = std::vector<unsigned char>();
	decodeOneStep("./cat.png", &ip);
	std::vector<unsigned char> op = std::vector<unsigned char>();


	auto input = Buffer(context, CL_MEM_READ_ONLY, W * H * sizeof(unsigned char));
	auto output = Buffer(context, CL_MEM_WRITE_ONLY, W * H * sizeof(unsigned char));
	

	auto imageForamt = ImageFormat(CL_R, CL_UNSIGNED_INT8);
	//clCreateImage
	auto BufferInputImage = Image2D(context, CL_MEM_READ_ONLY,imageForamt, W, H);
	auto BufferOutputImage = Image2D(context, CL_MEM_WRITE_ONLY, imageForamt, W,H);
	int filterSize = 5*5;
	auto bufferFilter = Buffer(context, 0, filterSize * sizeof(unsigned char));
	cl::size_t<3> origin;
	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	cl::size_t<3> region;
	origin[0] = W;
	origin[1] = H;
	origin[2] = 1;
	queue.enqueueWriteImage(BufferInputImage, CL_TRUE, origin, region, 0, 0, &ip[0]);
	Sampler sampler = cl::Sampler(context, CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST);
	std::ifstream sourceFileName = std::ifstream("boxBlur.cl");
	std::string sourceFile(std::istreambuf_iterator<char>(sourceFileName), (std::istreambuf_iterator<char>()));
	Program::Sources rotn_source(1, std::make_pair(sourceFile.c_str(), sourceFile.length() + 1));

	Program rotn_program(context, rotn_source);
	rotn_program.build(devices);
	Kernel kernel(rotn_program, "boxBlur");

	NDRange globalws(W, H);
	NDRange localws(8, 8);

	kernel.setArg(0, BufferInputImage);


	queue.enqueueNDRangeKernel(kernel, NullRange, globalws, localws);

	//queue.enqueueReadBuffer(output, CL_TRUE, 0, W * H * sizeof(unsigned char), op);
}