#pragma once

#include "glm/glm.hpp"
#include "glm/gtx/perpendicular.hpp"
#include "glmextra.hpp"
#include "types.h"
#include "scene.h"

#include "string"
#include <vector>
#include "CImg.h"

#define PRECISION 0.000001
#define NOCHANGE 0.01
#define GCONST 6.67384e-11
#define VC 3.0e+8

//#define SANTI (AL*AL)

typedef cimg_library::CImg<unsigned char> CImage;

class CTracer
{
	CImage stars, disk;
	double mass, raddisk, radhole;
	double coeff;
	double dtime;
        uint diskrad;
public:
        //SRay MakeRay(glm::uvec2 pixelPos);  // Create ray for specified pixel
        void MakeRay(glm::uvec2 pixelPos, std::vector<SRay> *rays);
        glm::dvec3 TraceRay(SRay ray); // Trace ray, compute its color
        void RenderImage(int xRes, int yRes);
        void SaveImageToFile(std::string fileName);
        CImage* LoadImageFromFile(std::string fileName);
        glm::dvec3 MakeSky (glm::dvec3 ray_pos,double alpha = 1);
        double FoundDisk(SRay ray, glm::dvec3 &color,double &alpha);
        double BlackHole(SRay ray);
public:
	// double len_forward;
        SCamera m_camera;
        CScene* m_pScene;
        std::vector<SRay> rays;
};