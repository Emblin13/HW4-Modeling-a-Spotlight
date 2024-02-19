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

	vec2 textures[NumVertices];
	textures[0] = vec2(0.5, 0.5);

	GLuint indices[NumIndices];

	for (int i = 0; i < NumTriangles; i++) {

		float angle = radians((GLfloat)(i * 360 / NumTriangles));
		vertices[i + 1] = vec4(cos(angle), 0.0, sin(angle), 1.0);
		textures[i + 1] = vec2(((1 + cos(angle)) / 2.0), ((1 + sin(angle)) / 2.0));

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



	//ADD OBJECT METHOD
	glGenVertexArrays(1, &disk_vao);
	glBindVertexArray(disk_vao);

	unsigned int handle[4];
	glGenBuffers(4, handle);

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
	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Indices in the model
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

}

void drawDisk() {
	glBindVertexArray(disk_vao);
	glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, NULL);

}





