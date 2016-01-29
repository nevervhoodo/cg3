#pragma once

#include "glm/glm.hpp"
#include "vector"

typedef unsigned int uint;
typedef unsigned char uchar;

struct SRay
{
  glm::dvec3 m_start;
  glm::dvec3 m_dir;
  SRay () {m_start=glm::dvec3(0.0,0.0,0.0);m_dir=glm::dvec3(0.0,0.0,0.0);}
};

struct SCamera
{
  glm::dvec3 m_pos;          // Camera position and orientation
  glm::dvec3 m_forward;      // Orthonormal basis
  glm::dvec3 m_right;
  glm::dvec3 m_up;

  glm::dvec2 m_viewAngle;    // View angles, rad
  glm::uvec2 m_resolution;  // Image resolution: w, h

  std::vector<glm::dvec3> m_pixels;  // Pixel array
  double disksize;
  void DefaultInit();
  void ParseSettings(FILE *);
private:
  inline void IgnoreLine(FILE *);
  void CheckBasis();
  void CheckSize();
  void CheckResolution(int,int);
};

struct SMesh
{
  std::vector<glm::dvec3> m_vertices;  // vertex positions
  std::vector<glm::uvec3> m_triangles;  // vetrex indices
};