#include "tracer.h"

#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>

using namespace glm;
using namespace std;

#define DEBUG false
#define DEBUG2 false
#define ARAY false
#define ANTIAL 2.0


void print (dvec3 v)
{
    cout <<"vec is "<<v.x<<" "<<v.y<<" "<<v.z<<endl;
}

void print(SRay ray)
{
        cout<<"ray is "<<ray.m_start.x<<" "<<ray.m_start.y<<" "<<ray.m_start.z<<
        "; "<<ray.m_dir.x<<" "<<ray.m_dir.y<<" "<<ray.m_dir.z<<endl;
}

dvec3 CTracer::MakeSky (dvec3 ray_pos, double alpha)
{
        if (length(ray_pos)>PRECISION)
                ray_pos = normalize(ray_pos);
        else
                cout<<"sky"<<endl;
        if (DEBUG2)
                cout<<ray_pos.x<<" "<<
                ray_pos.y<<" "<<ray_pos.z<<endl;
        double dphi,dteta;
        uint phi, teta;
        if (length(ray_pos)>PRECISION)
                ray_pos = normalize(ray_pos);
        else
                cout<<"tracer.cpp 15 normalize"<<endl;

        dphi = (atan2(ray_pos.x, ray_pos.y)+M_PI)/M_PI/2*(stars.width()-1);
        //dphi = (atan2(ray_pos.x, ray_pos.y)+M_PI)*stars.width()/M_PI/2;
        dteta = (asin (ray_pos.z)+M_PI/2)/M_PI*(stars.height()-1);
        //dteta = (asin (ray_pos.z)+M_PI/2)*stars.height()/M_PI;
        phi = dphi;//*stars.width();
        teta = dteta;//*stars.height();
        if (DEBUG)
                cout<<dteta<<" "<<
                dphi<<" "<<teta<<" "<<phi<<
                " "<<endl;
        uint8_t r = stars(phi, teta, 0);
        uint8_t g = stars(phi, teta, 1);
        uint8_t b = stars(phi, teta, 2);
        //uint8_t a = stars(j, i, 3);
        return dvec3(r,g,b) * (1-alpha) / 255.0;
}

double CTracer::FoundDisk(SRay ray, dvec3 &color, double &alpha)
{
        double dt = -ray.m_start.z/ray.m_dir.z;
        double rad;
        dvec2 coord;
        uint i,j;
        if ((dt>PRECISION)&&(dt<=dtime))
        {
                //cout<<"here"<<endl;
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
                        //cout<<"i,j "<<i<<" "<<j<<" "<<diskrad<<endl;
                        uint8_t r = disk(j, i, 0);
                        uint8_t g = disk(i, j, 1);
                        uint8_t b = disk(i, j, 2);
                        // if ((!r)&&(!g)&&(!b))
                        //         return -1;
                        uint8_t a = disk(i, j, 3);
                        alpha = a/255.0;
                        color = dvec3(r,g,b) * alpha / 255.0;
                        //         return dt;
                        // if (a)
                        // {
                        //         //printf("ssssssssss\n");
                        //         color = dvec3(r,g,b) / 255.0;
                        //         return dt;
                        // }
                }
        }
        return -1;
}

double CTracer::BlackHole(SRay ray)
{
        //double b,c,d,ht;
        double r1,r2;
        // b = length (ray.m_dir) * length (ray.m_dir);//mb -length!!
        // c = dot(ray.m_dir,ray.m_dir) - radhole*radhole;
        // d = b*b - c;
        // if (d>=PRECISION)
        // {   
        //         //cout<<"black here"<<endl;
        //         ht = fmin((-b + sqrt(d)),(-b - sqrt(d)));
        //         if ((ht>-PRECISION)&&(ht<=dtime))
        //         {
        //                 cout<<"black surely here"<<endl;
        //                 return ht;
        //         }
        // }
        // return -1;
        dvec3 m_fin = ray.m_start + ray.m_dir;
        r1 = sqrt(ray.m_start.x*ray.m_start.x + ray.m_start.y*ray.m_start.y 
                + ray.m_start.z*ray.m_start.z);
        
        r2 = sqrt(m_fin.x*m_fin.x + m_fin.y*m_fin.y + m_fin.z*m_fin.z); 

        if (((r1>radhole)^(r2>radhole))||(r2<radhole))
        {
                //cout<<"black here"<<endl;
                return dtime/2.0;
        }
        else
                return -1;  
}

