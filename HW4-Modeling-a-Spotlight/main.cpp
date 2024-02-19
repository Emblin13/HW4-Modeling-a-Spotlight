#include <GL/glew.h>
#include <math.h>
#include "GLM.h"
#include "Disk.h"

mat4 view_matrix(1.0f);
GLuint view_matrix_loc;

mat4 model_matrix(1.0f);
GLuint model_matrix_loc;

mat4 projection_matrix(1.0f);
GLuint projection_matrix_loc;

GLuint program;
float aspect = 0.0;
GLfloat angle = 0.0;
GLuint vao;
unsigned int vbo[1];
GLuint ebo;

/* Class GLM object*/
GLMmodel* objmodel = NULL;
vec3* modifiedVerts = NULL;
vec3* modifiedNorms = NULL;

// vectors that the program came with

bool show_line = false;
bool isTopView = false;
bool isPhongShading = true;
bool isGouraudShading = false;

vec4 light_position(10.0, 10.0, 10.0, 1.0);  // directional light source
GLfloat timeUpdate = 0.0;
GLfloat vscale = 0.0;

vec4 ambient_product;
vec4 diffuse_product;
vec4 specular_product;

vec4 material_ambient(0.6, 0.5, 0.3, 1.0f);
vec4 material_diffuse(0.9, 0.5, 0.3, 1.0f);
vec4 material_specular(0.8, 0.8, 0.8, 1.0f);

GLfloat eye[3] = { 0.0f, 5.0f, 30.5f };
GLfloat center[3] = { 0.0f, 0.0f, 0.0f };

vec4 lightintensity = vec4(0.9f, 0.9f, 0.9f, 1.0f);

GLboolean update = false;
GLboolean changeWave = false;

void Initialize();
void Display(void);
void Reshape(int width, int height);
void rotateLight();
void delay(int n);

GLchar* ReadFile(const char* filename);
GLuint initShaders(const char* v_shader, const char* f_shader);
/*************************************************************************************/
GLchar* ReadFile(const char* filename) {
	FILE* infile;
#ifdef WIN32
	fopen_s(&infile, filename, "rb");
#else
	infile = fopen(filename, "r");
#endif
	if (!infile) {
		printf("Unable to open file %s\n", filename);
		return NULL;
	}

	fseek(infile, 0, SEEK_END);
	int len = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	GLchar* source = (GLchar*)malloc(len + 1);
	fread(source, 1, len, infile);
	fclose(infile);
	source[len] = 0;

	return (GLchar*)(source);
}

GLuint initShaders(const char *v_shader, const char *f_shader) {

	GLuint p = glCreateProgram();
	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

	const char * vs = ReadFile(v_shader);
	const char * fs = ReadFile(f_shader);

	glShaderSource(v, 1, &vs, NULL);
	glShaderSource(f, 1, &fs, NULL);
	free((char*)vs);
	free((char*)fs);

	glCompileShader(v);
	GLint compiled;
	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(len + 1);
		glGetShaderInfoLog(v, len, &len, log);
		printf("Vertex Shader compilation failed: %s\n", log);
		free(log);
	}

	glCompileShader(f);

	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLsizei len;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(len + 1);
		glGetShaderInfoLog(f, len, &len, log);
		printf("Fragment Shader compilation failed: %s\n", log);
		free(log);
	}

	glAttachShader(p, v);
	glAttachShader(p, f);
	glLinkProgram(p);

	GLint linked;
	glGetProgramiv(p, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLsizei len;
		glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = (GLchar*)malloc(len + 1);
		glGetProgramInfoLog(p, len, &len, log);
		printf("Shader linking failed: %s\n", log);
		free(log);
	}
	glUseProgram(p);
	return p;
}


void updateVertexNormals(vec3* vertices, vec3* norms) {
	vec3 v1, v2, v3, n;

	for (int i = 0; i < objmodel->numnormals; i++) {
		norms[i] = vec3(0.0, 0.0, 0.0);
	}

	// Calculate the normals for each vertex
	for (int i = 0; i < objmodel->numindices; i += 3) {
		v1 = vertices[objmodel->indices[i + 0]];
		v2 = vertices[objmodel->indices[i + 1]];
		v3 = vertices[objmodel->indices[i + 2]];

		n = normalize(cross(v2 - v1, v3 - v1));

		norms[objmodel->indices[i]] += n;
		norms[objmodel->indices[i + 1]] += n;
		norms[objmodel->indices[i + 2]] += n;
	}

	// Normalize the normals and flip them)
	for (int i = 0; i < objmodel->numnormals; i++) {
		norms[i] = -normalize(norms[i]);
	}
}


