#include <iostream>
#include <vector>
#include <CL/opencl.hpp>
#include <cstdio>
#include <cstdlib>
#include <fstream>

int main() {
    int x = 1024, y = 10, z = 10, w = 10;
    
    // Declare a 4D array using nested vectors
    std::vector<std::vector<std::vector<std::vector<int>>>> arr4d(x,std::vector<std::vector<std::vector<int>>>(y,std::vector<std::vector<int>>(z,std::vector<int>(w))));

    int sum = 0;
    int product=1;
    int max=0;
    int min=999;
    std::vector<int> h_array(x * y * z * w);
    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            for (int k = 0; k < z; ++k) {
                for (int l = 0; l < w; ++l) {
                    arr4d[i][j][k][l] = 3;
                    sum += arr4d[i][j][k][l];
                    product *= arr4d[i][j][k][l];
                    if(arr4d[i][j][k][l]>max)
                        max=arr4d[i][j][k][l];
                    if(arr4d[i][j][k][l]<min)
                        min=arr4d[i][j][k][l];
                    h_array[i * y * z * w + j * z * w + k * w + l] = arr4d[i][j][k][l];
                }
            }
        }
    }
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
    std::cout<<"\n1 for sum,2 for product,3 for min,4 for max: ";
    std::cin>>choice;
    if(choice==1)
    {
        std::ifstream sourceFile("sum.cl");
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

        int global_size=x*y*z*w;
        int local_size=512;
        // Create OpenCL buffers 
        cl::Buffer d_input(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int)*x*y*z*w, h_array.data());
        cl::Buffer d_output(context, CL_MEM_WRITE_ONLY, sizeof(int)*global_size/local_size);


        // Create the OpenCL kernel
        cl::Kernel kernel(program, "sum");
        kernel.setArg(0, d_input);
        kernel.setArg(1, d_output);

        // Execute the OpenCL kernel
        cl_int err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global_size), cl::NDRange(local_size));

        if (err != CL_SUCCESS) {
            std::cerr << "Error enqueuing kernel. Error code: " << err << std::endl;
            return {};
        }
        // Read bufferOutput into a local array
        std::vector<int> output(global_size/local_size);
        queue.enqueueReadBuffer(d_output, CL_TRUE, 0, sizeof(int) * global_size/local_size, output.data());
        int csum=0;
        for(int i=0;i<global_size/local_size;i++)
        {
            csum+=output[i];
        }
        std::cout<<"Actual Sum: "<<sum<<" Computed Sum: "<<csum<<std::endl;
    }
    else if(choice==2)
    {
        std::ifstream sourceFile("product.cl");
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

        int global_size=x*y*z*w;
        int local_size=512;
        // Create OpenCL buffers 
        cl::Buffer d_input(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int)*x*y*z*w, h_array.data());
        cl::Buffer d_output(context, CL_MEM_WRITE_ONLY, sizeof(int)*global_size/local_size);


        // Create the OpenCL kernel
        cl::Kernel kernel(program, "product");
        kernel.setArg(0, d_input);
        kernel.setArg(1, d_output);

        // Execute the OpenCL kernel
        cl_int err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global_size), cl::NDRange(local_size));

        if (err != CL_SUCCESS) {
            std::cerr << "Error enqueuing kernel. Error code: " << err << std::endl;
            return {};
        }
        // Read bufferOutput into a local array
        std::vector<int> output(global_size/local_size);
        queue.enqueueReadBuffer(d_output, CL_TRUE, 0, sizeof(int) * global_size/local_size, output.data());
        int cproduct=1;
        for(int i=0;i<global_size/local_size;i++)
        {
            cproduct*=output[i];
        }
        std::cout<<"Actual Product: "<<product<<" Computed Product: "<<cproduct<<std::endl;
    }
    else if(choice==3)
    {
        std::ifstream sourceFile("min.cl");
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

        int global_size=x*y*z*w;
        int local_size=512;
        // Create OpenCL buffers 
        cl::Buffer d_input(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int)*x*y*z*w, h_array.data());
        cl::Buffer d_output(context, CL_MEM_WRITE_ONLY, sizeof(int)*global_size/local_size);


        // Create the OpenCL kernel
        cl::Kernel kernel(program, "min");
        kernel.setArg(0, d_input);
        kernel.setArg(1, d_output);

        // Execute the OpenCL kernel
        cl_int err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global_size), cl::NDRange(local_size));

        if (err != CL_SUCCESS) {
            std::cerr << "Error enqueuing kernel. Error code: " << err << std::endl;
            return {};
        }
        // Read bufferOutput into a local array
        std::vector<int> output(global_size/local_size);
        queue.enqueueReadBuffer(d_output, CL_TRUE, 0, sizeof(int) * global_size/local_size, output.data());
        int cmin=99999;
        for(int i=0;i<global_size/local_size;i++)
        {
            if(output[i]<cmin)
                cmin=output[i];
        }
        std::cout<<"Actual Min: "<<min<<" Computed Min: "<<cmin<<std::endl;
    }
    else if(choice==4)
    {
        std::ifstream sourceFile("max.cl");
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

        int global_size=x*y*z*w;
        int local_size=512;
        // Create OpenCL buffers 
        cl::Buffer d_input(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int)*x*y*z*w, h_array.data());
        cl::Buffer d_output(context, CL_MEM_WRITE_ONLY, sizeof(int)*global_size/local_size);


        // Create the OpenCL kernel
        cl::Kernel kernel(program, "max");
        kernel.setArg(0, d_input);
        kernel.setArg(1, d_output);

        // Execute the OpenCL kernel
        cl_int err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global_size), cl::NDRange(local_size));

        if (err != CL_SUCCESS) {
            std::cerr << "Error enqueuing kernel. Error code: " << err << std::endl;
            return {};
        }
        // Read bufferOutput into a local array
        std::vector<int> output(global_size/local_size);
        queue.enqueueReadBuffer(d_output, CL_TRUE, 0, sizeof(int) * global_size/local_size, output.data());
        int cmax=0;
        for(int i=0;i<global_size/local_size;i++)
        {
            if(output[i]>cmax)
                cmax=output[i];
        }
        std::cout<<"Actual Max: "<<max<<" Computed Max: "<<cmax<<std::endl;
    }
    else
    {
        std::cout<<"Invalid Choice\n";
        return 0;
    }
    return 0;
}
