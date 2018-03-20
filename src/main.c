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
#include "utils.h"


// Globals
// Data would normally be read from files

#define NEAR 1.0
#define FAR 700.0
#define RIGHT 0.5
#define LEFT -0.5
#define TOP 0.5
#define BOTTOM -0.5

#define HOR_SPEED 0.05
#define VERT_SPEED 0.05
#define ROT_SPEED 0.05

#define PI 3.1415

// Models
Model *model;

// Reference to shader program
GLuint program;

// Position variables

// Camera 

mat4 camera;
vec3 camera_pos;
vec3 camera_rot;
vec3 camera_lookat;

// Models

mat4 model_pos;
mat4 model_rot;
mat4 model_transform;

void init(void)
{
	// Default camera position and frostum coordinates
    mat4 projectionMatrix = frustum(LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR);
	set_default_camera(&camera_pos, &camera_lookat, &camera_rot);
	
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
    glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	
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
	
	// Camera coordinates, move the camera accoring to keyboard events
	move_camera(&camera_pos, &camera_lookat, &camera_rot, HOR_SPEED, ROT_SPEED, ROT_SPEED);
	camera = lookAtv(camera_pos, camera_lookat, camera_rot);

	glUniformMatrix4fv(glGetUniformLocation(program, "cameraMatrix"), 1, GL_TRUE, camera.m);

	// Set objects coordinates and upload the objects
	model_pos = T(0, 0, 0);
	model_rot = Ry(1.0);
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