void Initialize(void){
	// Create the program for rendering the model
	
	   if (!objmodel){
		
		    objmodel = glmReadOBJ("bunny.obj");
		
			if (!objmodel){
				printf("OBJ file does not exist \n");
				exit(0);
			}
		updateVertexNormals(objmodel->vertices, objmodel->normals);
	   }

	// Create and compile our GLSL program from the shaders

	program = initShaders("bunny_shader.vs", "bunny_shader.fs");
	//program = initShaders("phong_shader.vs", "phong_shader.fs");

	view_matrix_loc = glGetUniformLocation(program, "view_matrix");
	model_matrix_loc = glGetUniformLocation(program, "model_matrix");
	projection_matrix_loc = glGetUniformLocation(program, "projection_matrix");
	
	glUseProgram(program);
	view_matrix = glm::lookAt(vec3(0.0f, 1.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	projection_matrix = mat4(1.0f);

	// Fill up the buffers!
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint offset = 0;
	glGenBuffers(1, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * (objmodel->numvertices) + sizeof(vec3) * (objmodel->numvertices), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * (objmodel->numvertices), objmodel->vertices);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position
	
	offset += sizeof(vec3) * (objmodel->numvertices);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * (objmodel->numvertices), objmodel->normals);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(vec3) * (objmodel->numvertices)) );
	glEnableVertexAttribArray(1);  // Vertex normal

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*(objmodel->numindices), objmodel->indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	
	float material_shininess = 50.0;
	ambient_product = lightintensity*material_ambient;
	diffuse_product = lightintensity*material_diffuse;
	specular_product = lightintensity*material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, (GLfloat*)&light_position[0]);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);






	glUniform1f(glGetUniformLocation(program, "Spot_exponent"), 30.0f); 
	glUniform1f(glGetUniformLocation(program, "Spot_cutoff"), 15.0f);

	glUniform3fv(glGetUniformLocation(program, "Spot_direction"), 1, (GLfloat*)&angle);






	modifiedVerts = (vec3*)malloc(sizeof(vec3) * (objmodel->numvertices));
	modifiedNorms = (vec3*)malloc(sizeof(vec3) * (objmodel->numvertices));

	createDisk();

	glClearColor(0.8, 0.8, 0.8, 1.0);
}
/*******************************************************************************************/
void Display(void)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	projection_matrix = glm::perspective(radians(45.0f), aspect,0.3f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (GLfloat*)&view_matrix[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (GLfloat*)&projection_matrix[0]);

	
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	if (show_line)
	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	if (isPhongShading) {
		//program = initShaders("phong_shader.vs", "phong_shader.fs");
	}
	else {
		//program = initShaders("gouraud_shader.vs", "gouraud_shader.fs");
	}


	//Set normal view
	view_matrix = glm::lookAt(vec3(eye[0], eye[1], eye[2]), glm::vec3(center[0], center[1], center[2]), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);

	//Set top view
	if (isTopView) {
		view_matrix = lookAt(vec3(0.0f, 25.0f, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view_matrix[0]);
	}



	//Draws the bunny
	model_matrix = mat4(1.0f);
	model_matrix = scale(model_matrix, vec3(7.0f, 7.0f, 7.0f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	glDrawElements(GL_TRIANGLES, objmodel->numindices, GL_UNSIGNED_INT, NULL);
	

	//Draws the disk
	model_matrix = mat4(1.0f);
	model_matrix = translate(model_matrix, vec3(0.0f, -5.0f, 0.0f));
	model_matrix = scale(model_matrix, vec3(10.0f, 0.0f, 10.0f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model_matrix[0]);
	drawDisk();

	glBindVertexArray(0);
	
	glutSwapBuffers();
}

/******************************************************************************************/
void Reshape(int width, int height){
	glViewport(0, 0, width, height);
	aspect = float(width) / float(height);
}

/**************************************************************************************/

void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 'q':case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'u':case 'U':
		show_line = !show_line;
		break;
	case 't':case 'T':
		isTopView = !isTopView;
		break;
	case 's':case 'S':
		isPhongShading = !isPhongShading;
		isGouraudShading = !isGouraudShading;
		break;
}
	glutPostRedisplay();
}
/**************************************************************************************/

void delay(int n) {

	angle += 5.0f;

	rotateLight();

	glutPostRedisplay();

	glutTimerFunc(100, delay, n);

}

void rotateLight() {

	light_position.x = cos(radians(angle)) * 10.0f;
	light_position.z = sin(radians(angle)) * 10.0f;

	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, (GLfloat*)&light_position[0]);
}

/****************************************************************************************/
int main(int argc, char** argv){
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	
	glutCreateWindow("Modeling a Spot Light - Phong Shading");

	if (glewInit()){
		printf("Unable to initialize GLEW ... exiting\n");
	}
	
	Initialize();
	printf("%s\n", glGetString(GL_VERSION));
	glutDisplayFunc(Display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(Reshape);
	glutTimerFunc(100, delay, 0);
	glutMainLoop();
	return 0;
}
/******************************************************************************************************************/




