#include "tracer2.h"
#include <cstdio>
#include <cmath>

using glm::dvec3;
using glm::dvec2;
using glm::uvec2;
using glm::length;

int main(int argc, char** argv)
{

        CTracer tracer;
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
        tracer.RenderImage(xRes, yRes);
        tracer.SaveImageToFile("../img/Result.png");

        return 0;
}