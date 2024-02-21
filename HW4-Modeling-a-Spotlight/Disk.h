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

extern unsigned int disk_vao;

#define NumTriangles 4
#define NumVertices NumTriangles+1
#define NumIndices NumTriangles*3


void createDisk();
void drawDisk();