#include "tracer.h"
#include <cstdio>
#include <iostream>
#include <cmath>
#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)
#define NOASK

using glm::dvec3;
using glm::dvec2;
using glm::uvec2;
using glm::length;
using namespace std;

void COpenCLmanager::checkErr(cl_int err, const char * name) {
    if (err != CL_SUCCESS) {
	  	cerr << "ERROR: " << name  << " (" << err << ")" << endl;
	  	exit(EXIT_FAILURE);
   	}
}

void COpenCLmanager::clinit()
{
	cl_uint num_of_platforms = 0;
	cl_int err = clGetPlatformIDs(0, 0, &num_of_platforms);
	checkErr(err,"clGetPlatformIDs");
	vector<cl_platform_id> platforms(num_of_platforms);
	err = clGetPlatformIDs(num_of_platforms, &platforms[0], 0);
	checkErr(err,"clGetPlatformIDs");

	cout << "Platforms (" << num_of_platforms << "):\n";

	for(cl_uint i = 0; i < num_of_platforms; ++i)
    {
        size_t platform_name_length = 0;
        err = clGetPlatformInfo(
        	platforms[i],
        	CL_PLATFORM_NAME,
        	0,
        	0,
        	&platform_name_length
        );
        checkErr(err,"clGetPlatformInfo");

        vector<char> platform_name(platform_name_length);
        err = clGetPlatformInfo(
            platforms[i],
            CL_PLATFORM_NAME,
            platform_name_length,
            &platform_name[0],
            0
        );
        checkErr(err,"clGetPlatformInfo");

        cout << "    [" << i << "] " << &platform_name[0] <<endl;
    }

    cl_int selected_platform = 0;
    #ifndef NOASK
    	cout <<"Choose platform number, please :\n" << endl;
	    
	    int x;
	    cin >> x;
	    selected_platform = x;
	    if ((selected_platform>=0)&&(selected_platform<num_of_platforms))
	    {
	    	cout <<"Selected platform is "<<selected_platform<<endl;
	    }
	    else
	    {
	    	selected_platform = 0;
	    	cout <<"Using default platform "<<selected_platform<<endl;
	    }
    #endif
    
    cl_uint num_of_devices = 0;
    err = clGetDeviceIDs(
        platforms[selected_platform],
        CL_DEVICE_TYPE_ALL,
        0,
        0,
        &num_of_devices
    );
    checkErr(err,"clGetDeviceIDs");
    vector<cl_device_id> devices(num_of_devices);
    err = clGetDeviceIDs(
        platforms[selected_platform],
        CL_DEVICE_TYPE_ALL,
        num_of_devices,
        &devices[0],
        0
    );
    checkErr(err,"clGetDeviceIDs");
    cout << "Devices (" << num_of_devices<< "):\n";
    for(cl_uint i = 0; i < num_of_devices; ++i)
    {
        size_t device_name_length = 0;
        err = clGetDeviceInfo(
            devices[i],
            CL_DEVICE_NAME,
            0,
            0,
            &device_name_length
        );
        checkErr(err,"clGetDeviceInfo");

        vector<char> device_name(device_name_length);
        err = clGetDeviceInfo(
            devices[i],
            CL_DEVICE_NAME,
            device_name_length,
            &device_name[0],
            0
        );
        checkErr(err,"clGetDeviceInfo");
        cout << "    [" << i << "] " << &device_name[0]<<endl;  
    }
    cl_int selected_device = 0;
    if(num_of_devices>1)
    {	
    	cout <<"Choose device number, please :\n" << endl;
	    //cin << selected_device << endl;
	    int x;
	    cin >> x;
	    selected_device = x;
	    if ((selected_device>=0)&&(selected_device<num_of_devices))
	    {
	    	cout<< "Selected device is "<<selected_device<<endl;
	    }
	    else
	    {
	    	selected_device = 0;
	    	cout <<"Using default device "<<selected_device<<endl;
    	}
    }
    device_id = devices[selected_device];

    context= clCreateContext(
    	NULL, 
    	1, 
    	&(devices[selected_device]), 
    	NULL, 
    	NULL, 
    	&err
    );
    checkErr(err,"clCreateContext");

    command_queue = clCreateCommandQueue(
    	context, 
    	devices[selected_device], 
    	0, 
    	&err
    );
    checkErr(err,"clCreateCommandQueue");
}

void COpenCLmanager::buildkernel()
{
	cl_int err;
	cl_program program = NULL;
	kernel = NULL;

	FILE *fp;
	const char fileName[] = "../src/cl/tracer.cl";
	size_t source_size;
	char *source_str;
	int i;

	try {
		fp = fopen(fileName, "r");
		if (!fp) {
			fprintf(stderr, "Failed to load kernel.\n");
			exit(1);
		}
		source_str = (char *)malloc(MAX_SOURCE_SIZE);
		source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
		fclose(fp);
	} 
	catch (int a) {
		printf("%d", a);
	}
	/* создать бинарник из кода программы */
	program = clCreateProgramWithSource(context, 1, 
		(const char **)&source_str, (const size_t *)&source_size, &err);
	checkErr(err,"clCreateProgramWithSource");
	/* скомпилировать программу */
	err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	checkErr(err,"clBuildProgram");
	/* создать кернел */
	kernel = clCreateKernel(program, "test", &err);
	checkErr(err,"clCreateKernel");
}

int main(int argc, const char** argv)
{
        CTracer tracer;
        COpenCLmanager clmanager;
        clmanager.clinit();
        clmanager.buildkernel();

        if(argc == 2)
        {
                FILE* file = fopen(argv[1], "r");
                if(file)
                {
                        tracer.m_camera.ParseSettings(file);
                        fclose(file);
                }
                else
                {
                        tracer.m_camera.DefaultInit();
                        printf("Invalid config path! Using default parameters.\r\n");
                }
        } else {
                tracer.m_camera.DefaultInit();
                printf("No config! Using default parameters.\r\n");
        }
        int xRes, yRes;
        xRes = tracer.m_camera.m_resolution.x;
        yRes = tracer.m_camera.m_resolution.y;
        for (int i = 0; i < xRes; i++) {
            for (int j = 0; j < yRes; j++) {
                tracer.m_camera.m_pixels[i * xRes + j] = dvec3(0.0,0.0,0.0);
            }
        }
        tracer.RenderImage(xRes, yRes, clmanager);
        tracer.SaveImageToFile("../img/clResult.png");
        return 0;
}
