#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include "GLM.h"
#include "Disk.h"

unsigned int disk_vao;

using namespace glm;

void createDisk()
{
	
	vec4 vertices[NumVertices];
	vertices[0] = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 normals[NumVertices];
	normals[0] = vec3(0.0, 1.0, 0.0);

	//vec2 textures[NumVertices];
	//textures[0] = vec2(0.5, 0.5);

	GLuint indices[NumIndices];

	for (int i = 0; i < NumTriangles; i++) {

		float angle = radians((GLfloat)(i * 360 / NumTriangles));
		vertices[i + 1] = vec4(cos(angle), 0.0, sin(angle), 1.0);
		//textures[i + 1] = vec2(((1 + cos(angle)) / 2.0), ((1 + sin(angle)) / 2.0));

		if (i < NumTriangles - 1) {
			indices[i * 3 + 0] = 0;
			indices[i * 3 + 1] = i + 1;
			indices[i * 3 + 2] = i + 2;
		}

		if (i == NumTriangles - 1) {
			indices[i * 3 + 0] = 0;
			indices[i * 3 + 1] = i + 1;
			indices[i * 3 + 2] = 1;
		}

	}

	//Calculate the per-vertex normals
	for (int i = 0; i < NumVertices; i++) {
		normals[i] = vec3(0.0, 1.0, 0.0);
	}


	//Unitize the model
	float min_x, max_x, min_y, max_y, min_z, max_z;
	min_x = max_x = vertices[0].x;
	min_y = max_y = vertices[0].y;
	min_z = max_z = vertices[0].z;

	// Find the min and max values of the vertices
	for (int i = 0; i < NumVertices; i++) {
		if (vertices[i].x < min_x) min_x = vertices[i].x;
		if (vertices[i].x > max_x) max_x = vertices[i].x;
		if (vertices[i].y < min_y) min_y = vertices[i].y;
		if (vertices[i].y > max_y) max_y = vertices[i].y;
		if (vertices[i].z < min_z) min_z = vertices[i].z;
		if (vertices[i].z > max_z) max_z = vertices[i].z;
	}

	float center_x = (max_x + min_x) / 2;
	float center_y = (max_y + min_y) / 2;
	float center_z = (max_z + min_z) / 2;

	float width = abs(max_x - min_x);
	float height = abs(max_y - min_y);
	float depth = abs(max_z - min_z);

	float scale = max(depth, max(width, height));

	//Moving points to center of the screen
	for (int i = 0; i < NumVertices; i++) {
		vertices[i].x -= center_x;
		vertices[i].y -= center_y;
		vertices[i].z -= center_z;
	}

	//Scale the model to fit within the box whose width, height, and depth extend from -0.5 to 0.5
	for (int i = 0; i < NumVertices; i++) {
		vertices[i].x /= scale;
		vertices[i].y /= scale;
		vertices[i].z /= scale;
	}

	//Scale the mode to fit within the box whose width, height, and depth extend from -1.0 to 1.0
	for (int i = 0; i < NumVertices; i++) {
		vertices[i].x *= 2;
		vertices[i].y *= 2;
		vertices[i].z *= 2;
	}
	

	//ADD OBJECT METHOD
	glGenVertexArrays(1, &disk_vao);
	glBindVertexArray(disk_vao);

	unsigned int handle[3];
	glGenBuffers(3, handle);

	// Vertices in the model
	glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Normals in the model
	glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	// Texture coordinates in the model
	//glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures, GL_STATIC_DRAW);
	//glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(2);

	// Indices in the model
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

}

void drawDisk() {
	glBindVertexArray(disk_vao);
	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, NULL);
}





