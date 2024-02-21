#include <GL/glew.h>
#include <math.h>
#include "GLM.h"
#include "Disk.h"
#include "Sphere.h"

mat4 view(1.0f);
GLuint view_matrix_loc;

mat4 model(1.0f);
GLuint model_matrix_loc;

mat4 projection(1.0f);
GLuint projection_matrix_loc;

GLuint program;
float aspect = 0.0;
GLfloat angle = 0.0;
GLuint vao;
unsigned int vbo[1];
GLuint ebo;

/* Class GLM object*/
GLMmodel* objmodel = NULL;
//vec3* modifiedVerts = NULL;
//vec3* modifiedNorms = NULL;

// vectors that the program came with

bool show_line = false;
bool isTopView = false;
bool isPhongShading = true;
bool isGouraudShading = false;

vec4 light_position(10.0, 10.0, 10.0, 1.0);
GLfloat timeUpdate = 0.0;
GLfloat vscale = 0.0;

vec4 ambient_product;
vec4 diffuse_product;
vec4 specular_product;

vec4 material_ambient(0.6, 0.5, 0.3, 1.0f);
vec4 material_diffuse(0.9, 0.5, 0.3, 1.0f);
vec4 material_specular(0.8, 0.8, 0.8, 1.0f);
float material_shininess = 50.0;

