#include "tracer2.h"

#include <iostream>
#include <vector>
#include <cmath>

using namespace glm;
using namespace std;

#define ANTIAL 2.0
#define ALPHADEBUG true
bool teatime = false;

dvec3 CTracer::MakeSky (dvec3 ray_pos, double alpha)
{
        if (length(ray_pos)>PRECISION)
                ray_pos = normalize(ray_pos);
        else
                cout<<"sky"<<endl;
        double dphi,dteta;
        uint phi, teta;
        if (length(ray_pos)>PRECISION)
                ray_pos = normalize(ray_pos);
        else
                cout<<"tracer.cpp 15 normalize"<<endl;
        dphi = (atan2(ray_pos.x, ray_pos.y)+M_PI)/M_PI/2*(stars.width()-1);
        dteta = (asin (ray_pos.z)+M_PI/2)/M_PI*(stars.height()-1);
        phi = dphi;
        teta = dteta;
        uint8_t r = stars(phi, teta, 0);
        uint8_t g = stars(phi, teta, 1);
        uint8_t b = stars(phi, teta, 2);

        // if ((alpha + 1)<PRECISION)
        //     return dvec3(r,g,b) / 255.0;
        // else
        return dvec3(r,g,b) * (double)(alpha) / 255.0;
}

double CTracer::FoundDisk(SRay ray, dvec3 &color, double &alpha)
{
        double dt = -ray.m_start.z/ray.m_dir.z;
        double rad;
        dvec2 coord;
        dvec3 tmpcolor;
        uint i,j;
        if ((dt>PRECISION)&&(dt<=dtime))
        {
                ray.m_start += ray.m_dir*double(dt);
                rad = sqrt(ray.m_start.x*ray.m_start.x + 
                        ray.m_start.y*ray.m_start.y); 
                if (rad <= raddisk )
                {
                        coord = dvec2(ray.m_start.x,ray.m_start.y);
                        coord /= (double)raddisk;
                        coord *= (double)diskrad;
                        i = coord.x + diskrad;
                        j = coord.y + diskrad;
                        uint8_t r = disk(j, i, 0);
                        uint8_t g = disk(i, j, 1);
                        uint8_t b = disk(i, j, 2);
                        uint8_t a = disk(i, j, 3);
                        //color *= (1-alpha)
                        //tmpcolor = dvec3(r,g,b) * (1-alpha) / 255.0;
                        // alpha = (alpha-1)*a/255.0;
                        // color += dvec3(r,g,b) * alpha / 255.0;
                        // if ((alpha+1)<PRECISION)
                        // {
                        //     alpha = a/255.0;
                        //     color += dvec3(r,g,b) * double(alpha) / 255.0;
                        // }
                        // else
                        // {
                        //a = (1 - alpha)*a/255.0;
                        color += dvec3(r,g,b) * double(a*alpha) / 255.0 /255.0;
                        alpha *= (1 - a/255.0);
                        // }
                }
        }
        return -1;
}

double CTracer::BlackHole(SRay ray)
{
        double r1; //dist from hole to ray.start
        double r2; //dist from hole to ray.finish (m_fin)
        dvec3 m_fin = ray.m_start + ray.m_dir; //finish of ray
        r1 = sqrt(ray.m_start.x*ray.m_start.x + ray.m_start.y*ray.m_start.y 
                + ray.m_start.z*ray.m_start.z); 
        
        r2 = sqrt(m_fin.x*m_fin.x + m_fin.y*m_fin.y + m_fin.z*m_fin.z); 

        if (((r1>radhole)^(r2>radhole))||(r2<radhole))
        {
                return dtime/2.0;
        }
        else
            if (ALPHADEBUG)
            {
                if (length(r1 - radhole)< radhole*0.01)
                    return -2;
                else return -1;
            }
            else
                return -1;
}

