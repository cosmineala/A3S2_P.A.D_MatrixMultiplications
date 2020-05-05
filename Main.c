#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)
#define size_of_matrix 10

typedef struct MyMatrix {
   int size;
   int* matrix;
} Matrix;

int r_matrix( Matrix matrix, int x, int y );
void w_matrix( Matrix matrix, int x, int y, int e );

Matrix read_matrix();
void print_matrix( Matrix matrix );

int main(void) {

	Matrix M1 = read_matrix();
	printf("\n");
	Matrix M2 = read_matrix();
	// Load the kernel source code into the array source_str
	FILE* fp;
	char* source_str;
	size_t source_size;

	///Users/cosmin/CapisizuCosminDrive/An3/Sem2/P.D.A./Git/A3S2_P.A.D_MatrixMultiplications/mykernel.cl
	fp = fopen("mykernel.cl", "r");
		if (!fp) {
			fprintf(stderr, "Failed to load kernel.\n");
			exit(1);
		}
	
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	// Get platform and device information
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	// Create an OpenCL context
	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	// !!! Urmatoarele linii sunt foarte importante:
	//										* Numai una trebuie sa existe cealalta trebuie comentata.
	//										* In dreptul fiecarei lini este un comentriu in functie de platforma.
	cl_command_queue command_queue = clCreateCommandQueueWithPropertiesAPPLE(context, device_id, 0, &ret);//FOR MAC OS (APPLE)

	//cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, 0, &ret);//FOR WINDOWS
	
	int len = M1.size * M2.size * sizeof( int );
	// Create memory buffers on the device for each vector 
	cl_mem M1_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, len, NULL, &ret);
	cl_mem M2_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, len, NULL, &ret);
	cl_mem M3_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, len, NULL, &ret);

	// Copy the lists A and B to their respective memory buffers
	ret = clEnqueueWriteBuffer(command_queue, M1_mem_obj, CL_TRUE, 0, len, M1.matrix, 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, M2_mem_obj, CL_TRUE, 0, len, M2.matrix, 0, NULL, NULL);

	// Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, (const size_t*)&source_size, &ret);

	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	// Create the OpenCL kernel
	cl_kernel kernel = clCreateKernel(program, "multiplicationModule", &ret);

	// Set the arguments of the kernel
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&M1_mem_obj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&M2_mem_obj);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&M3_mem_obj);

	size_t global_item_size[2];
	//global_item_size[0] = size_of_matrix;
	global_item_size[0] = M1.size;
	global_item_size[1] = M2.size;
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_item_size, NULL, 0, NULL, NULL);

	Matrix M3;
	M3.size = M1.size;
	M3.matrix = malloc( M1.size * M2.size * sizeof(int) );
	ret = clEnqueueReadBuffer(command_queue, M3_mem_obj, CL_TRUE, 0, len, M3.matrix, 0, NULL, NULL);

	// Display the result to the screen
	printf("M1 * M2 = \n");
	print_matrix( M3 );

	return 0;
}

int r_matrix( Matrix matrix, int x, int y ){
	return matrix.matrix[ matrix.size * x + y ];
}
void w_matrix( Matrix matrix, int x, int y, int e ){
	matrix.matrix[ matrix.size * x + y ] = e;
}

void print_matrix( Matrix matrix ){
	for (int i = 0; i < matrix.size; ++i)
	{
		for (int j = 0; j < matrix.size; ++j)
		{
			printf("%d ", r_matrix( matrix, i, j ) );
		}
		printf("\n");
	}
}


Matrix read_matrix(){
					//printf("\nT1\n");
	Matrix matrix;
	printf("Enter marix size: ");
	scanf("%d", & matrix.size);
	matrix.matrix = malloc( matrix.size * sizeof( int ) );
					//printf("\nT2\n");

	for (int i = 0; i < matrix.size; ++i)
	{
		for (int j = 0; j < matrix.size; ++j)
		{
			int e;
			printf("\ti:%d j:%d = ", i, j );
			scanf("%d", &e);
			w_matrix( matrix, i, j, e );
		}
		printf("\n");
	}

	return matrix;
}
// void print_matrix( Matrix matrix ){
// 	for (int i = 0; i < matrix.size; ++i)
// 	{
// 		for (int j = 0; j < matrix.size; ++j)
// 		{
// 			r_matrix( matrix, i, j );
// 		}
// 		printf("\n");
// 	}
// }


