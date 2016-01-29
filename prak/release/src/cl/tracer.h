#pragma once

#include "../glm/glm.hpp"
#include <CL/cl.h>
#include "../types.h"

#include "string"
#include <vector>
#include "CImg.h"

#define PRECISION 0.000001
#define NOCHANGE 0.01
#define GCONST 6.67384e-11
#define VC 3.0e+8

typedef cimg_library::CImg<unsigned char> CImage;

class COpenCLmanager
{
public:
	cl_context context;
	cl_command_queue command_queue;
	cl_device_id device_id;
	cl_kernel kernel;
	void clinit();
	void buildkernel();
	void checkErr(cl_int err, const char * name);
};

class CTracer
{
	CImage stars, disk;
	double mass, raddisk, radhole;
	double coeff;
	double dtime;
    uint diskrad;

public:
    void RenderImage(int xRes, int yRes, COpenCLmanager &clmanager);
    void SaveImageToFile(std::string fileName);
    CImage* LoadImageFromFile(std::string fileName);
    uint GetInfoSize() {return (6 + 10 + 4 + 3); }
    void CreateInfo(double *info);
public:
    SCamera m_camera;
    std::vector<SRay> rays;
};