void CTracer::MakeRay(uvec2 pixelPos, vector<SRay> *rays)
{
        //cout<<"evrika"<<endl;
        double ppx = double(pixelPos.x) - m_camera.m_resolution.x / 2.0;
        double ppy = double(pixelPos.y) - m_camera.m_resolution.y / 2.0;
//coordinates from -size/2 to size/2
        //vector<SRay> rays;
        dvec3 n_forw, n_up, n_right;
        //SRay ray;
        
        n_up = normalize(m_camera.m_up);
        n_right = normalize(m_camera.m_right);
        //cout<<rays->size()<<endl;
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
        // return ray.m_dir;
        //cout<<"*"<<endl;
        double r, ht, dt;
        bool early = true;
        double alpha = 0;
        dvec3 a,an,change;
        dvec3 color = dvec3(0.0,0.0,0.0);
        if (length(ray.m_dir)>PRECISION)
                ray.m_dir = normalize(ray.m_dir);
        else
                cout<<"tracer0"<<endl;
        ray.m_dir *= VC;
        //for (int iter;iter<1000;iter++)
        long int kmax = 1e+10;
        //#pragma omp parallel for
        //for (long int k=0;k<kmax;k++)
        for (;;)
        {
                r = length(ray.m_start);
                a = -coeff/r/r/r * ray.m_start;
                if (ARAY)
                    cout<<"a= "<<a.x<<" "<<a.y<<" "<<a.z<<endl;
                an = perp (a,ray.m_dir);
                change = dtime*ray.m_dir+an*double(dtime*dtime/2.0);
                //cout<<length(dtime*an)<<endl;
                //cout <<dot(m_camera.m_pos,ray.m_start)<<endl;
                if (early)
                        if (dot(ray.m_start,m_camera.m_forward)>PRECISION)
                                early = false;
                if ((!early)&&(length(dtime*an)<10000))
                {
                        //cout << "happy"<<endl;
                        color += MakeSky(ray.m_dir,alpha);
                        //kmax = k;
                        return color;
                }
                ray.m_start+=change;
                ray.m_dir+=dtime*an;
                if (length(ray.m_dir)>PRECISION)
                        ray.m_dir = normalize(ray.m_dir);
                else
                        cout<<"tracer"<<endl;
                ray.m_dir *= VC;
                if (ARAY)
                    print(ray);

                ht = BlackHole(ray);
                dt = FoundDisk(ray,color,alpha);
                //ray.m_dir=normalize(ray.m_dir)*double(VC); 
                /*
                if (BlackHole(ray))
                        return color;
                if (FoundDisk(ray,color))
                        return color;  */
                if ((dt>PRECISION)&&(dt<dtime)&&(alpha<PRECISION))
                {
                        //kmax = k;
                        return color;
                }
                if ((ht>PRECISION)&&(ht<dtime))
                {
                        //kmax = k;
                        return color;
                }
                //         if ((dt>PRECISION)&&(dt<dtime))
                //                 if (dt<ht)
                //                 {
                //                         early = false;
                //                         return color;
                //                 }
                //                 else
                //                 {
                //                         early = false;
                //                         return dvec3(0.0,0.0,0.0);
                //                 }
                //         else
                //         {
                //                 early = false;
                //                 return dvec3(0.0,0.0,0.0);
                //         }
                // else if ((dt>PRECISION)&&(dt<dtime))
                //         return color;
                // else if (r > length(m_camera.m_pos)*2)
                //         return MakeSky(ray.m_dir);
        }
        return color;
        //return MakeSky(ray.m_dir);
}