GLfloat eye[3] = { 0.0f, 5.0f, 30.5f };
//GLfloat eye[3] = { 0.0f, 15.0f, 50.5f };
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
void setUniform(GLuint p);
void unitizeModel();
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

	// Normalize the normals
	for (int i = 0; i < objmodel->numnormals; i++) {
		norms[i] = normalize(norms[i]);
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

	   unitizeModel();


	//program = initShaders("bunny_shader.vs", "bunny_shader.fs");
	//program = initShaders("phong_shader.vs", "phong_shader.fs");
	//program = initShaders("gouraud_shader.vs", "gouraud_shader.fs");

	if (isPhongShading) {
		program = initShaders("spot_phong.vs", "spot_phong.fs");
	}
	else {
		program = initShaders("spot_gouraud.vs", "spot_gouraud.fs");
	}

	glUseProgram(program);


	view_matrix_loc = glGetUniformLocation(program, "view");
	model_matrix_loc = glGetUniformLocation(program, "model");
	projection_matrix_loc = glGetUniformLocation(program, "projection");

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
	

	ambient_product = lightintensity*material_ambient;
	diffuse_product = lightintensity*material_diffuse;
	specular_product = lightintensity*material_specular;


	setUniform(program);


	createDisk();
	createSphere();
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void unitizeModel() {
	//Unitize the model
	float min_x, max_x, min_y, max_y, min_z, max_z;
	min_x = max_x = objmodel->vertices[0].x;
	min_y = max_y = objmodel->vertices[0].y;
	min_z = max_z = objmodel->vertices[0].z;

	// Find the min and max values of the vertices
	for (int i = 0; i < objmodel->numvertices; i++) {
		if (objmodel->vertices[i].x < min_x) min_x = objmodel->vertices[i].x;
		if (objmodel->vertices[i].x > max_x) max_x = objmodel->vertices[i].x;
		if (objmodel->vertices[i].y < min_y) min_y = objmodel->vertices[i].y;
		if (objmodel->vertices[i].y > max_y) max_y = objmodel->vertices[i].y;
		if (objmodel->vertices[i].z < min_z) min_z = objmodel->vertices[i].z;
		if (objmodel->vertices[i].z > max_z) max_z = objmodel->vertices[i].z;
	}

	float center_x = (max_x + min_x) / 2;
	float center_y = (max_y + min_y) / 2;
	float center_z = (max_z + min_z) / 2;

	float width = abs(max_x - min_x);
	float height = abs(max_y - min_y);
	float depth = abs(max_z - min_z);

	float scale = max(depth, max(width, height));

	//Moving points to center of the screen
	for (int i = 0; i < objmodel->numvertices; i++) {
		objmodel->vertices[i].x -= center_x;
		objmodel->vertices[i].y -= center_y;
		objmodel->vertices[i].z -= center_z;
	}

	//Scale the model to fit within the box whose width, height, and depth extend from -0.5 to 0.5
	for (int i = 0; i < objmodel->numvertices; i++) {
		objmodel->vertices[i].x /= scale;
		objmodel->vertices[i].y /= scale;
		objmodel->vertices[i].z /= scale;
	}

	//Scale the mode to fit within the box whose width, height, and depth extend from -1.0 to 1.0
	for (int i = 0; i < objmodel->numvertices; i++) {
		objmodel->vertices[i].x *= 2;
		objmodel->vertices[i].y *= 2;
		objmodel->vertices[i].z *= 2;
	}
}

/*******************************************************************************************/
void Display(void)
{
	// Clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);



	vec4 light_pos_new = view * light_position;
	glUniform4fv(glGetUniformLocation(program, "Spot.position"), 1, (GLfloat*)&light_pos_new[0]);

	vec3 spot_direction = mat3(view) * vec3(-light_position);
	glUniform1f(glGetUniformLocation(program, "Spot.exponent"), 30.0f);
	glUniform1f(glGetUniformLocation(program, "Spot.cutoff"), 15.0f);
	glUniform3fv(glGetUniformLocation(program, "Spot.direction"), 1, (GLfloat*)&spot_direction[0]);

	glUniform1i(glGetUniformLocation(program, "On"), 0);

	projection = glm::perspective(radians(45.0f), aspect,0.3f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (GLfloat*)&view[0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (GLfloat*)&projection[0]);

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
	view = glm::lookAt(vec3(eye[0], eye[1], eye[2]), glm::vec3(center[0], center[1], center[2]), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view[0]);

	//Set top view
	if (isTopView) {
		view = lookAt(vec3(0.0f, 25.0f, 0.0), vec3(0.0, 0.0, 0.0), vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, (GLfloat*)&view[0]);
	}

	//Draws the bunny
	model = mat4(1.0f);
	model = scale(model, vec3(7.0f, 7.0f, 7.0f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	//glDrawElements(GL_TRIANGLES, objmodel->numindices, GL_UNSIGNED_INT, NULL);
	

	//Draws the disk
	model = mat4(1.0f);
	model = translate(model, vec3(0.0f, -7.0f, 0.0f));
	model = scale(model, vec3(10.0f, 0.0f, 10.0f));
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	drawDisk();


	//Draws the sphere
	model = mat4(1.0f);
	model = translate(model, vec3(light_position.x, light_position.y, light_position.z));
	glUniform1i(glGetUniformLocation(program, "On"), 1); //Enables full-model lighting. If any normals have light shining on them, the entire object lights up.
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (GLfloat*)&model[0]);
	drawSphere();

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

		if (isPhongShading) glutSetWindowTitle("Modeling a Spot Light - Phong Shading");
		else glutSetWindowTitle("Modeling a Spot Light - Gouraud Shading");

		Initialize();
		break;
}
	glutPostRedisplay();
}
/**************************************************************************************/




void setUniform(GLuint p) {

	glUseProgram(p);
	glUniform4fv(glGetUniformLocation(p, "AmbientProduct"), 1, (GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(p, "DiffuseProduct"), 1, (GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(p, "SpecularProduct"), 1, (GLfloat*)&specular_product[0]);
	glUniform4fv(glGetUniformLocation(p, "Spot.intensity"), 1, (GLfloat*)&lightintensity[0]);

	glUniform4fv(glGetUniformLocation(p, "Spot.position"), 1, (GLfloat*)&light_position[0]);
	glUniform1f(glGetUniformLocation(p, "Shininess"), material_shininess);


}


//This updates at the same rate needed for the assignment, just done in more increments so it's smoother.
void delay(int n) {

	angle += 0.5f;

	rotateLight();

	glutPostRedisplay();

	glutTimerFunc(10, delay, n);

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
	glutTimerFunc(10, delay, 0);
	glutMainLoop();
	return 0;
}
/******************************************************************************************************************/




