#include "tracer.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <CL/cl.h>

using namespace glm;
using namespace std;

#define DEBUG false
#define DEBUG2 false
#define ARAY false
#define ANTIAL 2.0
#define MAGICMAXSIZE 65632
#define MAGICMAXSIZE2 1.723025e+13

void CTracer::CreateInfo(double *info)
{
    info[0] = mass;
    info[1] = raddisk;
    info[2] = radhole;
    info[3] = coeff;
    info[4] = dtime;
    info[5] = diskrad;
    info[6] = m_camera.m_pos.x;
    info[7] = m_camera.m_pos.y;
    info[8] = m_camera.m_pos.z;
    info[9] = m_camera.m_forward.x;
    info[10] = m_camera.m_forward.y;
    info[11] = m_camera.m_forward.z;
    info[12] = m_camera.m_right.x;
    info[13] = m_camera.m_right.y;
    info[14] = m_camera.m_right.z;
    info[15] = m_camera.m_up.x;
    info[16] = m_camera.m_up.y;
    info[17] = m_camera.m_up.z;
    info[18] = m_camera.m_viewAngle.x;
    info[19] = m_camera.m_viewAngle.y;
    info[20] = m_camera.disksize;
    info[21] = m_camera.m_resolution.x;
    info[22] = m_camera.m_resolution.y;
}

