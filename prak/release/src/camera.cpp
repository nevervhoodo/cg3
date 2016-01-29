#include <cmath>
#include <iostream>
#include <string>

#include "glm/glm.hpp"
#include "vector"
#include "types.h"

#define PRECISION 0.000001

using namespace std;
using namespace glm;

inline void SCamera::IgnoreLine(FILE *f)
{
    int c='a';
    while (c!='\n')
        c=fgetc(f);
}

void SCamera::CheckBasis()
{

    if (dot(m_up, m_right)>PRECISION)
        throw 'a';
    if (dot(m_up, m_forward)>PRECISION)
        throw 'b';
    if (dot(m_forward, m_right)>PRECISION)
        throw 'c'; 
    //m_up = normalize (m_up);   
    //m_right = normalize (m_right);
}

void SCamera::CheckResolution(int x, int y)
{
    if ((x<PRECISION)||(y<PRECISION))
        throw "image size should be grater than zero\r\n";
}

void SCamera::CheckSize()
{
    if ((tan(m_viewAngle.y)/tan(m_viewAngle.x)-(m_resolution.x/m_resolution.y))
        >PRECISION)
    {
        m_resolution.x = (tan(m_viewAngle.y)/tan(m_viewAngle.x)-
        (m_resolution.x/m_resolution.y))*m_resolution.y;
        cout<<"Warning: the width is changed. Now it is "<<m_resolution.x<<endl;
    }
    m_up = m_up * dvec3(m_resolution.y,m_resolution.y,m_resolution.y);   
    m_right = m_right * dvec3(m_resolution.x,m_resolution.x,m_resolution.x);
}

void SCamera::DefaultInit()
{
    //Defaiul settings
    m_pos = dvec3(3e+11,0.0,2e+10);          // Camera position and orientation
    m_forward = dvec3(-20,0.0,0.0);      // Orthonormal basis
    m_right = dvec3(0.0,10.0,0.0);
    //cout<<m_right.x<<" "<<m_right.y<<" "<<m_right.z<<endl;
    m_up = dvec3(0.0,0.0,-10.0); 
    //cout<<m_up.x<<" "<<m_up.y<<" "<<m_up.z<<endl;
    m_viewAngle = dvec2(30.0/180.0*M_PI,30.0/180.0*M_PI);    // View angles, rad
    m_resolution = uvec2(512,512);  // Default resolutio
    m_pixels.resize(m_resolution.x*m_resolution.y);  // Pixel array
    disksize = 7;
}

void SCamera::ParseSettings(FILE *file)
{
/*
    Size of the output image is X*Y
    input x:
    input y:
    ---------------------------------
    Size of the disk: it is n times wider than black hole
    input n:
    ---------------------------------
    Camera position: {x,y,z}
    input x:
    input y:
    input z:
    ---------------------------------
    Camera orientation: up {x1,y1,z1}, right {x2,y2,z2}, forward {x3,y3,z4}
    input up: {,,}
    input down: {,,}
    input forward: {,,}
    ---------------------------------
    Camera angle (degrees): up, right
    input up:
    input right:
    */
    try
    {
        IgnoreLine(file);
        int n_x, n_y;
        double x,y,z;
        if(fscanf(file, "input x: %d\n", &n_x) == 1) 
            m_resolution.x = n_x;
        else
            throw 2;
        if(fscanf(file, "input y: %d\n", &n_y) == 1) 
            m_resolution.y = n_y;
        else
            throw 3;
        IgnoreLine(file);
        IgnoreLine(file);
        if(fscanf(file, "input n: %lf\n", &x) == 1) 
            disksize = x;
        else
            throw 6;
        IgnoreLine(file);
        IgnoreLine(file);
        if(fscanf(file, "input x: %le\n", &x) == 1) 
            m_pos.x = x;
        else
            throw 9;
        if(fscanf(file, "input y: %le\n", &y) == 1) 
            m_pos.y = y;
        else
            throw 10;
        if(fscanf(file, "input z: %le\n", &z) == 1) 
            m_pos.z = z;
        else
            throw 11;
        IgnoreLine(file);
        IgnoreLine(file);
        if(fscanf(file, "input up: {%le,%le,%le}\n", &x,&y,&z) == 3) 
            m_up = dvec3(x,y,z);
        else
            throw 14;
        if(fscanf(file, "input right: {%le,%le,%le}\n", &x,&y,&z) == 3) 
            m_right = dvec3(x,y,z);
        else
            throw 15;
        if(fscanf(file, "input forward: {%le,%le,%le}\n", &x,&y,&z) == 3) 
            m_forward = dvec3(x,y,z);
        else
            throw 16;
        CheckBasis();
        IgnoreLine(file);
        IgnoreLine(file);
        if(fscanf(file, "input up: %lf\n", &x) == 1) 
            if ((x<90)&&(x>PRECISION))
                m_viewAngle.y = x/180*M_PI;
            else throw "angle should be in (0..90)";
        else
            throw 19;
        if(fscanf(file, "input right: %lf\n", &y) == 1) 
            if ((y<90)&&(y>PRECISION))
                m_viewAngle.x = y/180*M_PI;
            else throw "angle should be in (0..90)";
        else
            throw 20;
        //CheckSize();
        m_pixels.resize(m_resolution.x*m_resolution.y);  // Pixel array
    }
    catch (char c)
    {
        DefaultInit();
        cout<<"Invalid config format! Using default parameters"<<endl<<
            "vectors "<</*{"<<v1.x<<";"<<v1.y<<";"<<v1.z<<"},{"<<v2.x<<";"<<v2.y<<";"<<
            v2.z<<"}*/"are not orthogonal"<<endl;
    }
    catch (int num) 
    {
        DefaultInit();
        cout<<"Invalid config format! Using default parameters"<<endl<<
            "Error in line "<<num<<endl;
    }
    catch (const char *s)
    {
        DefaultInit();
        cout<<"Invalid config format! Using default parameters "<<s<<endl;
    }
}