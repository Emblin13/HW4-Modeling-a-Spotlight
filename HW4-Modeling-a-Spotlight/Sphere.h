#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <GL/freeglut.h>

#ifndef M_PI
#define M_PI 3.14159265f
#endif

#define GLM_FORCE_RADIANS 
#define TWOPI 2*3.141592653589793238//4626433832795

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective


using namespace glm;

extern unsigned int sphere_vao;

#define STEP 20
#define NumTriangles STEP*(STEP-1)*2
#define NumVertices (STEP+1)*(STEP+1)
#define NumIndices STEP*(STEP+1)*6


void createSphere();
void drawSphere();