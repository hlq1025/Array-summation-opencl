#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "reduction_complete.cl"

#define ARRAY_SIZE 4097


#define KERNEL_1 "reduction_vector"
#define KERNEL_2 "reduction_complete"

#include <math.h>
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <CL/cl.h>
using namespace std;

/* Find a GPU or CPU associated with the first available platform */
cl_device_id create_device() {

	cl_int			iStatus = 0;				// 函数返回状态



	cl_uint			uiNumPlatforms = 0;				// 平台个数



	cl_platform_id	Platform = NULL;				// 选择的平台



	size_t			uiSize = 0;				// 平台版本名字字节数	



	cl_int			iErr = 0;				// 返回参数



	char			*pName = NULL;				// 平台版本名



	cl_uint			uiNumDevices = 0;				// 设备数量



	cl_device_id	*pDevices = NULL;				// 设备



	iStatus = clGetPlatformIDs(0, NULL, &uiNumPlatforms);



	if (CL_SUCCESS != iStatus)



	{



		cout << "Error: Getting platforms error" << endl;



		return 0;



	}











	// 获得平台地址



	if (uiNumPlatforms > 0)  // 如果有可用平台



	{



		// 根据平台数为平台分配内存空间



		cl_platform_id *pPlatforms = (cl_platform_id *)malloc(uiNumPlatforms * sizeof(cl_platform_id));







		// 获得可用的平台



		iStatus = clGetPlatformIDs(uiNumPlatforms, pPlatforms, NULL);



		Platform = pPlatforms[1];	// 获得第一个平台的地址



		free(pPlatforms);			// 释放平台占用的内存空间



	}







	// 获得平台版本名



	// 获得平台版本名的字节数



	iErr = clGetPlatformInfo(Platform, CL_PLATFORM_VERSION, 0, NULL, &uiSize);







	// 根据字节数为平台版本名分配内存空间



	pName = (char *)alloca(uiSize * sizeof(char));







	// 获得平台版本名字



	iErr = clGetPlatformInfo(Platform, CL_PLATFORM_VERSION, uiSize, pName, NULL);



	cout << pName << endl;















	//--------------2. 查询GPU设备，并选择可用设备------------------------



	// 获得GPU设备数量



	iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiNumDevices);



	if (0 == uiNumDevices)	// 如果没有GPU设备



	{



		cout << "No GPU device available." << endl;



		cout << "Choose CPU as default device." << endl;







		// 选择CPU作为设备，获得设备数



		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_CPU, 0, NULL, &uiNumDevices);







		// 为设备分配空间



		pDevices = (cl_device_id *)malloc(uiNumDevices * sizeof(cl_device_id));







		// 获得平台



		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_CPU, uiNumDevices, pDevices, NULL);



	}



	else



	{



		pDevices = (cl_device_id *)malloc(uiNumDevices * sizeof(cl_device_id));







		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, uiNumDevices, pDevices, NULL);



	}






	return pDevices[0];

   


  }
   

/* Create program from a file and compile it */
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file */
   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   /* Build program */
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}

int main() {

   /* OpenCL structures */
   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_kernel  complete_kernel;
   cl_command_queue queue;
   cl_event  end_event;
   cl_int i, err;
   size_t  global_size;

   /* Data and buffers */
   float data[ARRAY_SIZE];
   float sum, actual_sum;
   cl_mem data_buffer, sum_buffer,sum_partial;
   
   global_size = ARRAY_SIZE;
   /* Initialize data */
   for(i=0; i<ARRAY_SIZE; i++) {
      data[i] = 1.0f*i;
   }

   /* Create device and determine local size */
   device = create_device();
   //err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, 	
         //sizeof(local_size), &local_size, NULL);	
   //if(err < 0) {
     // perror("Couldn't obtain device information");
      //exit(1);   
   //}

   /* Create a context */
   context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
   if(err < 0) {
      perror("Couldn't create a context");
      exit(1);   
   }

   /* Build program */
   program = build_program(context, device, PROGRAM_FILE);

   /* Create data buffer */
   data_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE |
         CL_MEM_USE_HOST_PTR, ARRAY_SIZE * sizeof(float), data, &err);
   sum_partial = clCreateBuffer(context, CL_MEM_READ_WRITE, ARRAY_SIZE * sizeof(float), data, &err);
   sum_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 
         sizeof(float), NULL, &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   /* Create a command queue */
   queue = clCreateCommandQueue(context, device, 
         CL_QUEUE_PROFILING_ENABLE, &err);
   if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
   };

   /* Create kernels */
   //vector_kernel = clCreateKernel(program, KERNEL_1, &err);
   complete_kernel = clCreateKernel(program, KERNEL_2, &err);
   if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
   };

  

   /* Set arguments for complete kernel */
   err = clSetKernelArg(complete_kernel, 0, sizeof(cl_mem), &data_buffer);
   err |= clSetKernelArg(complete_kernel, 1, sizeof(cl_mem), &sum_partial);
   err |= clSetKernelArg(complete_kernel, 2, sizeof(cl_mem), &sum_buffer);
   if(err < 0) {
      perror("Couldn't create a kernel argument");
      exit(1);   
   }

   /* Enqueue kernels */
   //global_size = ARRAY_SIZE/4;
 
   err = clEnqueueNDRangeKernel(queue, complete_kernel, 1, NULL, &global_size, 
         NULL, 0, NULL, &end_event);
   printf("Global size = %lu\n", global_size);

   /* Finish processing the queue and get profiling information */
   clFinish(queue);
   //clGetEventProfilingInfo(start_event, CL_PROFILING_COMMAND_START,
        // sizeof(time_start), &time_start, NULL);
   //clGetEventProfilingInfo(end_event, CL_PROFILING_COMMAND_END,
        // sizeof(time_end), &time_end, NULL);
   //total_time = time_end - time_start;

   /* Read the result */
   err = clEnqueueReadBuffer(queue, sum_buffer, CL_TRUE, 0, 
      sizeof(float), &sum, 0, NULL, NULL);
   if(err < 0) {
      perror("Couldn't read the buffer");
      exit(1);   
   }

   /* Check result */
   actual_sum = 1.0f * ARRAY_SIZE*(ARRAY_SIZE-1)/2;
   cout << actual_sum << endl;
   cout << sum << endl;
   if(fabs(sum - actual_sum) > 0.01*fabs(sum))
      printf("Check failed.\n");
   else
      printf("Check passed.\n");
  // printf("Total time = %lu\n", total_time);

   /* Deallocate resources */
   //clReleaseEvent(start_event);
   //clReleaseEvent(end_event);
   clReleaseMemObject(sum_buffer);
   clReleaseMemObject(data_buffer);
   //clReleaseKernel(vector_kernel);
   clReleaseKernel(complete_kernel);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
   return 0;
}
