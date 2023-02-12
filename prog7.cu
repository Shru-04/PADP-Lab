#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <time.h>

//OpenCL kernel function string to be fed in each device's work-item				(V.IMP)
const char *saxpy_kernel = "__kernel void saxpy_kernel(float alpha, 	"
		     "				__global float *A,	"
		     "				__global float *B,	"
		     "				__global float *C){	"
		     "		//Get global index of work item		\n"
		     "		int id = get_global_id(0);		"
		     "		C[id] = alpha * A[id] + B[id];		"
		     "	    }						";

int main(int argc, char **argv){
	if (argc != 3){
		printf("Usage : ./prog7 <vector size> <local size/threads>\n");
		exit(1);
	}

	unsigned int VECTOR_SIZE = atoi(argv[1]);
	int i;
	double start,end;

	// Allocate A,B,C in host memory
	float *A = (float *)calloc(VECTOR_SIZE,sizeof(float));
	float *B = (float *)calloc(VECTOR_SIZE,sizeof(float));
	float *C = (float *)calloc(VECTOR_SIZE,sizeof(float));
	float alpha = 2.0;
	for (i = 0; i < VECTOR_SIZE; i++){
		A[i] = i;
		B[i] = VECTOR_SIZE - i;
	}

	cl_int clstat;
	start = clock();
	
	// Get platform and device info (ids n nums)
	cl_platform_id *platforms = NULL;
	cl_uint num_platforms;
	clGetPlatformIDs(0,NULL,&num_platforms);
	platforms = (cl_platform_id *)malloc(num_platforms * sizeof(cl_platform_id));    			
	clGetPlatformIDs(1,platforms,NULL);

	cl_device_id *device_list = NULL;
	cl_uint num_devices;
	clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_GPU,0,NULL,&num_devices);
	device_list = (cl_device_id *)calloc(num_devices,sizeof(cl_device_id));
	clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_GPU,num_devices,device_list,NULL);

	// Create OpenCL context for each device
	cl_context context = clCreateContext(NULL,num_devices,device_list,NULL, NULL, &clstat);

	// Create Command Queue
	cl_command_queue cmd_queue = clCreateCommandQueueWithProperties(context,device_list[0],NULL,&clstat);
	
	// Create Memory Buffers (buffer objects)
	cl_mem A_clmem = clCreateBuffer(context,CL_MEM_READ_ONLY,VECTOR_SIZE * sizeof(float),NULL,&clstat);		//vimp - (context,flag,size,NULL,NULL)
	cl_mem B_clmem = clCreateBuffer(context,CL_MEM_READ_ONLY,VECTOR_SIZE * sizeof(float),NULL,&clstat);
	cl_mem C_clmem = clCreateBuffer(context,CL_MEM_WRITE_ONLY,VECTOR_SIZE * sizeof(float),NULL,&clstat);

	// Copy host objects into device memory
	clEnqueueWriteBuffer(cmd_queue,A_clmem,CL_TRUE,0,VECTOR_SIZE * sizeof(float),A,0,NULL,NULL);		//vimp - (cq, cl_mem, block_write, offset, size, host_mem, 0, NULL, NULL)
	clEnqueueWriteBuffer(cmd_queue,B_clmem,CL_TRUE,0,VECTOR_SIZE * sizeof(float),B,0,NULL,NULL);

	// Create Program From Kernel Source
	cl_program prg = clCreateProgramWithSource(context,1,(const char **)&saxpy_kernel, NULL, &clstat);	//vimp - (context, count, **kernel_string, lengths = NULL, NULL)

	// Build Program Object
	clBuildProgram(prg,num_devices,device_list,0,NULL,NULL);						//vimp - (programobj, num_devices, device_list[0], events = 0,NULL,NULL)

	// Create OpenCL kernel for each device
	cl_kernel kernel = clCreateKernel(prg, "saxpy_kernel", NULL);						//vimp - (programobj, func_name. NULL)

	// Set Kernel Args
	clSetKernelArg(kernel,0,sizeof(float),(void *)&alpha);						//vimp - (kernel, arg_pos, sizeof, arg_var)
	clSetKernelArg(kernel,1,sizeof(cl_mem),(void *)&A_clmem);						
	clSetKernelArg(kernel,2,sizeof(cl_mem),(void *)&B_clmem);						
	clSetKernelArg(kernel,3,sizeof(cl_mem),(void *)&C_clmem);						
	
	// EXECUTE KERNEL FUNCTION
	size_t global_size = VECTOR_SIZE;		//Process all vector items
	size_t local_size = (size_t)atoi(argv[2]); 	//Set no. of work-items in workgrp
	
	clEnqueueNDRangeKernel(cmd_queue,kernel,1,0,&global_size,&local_size,0,NULL,NULL);		//vimp - (cq, kernel, dim, offset, *glob_s, *local_s, events = 0, NULL, NULL)

	// Copy results back to host mem
	clEnqueueReadBuffer(cmd_queue,C_clmem,CL_TRUE,0,VECTOR_SIZE * sizeof(float),C,0,NULL,NULL);

	// Flush and clean command queue
	clFlush(cmd_queue);
	clFinish(cmd_queue);

	end = clock();

	// Print results
	//for (i = 0; i < VECTOR_SIZE; i++){
	//	printf("%f * %f + %f = %f\n",alpha,A[i],B[i],C[i]);
	//}
	printf("\n\nTime taken = %lf sec\n",(end - start)/CLOCKS_PER_SEC);

	// RELEASE
	clReleaseKernel(kernel);
	clReleaseProgram(prg);
	clReleaseMemObject(A_clmem);
	clReleaseMemObject(B_clmem);
	clReleaseMemObject(C_clmem);
	clReleaseCommandQueue(cmd_queue);
	clReleaseContext(context);
	free(A);
	free(B);
	free(C);
	return 0;
}
