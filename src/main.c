// TSBK07 Project
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include <math.h>
#include <stdio.h>
#include "loadobj.h"
#include "LoadTGA.h"


// Globals
// Data would normally be read from files

#define near 1.0
#define far 70.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

#define PI 3.1415

// Models
Model *model;

  
// Reference to shader program
GLuint program;

	

// Position variables

// Camera 
mat4 camera;
vec3 pos;
vec3 lookat;
vec3 rotnormal;

mat4 model_pos;
mat4 model_rot;
mat4 model_transform;


void init(void)
{
	GLfloat projectionMatrix[] = { 
		2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
		0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
		0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
		0.0f, 0.0f, -1.0f, 0.0f };

	
	
	// Load models
	model = LoadModelPlus("../models/bunnyplus.obj");
	
	

	// GL inits
	dumpInfo();
	glClearColor(0.2,0.2,0.5,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("project.vert", "project.frag");
	
	printError("init shader");

	// Upload projection matrix to GPU
    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);
	
	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	printError("init arrays");
}
void OnTimer(int value)

{

    glutPostRedisplay();

    glutTimerFunc(20, &OnTimer, value);

}

void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Camera coordinates
	pos.x = -30.0;
	pos.y = 20.0;
	pos.z = 15.0;
	lookat.x = 0.0; 
	lookat.y = 0.0; 
	lookat.z = 0.0;
	rotnormal.x = 0.0; 
	rotnormal.y = 1.0; 
	rotnormal.z = 0.0;
	camera = lookAtv(pos, lookat, rotnormal);

	glUniformMatrix4fv(glGetUniformLocation(program, "cameraMatrix"), 1, GL_TRUE, camera.m);

	// Set objects coordinates and upload the objects
	model_pos = T(0, -7, 0);
	model_rot = Ry(0.0);
	model_transform = Mult(model_pos, model_rot);  
	glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);	
	DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");


	printError("display");
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("TSBK07 project");
	glutDisplayFunc(display); 
	init ();
	glutTimerFunc(20, &OnTimer, 0);
	glutMainLoop();
	return 0;
}