void CTracer::RenderImage(int xRes, int yRes, COpenCLmanager &clmanager)
{
        mass = 8.57e+36f; 
        radhole = GCONST*2.0*mass/VC/VC;
        coeff = GCONST*mass;
        raddisk = radhole * m_camera.disksize;
        dtime = 10.0;
        
        disk = CImage("data/disk_32.png");
        diskrad = fmin(disk.height(),disk.width())/2;
        stars = CImage("data/stars.jpg");
        
        cl_mem starsobj = NULL;
        cl_mem diskobj = NULL;
        cl_mem memobj = NULL;
        cl_mem infoobj = NULL;
        cl_int err;
        if ((xRes>CL_DEVICE_MAX_WORK_GROUP_SIZE)||
                (yRes>CL_DEVICE_MAX_WORK_GROUP_SIZE))
        {
                cout << "too big picture is expected" <<endl;
                //exit (1);
        }
        if ((xRes*yRes)>MAGICMAXSIZE2)
        {
                cout << "too big picture is expected (next)" <<endl;
                exit (1);
        }
        int memLenth = xRes*yRes;
        vector<cl_double3> mem(memLenth);
        cl_uint4 *clstars =  (cl_uint4 *)malloc(sizeof(cl_uint4)*stars.height()*stars.width());
        uint h = stars.height();
        uint w = stars.width();
        for (uint i=0;i<h;i++)
            for (uint j=0;j<w;j++)
            {
                uint8_t r = stars(j, i, 0);
                uint8_t g = stars(j, i, 1);
                uint8_t b = stars(j, i, 2);
                clstars[(i*w+j)].s[0] = r;
                clstars[(i*w+j)].s[1] = g;
                clstars[(i*w+j)].s[2] = b;
                clstars[(i*w+j)].s[3] = 0;
            }
        
        cl_uint4 *cldisk =  (cl_uint4 *)malloc(sizeof(cl_uint4)*disk.height()*disk.width());
        uint hd = disk.height();
        uint wd = disk.width();
        for (uint i=0;i<hd;i++)
            for (uint j=0;j<wd;j++)
            {
                uint8_t r = disk(j, i, 0);
                uint8_t g = disk(j, i, 1);
                uint8_t b = disk(j, i, 2);
                uint8_t a = disk(j, i, 3);
                cldisk[(i*wd+j)].s[0] = r;
                cldisk[(i*wd+j)].s[1] = g;
                cldisk[(i*wd+j)].s[2] = b;
                cldisk[(i*wd+j)].s[3] = a;
            }


        starsobj = clCreateBuffer(clmanager.context, CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR, 
                sizeof(cl_uint4)*stars.height()*stars.width(), clstars, &err);
        clmanager.checkErr(err,"clCreateBuffer stars");
        diskobj = clCreateBuffer(clmanager.context, CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR, 
                sizeof(cl_uint4)*disk.height()*disk.width(), cldisk, &err);
        clmanager.checkErr(err,"clCreateBuffer disk");
        memobj = clCreateBuffer(clmanager.context, CL_MEM_READ_WRITE, 
                memLenth*sizeof(cl_double3), NULL, &err);
        clmanager.checkErr(err,"clCreateBuffer mem");
        double *info =  (double *)malloc(sizeof(cl_double)*GetInfoSize());
        CreateInfo(info);
        infoobj = clCreateBuffer(clmanager.context, CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR, 
                sizeof(cl_double)*GetInfoSize(), info, &err);
        clmanager.checkErr(err,"clCreateBuffer info");
        err = clSetKernelArg(clmanager.kernel, 0, sizeof(cl_mem),(void *)&memobj);
        clmanager.checkErr(err,"arg 0");
        err = clSetKernelArg(clmanager.kernel, 1, sizeof(cl_int),(void *) &xRes);
        clmanager.checkErr(err,"arg 1");
        err = clSetKernelArg(clmanager.kernel, 2, sizeof(cl_int),(void *) &yRes);
        clmanager.checkErr(err,"arg 2");
        err = clSetKernelArg(clmanager.kernel, 3, sizeof(cl_mem), (void *) &infoobj);
        clmanager.checkErr(err,"arg 3");
        err = clSetKernelArg(clmanager.kernel, 4, sizeof(cl_mem), (void *) &starsobj);
        clmanager.checkErr(err,"arg 4");
        err = clSetKernelArg(clmanager.kernel, 5, sizeof(cl_int),(void *) &h);
        clmanager.checkErr(err,"arg 5");
        err = clSetKernelArg(clmanager.kernel, 6, sizeof(cl_int),(void *) &w);
        clmanager.checkErr(err,"arg 6");
        err = clSetKernelArg(clmanager.kernel, 7, sizeof(cl_mem), (void *) &diskobj);
        clmanager.checkErr(err,"arg 7");
        err = clSetKernelArg(clmanager.kernel, 8, sizeof(cl_int),(void *) &hd);
        clmanager.checkErr(err,"arg 8");
        err = clSetKernelArg(clmanager.kernel, 9, sizeof(cl_int),(void *) &wd);
        clmanager.checkErr(err,"arg 9");

        size_t global_work_size[1] = { memLenth };
        err = clEnqueueNDRangeKernel(clmanager.command_queue, 
                clmanager.kernel, 1, NULL, global_work_size, NULL, 
                0, NULL, NULL);
        clmanager.checkErr(err,"clEnqueueNDRangeKernel");
        err = clEnqueueReadBuffer(clmanager.command_queue, memobj, CL_TRUE, 0, 
                memLenth * sizeof(cl_double3), mem.data(), 0, NULL, NULL);
        clmanager.checkErr(err,"clEnqueueReadBuffer");
        err = clFinish(clmanager.command_queue);
        clmanager.checkErr(err,"finish");

        for (int i = 0; i <xRes; i++) {
                for (int j = 0; j < yRes; j++) {
                        m_camera.m_pixels[i * xRes + j].x = fabsf(mem[(i * xRes + j)].s[0]);
                        m_camera.m_pixels[i * xRes + j].y = fabsf(mem[(i * xRes + j)].s[1]);
                        m_camera.m_pixels[i * xRes + j].z = fabsf(mem[(i * xRes + j)].s[2]);
                }
        }
        
}

void CTracer::SaveImageToFile(std::string fileName)
{
        int width = m_camera.m_resolution.x;
        int height = m_camera.m_resolution.y;

        CImage image(width, height, 1, 3);

        int i, j;
        int textureDisplacement = 0;
        for (i = 0; i < height; i++) {
                for (j = 0; j < width; j++) {
                        dvec3 color = m_camera.m_pixels[textureDisplacement + j];
                        
                        image(j, i, 0) = clamp(color[0], 0.0, 1.0) * 255.0;
                        image(j, i, 1) = clamp(color[1], 0.0, 1.0) * 255.0;
                        image(j, i, 2) = clamp(color[2], 0.0, 1.0) * 255.0;
                }

                textureDisplacement += width;
        }

        image.save(fileName.c_str());
}

CImage* CTracer::LoadImageFromFile(std::string fileName)
{
        try {
                CImage* pImage = new CImage(fileName.c_str());
                return pImage;
        } catch (...) {
                std::cerr << "Error opening file " << fileName << std::endl;
                return NULL;
        }
}