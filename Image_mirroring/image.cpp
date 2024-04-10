#include <iostream>
#include <vector>
#include <CL/opencl.hpp>
#include <opencv2/opencv.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>

int main() {
    cv::Mat inputImage = cv::imread("input.jpg", cv::IMREAD_GRAYSCALE);
    if (inputImage.empty()) {
        std::cerr << "Error: Could not open or find the image" << std::endl;
        return 1;
    }

    cv::imshow("Input Image", inputImage);


    int width = inputImage.cols;
    int height = inputImage.rows;
    int choice;
    std::string source;
    
    // Get available OpenCL platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    // Choose the first platform
    cl::Platform platform = platforms[0];

    // Get available devices
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

    // Create a context and command queue for the first device
    cl::Context context(devices);
    cl::CommandQueue queue(context, devices[0]);
    std::cout<<"1 for vertical flip and 2 for horizontal flip: ";
    std::cin>>choice;
    if(choice==1)
    {
        std::ifstream sourceFile("ver_flip.cl");
        if (!sourceFile.is_open()) {
            std::cerr << "Failed to open OpenCL source file." << std::endl;
            return {};
        }
        source.assign(std::istreambuf_iterator<char>(sourceFile),(std::istreambuf_iterator<char>()));

        cl::Program program(context, source);
        if (program.build(devices) != CL_SUCCESS) {
        std::cerr << "Error building program: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
        return 1;
        }

        // Create OpenCL buffers 
        cl::Buffer d_input(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uchar)*width*height, inputImage.data);
        cl::Buffer d_output(context, CL_MEM_WRITE_ONLY, sizeof(uchar)*width*height);

        // Create the OpenCL kernel
        cl::Kernel kernel(program, "vertical");
        kernel.setArg(0, d_input);
        kernel.setArg(1, d_output);
        kernel.setArg(2, width);
        kernel.setArg(3, height);

        // Execute the OpenCL kernel
        cl_int err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(width, height));

        if (err != CL_SUCCESS) {
            std::cerr << "Error enqueuing kernel. Error code: " << err << std::endl;
            return {};
        }
        // Read bufferOutput into a local array
        std::vector<uchar> outputImage(width * height);
        queue.enqueueReadBuffer(d_output, CL_TRUE, 0, sizeof(uchar) * width * height, outputImage.data());

        // Create output image
        cv::Mat outputMat(height, width, CV_8UC1, outputImage.data());

        // Display output image
        cv::imshow("Output Image", outputMat);
        cv::imwrite("Vertical_flip.jpg", outputMat);

        // Wait for a key press
        cv::waitKey(0);
    }
    else if(choice==2)
    {
        std::ifstream sourceFile("hor_flip.cl");
        if (!sourceFile.is_open()) {
            std::cerr << "Failed to open OpenCL source file." << std::endl;
            return {};
        }
        source.assign(std::istreambuf_iterator<char>(sourceFile),(std::istreambuf_iterator<char>()));

        cl::Program program(context, source);
        if (program.build(devices) != CL_SUCCESS) {
        std::cerr << "Error building program: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
        return 1;
        }

        // Create OpenCL buffers 
        cl::Buffer d_input(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uchar)*width*height, inputImage.data);
        cl::Buffer d_output(context, CL_MEM_WRITE_ONLY, sizeof(uchar)*width*height);

        // Create the OpenCL kernel
        cl::Kernel kernel(program, "horizontal");
        kernel.setArg(0, d_input);
        kernel.setArg(1, d_output);
        kernel.setArg(2, width);
        kernel.setArg(3, height);

        // Execute the OpenCL kernel
        cl_int err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(width, height));

        if (err != CL_SUCCESS) {
            std::cerr << "Error enqueuing kernel. Error code: " << err << std::endl;
            return {};
        }
        // Read bufferOutput into a local array
        std::vector<uchar> outputImage(width * height);
        queue.enqueueReadBuffer(d_output, CL_TRUE, 0, sizeof(uchar) * width * height, outputImage.data());

        // Create output image
        cv::Mat outputMat(height, width, CV_8UC1, outputImage.data());

        // Display output image
        cv::imshow("Output Image", outputMat);
        cv::imwrite("Horizontal_flip.jpg", outputMat);

        // Wait for a key press
        cv::waitKey(0);
    }
    else
    {
        std::cout<<"Invalid Choice\n";
        return 0;
    }
    return 0;
}