void CTracer::MakeRay(uvec2 pixelPos, vector<SRay> *rays)
{
        double ppx = double(pixelPos.x) - m_camera.m_resolution.x / 2.0;
        double ppy = double(pixelPos.y) - m_camera.m_resolution.y / 2.0;
        dvec3 n_forw, n_up, n_right;
        
        n_up = normalize(m_camera.m_up);
        n_right = normalize(m_camera.m_right);
        uint c = 0;
        float d = float(1/ANTIAL);
        for (float i=d/2.0;i<1;i+=d)
        {
                for (float j=d/2.0;j<1;j+=d)
                {
                        double x = 2.0 * length(m_camera.m_forward) * 
                                tan(m_camera.m_viewAngle.x) / m_camera.m_resolution.x;
                        double y = 2.0 * length(m_camera.m_forward) * 
                                tan(m_camera.m_viewAngle.y) / m_camera.m_resolution.y;
                        x *= j + ppx;
                        y *= i + ppy;

                        (*rays)[c].m_start = m_camera.m_forward;
                        (*rays)[c].m_start += x * n_right;
                        (*rays)[c].m_start += y * n_up;

                        (*rays)[c].m_dir = normalize((*rays)[c].m_start);
                        (*rays)[c].m_start += m_camera.m_pos;
                        c++;
                }
        }
}

glm::dvec3 CTracer::TraceRay(SRay ray)
{
        double r, ht;
        double dt;
        bool early = true;
        teatime = false;
        double alpha = 1;
        dvec3 a,an,change;
        dvec3 color = dvec3(0.0,0.0,0.0);
        if (length(ray.m_dir)>PRECISION)
                ray.m_dir = normalize(ray.m_dir);
        else
                cout<<"tracer0"<<endl;
        ray.m_dir *= VC;
        for (;;)
        {
                r = length(ray.m_start);
                a = -coeff/r/r/r * ray.m_start;
                an = perp (a,ray.m_dir);
                change = dtime*ray.m_dir+an*double(dtime*dtime/2.0);

                if (early)
                        if (dot(ray.m_start,m_camera.m_forward)>PRECISION)
                                early = false;
                if ((!early)&&(length(dtime*an)<10000))
                {
                        color += MakeSky(ray.m_dir,alpha);
                        return color;
                }
                ray.m_start+=change;
                ray.m_dir+=dtime*an;
                if (length(ray.m_dir)>PRECISION)
                        ray.m_dir = normalize(ray.m_dir);
                else
                        cout<<"tracer"<<endl;
                ray.m_dir *= VC;

                ht = BlackHole(ray);
                if (ALPHADEBUG)
                    if ((ht + 2)<PRECISION)
                        teatime = true;
                dt = FoundDisk(ray,color,alpha);
                // if ((alpha>PRECISION)&&(fabsf(1-alpha)<PRECISION))
                //     cout<<"yes"<<endl;
                // if ((dt>PRECISION)&&(dt<dtime)&&((alpha-1)<PRECISION))
                // {
                //         return color;
                // }
                // if ((dt>PRECISION)&&(dt<dtime)&&(alpha<PRECISION))
                // {
                //         return color;
                // }
                if ((ht>PRECISION)&&(ht<dtime))
                {
                        return color;
                }
        }
        return color;
}

void CTracer::RenderImage(int xRes, int yRes)
{
        mass = 8.57e+36f; 
        radhole = GCONST*2.0*mass/VC/VC;
        coeff = GCONST*mass;
        raddisk = radhole * m_camera.disksize;
        dtime = 10.0;

        disk = CImage("data/disk_32.png");
        diskrad = fmin(disk.height(),disk.width())/2;
        stars = CImage("data/stars.jpg");
        
        rays.resize(ANTIAL*ANTIAL);
        for (uint c=0;c<ANTIAL*ANTIAL;c++)
            rays[c].m_start=rays[c].m_dir=dvec3(0.0,0.0,0.0);

        for (int i = 0; i < yRes; i++) {
                for (int j = 0; j < xRes; j++) {
                        MakeRay(uvec2(j, i),&rays);
                        m_camera.m_pixels[i * xRes + j] = dvec3(0.0,0.0,0.0);
                        uint size = ANTIAL*ANTIAL;
                        for (uint step=0;step<size;step++)
                                m_camera.m_pixels[i * xRes + j] +=
                                        TraceRay(rays[step]);
                        m_camera.m_pixels[i * xRes + j] /= size;
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