void CTracer::RenderImage(int xRes, int yRes)
{
        // Reading input texture sample
        //CImage* pImage = LoadImageFromFile("data/disk_32.png");

        mass = 8.57e+36f; //default !!
        radhole = GCONST*2.0*mass/VC/VC;
        coeff = GCONST*mass;
        raddisk = radhole * m_camera.disksize;//!CONFIG
        dtime = 10.0;

        disk = CImage("data/disk_32.png");
        diskrad = fmin(disk.height(),disk.width())/2;
        stars = CImage("data/stars.jpg");
        
        rays.resize(ANTIAL*ANTIAL);
        for (uint c=0;c<ANTIAL*ANTIAL;c++)
            rays[c].m_start=rays[c].m_dir=dvec3(0.0,0.0,0.0);
        // SRay ray;
        // ray.m_start = dvec3(0.0,0.0,0.0);
        // ray.m_dir = dvec3(0.0,0.0,0.0);
        // for (uint c=0;c<ANTIAL2;c++)
        // {
        //         rays.push_back(ray);
        // }

        //uint antial = 2;
        /*for (int i = 0; i < img.height(); i++) { // Image lines
                for(int j = 0; j < img.width(); j++) { // Pixels in line
                        uint8_t r = img(j, i, 0);
                        uint8_t g = img(j, i, 1);
                        uint8_t b = img(j, i, 2);
                        uint8_t a = img(j, i, 3); // TODO: check!
                }
        }*/

        // Rendering
        //m_camera.m_resolution = uvec2(xRes, yRes);
        //m_camera.m_pixels.resize(xRes * yRes);

        // for (int i = 0; i < yRes; i++) {
        //         for (int j = 0; j < xRes; j++) {
        //                 SRay ray = MakeRay(uvec2(j, i));
        //                 m_camera.m_pixels[i * xRes + j] = TraceRay(ray);
        //                 //cout << m_camera.m_pixels[i * xRes + j]<<endl;
        //         }
        //         if (!(i%20))
        //                 cout<<"*"<<endl;
        // }
        //#pragma omp parallel for
        for (int i = 0; i < yRes; i++) {
                //#pragma omp parallel for
                for (int j = 0; j < xRes; j++) {
                        MakeRay(uvec2(j, i),&rays);
                        m_camera.m_pixels[i * xRes + j] = dvec3(0.0,0.0,0.0);
                        uint size = ANTIAL*ANTIAL;
                        #pragma omp parallel for
                        for (uint step=0;step<size;step++)
                                m_camera.m_pixels[i * xRes + j] +=
                                        TraceRay(rays[step]);
                        //cout << m_camera.m_pixels[i * xRes + j]<<endl;
                        m_camera.m_pixels[i * xRes + j] /= ANTIAL;
                }
                // if (!(i%20))
                //         cout<<"*"<<endl;
        }
}

void CTracer::SaveImageToFile(std::string fileName)
{
        int width = m_camera.m_resolution.x;
        int height = m_camera.m_resolution.y;

        CImage image(width, height, 1, 3);

        //unsigned char* imageBuffer = (unsigned char*)image.data();

        int i, j;
        //int imageDisplacement = 0;
        int textureDisplacement = 0;
        //#pragma omp parallel for
        for (i = 0; i < height; i++) {
                //#pragma omp parallel for
                for (j = 0; j < width; j++) {
                        dvec3 color = m_camera.m_pixels[textureDisplacement + j];
                        
                        image(j, i, 0) = clamp(color[0], 0.0, 1.0) * 255.0;
                        image(j, i, 1) = clamp(color[1], 0.0, 1.0) * 255.0;
                        image(j, i, 2) = clamp(color[2], 0.0, 1.0) * 255.0;
                        
                        /*
                        imageBuffer[imageDisplacement + j * 3] = 255.0f; //clamp(color.b, 0.0f, 1.0f) * 255.0f;
                        imageBuffer[imageDisplacement + j * 3 + 1] = clamp(color.g, 0.0f, 1.0f) * 255.0f;
                        imageBuffer[imageDisplacement + j * 3 + 2] = clamp(color.r, 0.0f, 1.0f) * 255.0f;
                        */
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