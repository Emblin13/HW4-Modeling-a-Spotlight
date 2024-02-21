#include <GL/glew.h>
#include "GLM.h"
#include "Sphere.h"

using namespace glm;

unsigned int sphere_vao;

void createSphere()
{
	vec4 vertices[NumVertices];
	vec3 normals[NumVertices];
	GLuint indices[NumIndices];
	vec2 textures[NumVertices];

	//Calculate the vertices, normals and texture coordinates
	float theta, phi;
	int i = 0, a = 0, k = 0, index = 0;
	for (double b = (-STEP / 2.0); b <= (STEP / 2.0); b++) {

		for (int a = 0; a <= STEP; a++) {

			phi = ((1.0 * a) / STEP) * 2 * M_PI;
			theta = ((1.0 * b) / STEP) * M_PI;

			vertices[i].x = cos(theta) * sin(phi);
			vertices[i].y = sin(theta);
			vertices[i].z = cos(theta) * cos(phi);
			vertices[i].w = 1.0f;

			normals[i] = normalize(vec3(vertices[i]));

			textures[i].x = (1.0 * a) / STEP;
			textures[i].y = ((1.0 * b) / STEP) + 0.5;

			i++;
		}
	}

	//Calculate the indices
	int index_one = 0, index_two = 0, index_three = 0, index_four = 0;
	for (int i = 0; i < (NumVertices - (STEP + 1)); i += STEP + 1) {

		for (int j = i; j < (i + STEP); j++) {

			index_one = j;
			index_two = j + STEP + 1;
			index_three = j + STEP + 2;
			index_four = j + 1;

			indices[index++] = index_one;
			indices[index++] = index_two;
			indices[index++] = index_three;

			indices[index++] = index_one;
			indices[index++] = index_three;
			indices[index++] = index_four;
		}

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
	glGenVertexArrays(1, &sphere_vao);
	glBindVertexArray(sphere_vao);

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

void drawSphere() {
    glBindVertexArray(sphere_vao);
    glDrawElements(GL_TRIANGLES, NumIndices, GL_UNSIGNED_INT, 0);
}