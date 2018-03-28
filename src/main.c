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
#define FAR 500.0
#define RIGHT 0.5
#define LEFT -0.5
#define TOP 0.5
#define BOTTOM -0.5

#define HOR_SPEED 0.01
#define VERT_SPEED 0.01
#define ROT_SPEED 0.05

#define PI 3.1415

#define SIZE 20

// Models
Model *skybox;
Model *model;
Model *score;


// Texture
GLuint wallTex;
GLuint grassTex;
GLuint skyTex;
GLuint scoreTex;

// Reference to shader program
GLuint program;
GLuint program_sky;

// Position variables

// Camera 

mat4 camera;
vec3 camera_pos;
vec3 camera_rot;
vec3 camera_lookat;

// Models
mat4 sky_scale;
mat4 sky_transform;

//mat4 model_pos;
//mat4 model_rot;
//mat4 model_transform;
mat4 north_wall_pos;
mat4 east_wall_pos;
mat4 south_wall_pos;
mat4 west_wall_pos;
mat4 ground_pos;


void init(void)
{
	// Default camera position and frostum coordinates
    //mat4 projectionMatrix = frustum(LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR);
	mat4 projectionMatrix = perspective(90, 16.0/9.0, 0.1, 500);
	set_default_camera(&camera_pos, &camera_lookat, &camera_rot);
	
	// Load models
	model = LoadModelPlus("../models/square.obj");
	skybox = LoadModelPlus("../models/skybox.obj");
	score = LoadModelPlus("../models/can.obj");

	// Load textures 
	LoadTGATextureSimple("../models/grass.tga", &grassTex);
	LoadTGATextureSimple("../models/wall.tga", &wallTex);
	LoadTGATextureSimple("../models/SkyBox512.tga", &skyTex);
	LoadTGATextureSimple("../models/wall.tga", &scoreTex);

	// Bind texture to GL_TEXTURE
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, wallTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, scoreTex);


	// Link the texture unit by sending the id to the GPU
	
	/*glUseProgram(program_sky);
	glUniform1i(glGetUniformLocation(program_sky, "texUnit"), 0);
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "texUnit"), 1);*/

	// GL inits
	dumpInfo();
	glClearColor(0.2,0.2,0.5,0);
	//glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("project.vert", "project.frag");
	program_sky = loadShaders("sky.vert", "sky.frag");
	
	printError("init shader");

	// Upload projection matrix to GPU, by first selecting the porgram to use
	glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUseProgram(program_sky);
    glUniformMatrix4fv(glGetUniformLocation(program_sky, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);


	// Init default rotation :
	north_wall_pos = Ry(0.0);
	east_wall_pos = Mult(T(1, 0, 0),Ry(-PI/2));
	south_wall_pos = Mult(T(1, 0, 1), Ry(PI));
	west_wall_pos = Mult(T(0, 0, 1),Ry(PI/2));
	ground_pos = Mult(T(0, 0, 1), Rx(-PI/2));
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
	
	

	// Draw the sky
	sky_scale = S(70, 70, 70);
	mat4 camera_pos_moved = T(camera_pos.x, 0, camera_pos.z);
	sky_transform = Mult(camera_pos_moved, sky_scale);

	glUseProgram(program_sky);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	// Set the sky texture to the texUnit
	glUniform1i(glGetUniformLocation(program_sky, "texUnit"), 0);
	// Upload transformation and camera matricies and draw sky
	glUniformMatrix4fv(glGetUniformLocation(program_sky, "cameraMatrix"), 1, GL_TRUE, camera.m);
	glUniformMatrix4fv(glGetUniformLocation(program_sky, "transformMatrix"), 1, GL_TRUE,  sky_transform.m);
	DrawModel(skybox,program_sky, "in_vertex",  "in_normal", "in_texture");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	
	

	glUseProgram(program);
	// Upload camera matrix
	glUniformMatrix4fv(glGetUniformLocation(program, "cameraMatrix"), 1, GL_TRUE, camera.m);
	GLfloat pos[3] = {camera_pos.x, camera_pos.y, camera_pos.z};
	glUniform3fv(glGetUniformLocation(program, "cameraPosition"),  1, pos);
	

	for (int y = 1; y < SIZE - 1; ++y)
	{
		for (int x = 1; x < SIZE - 1; ++x)
		{
			if (has_ground(x, y))
			
			{
				// Draw ground
				glUniform1i(glGetUniformLocation(program, "texUnit"), 1);
				draw_square(x, y, ground_pos, model, program);

				// Use wall texture
				glUniform1i(glGetUniformLocation(program, "texUnit"), 2);
				
				// Draw walls
				
				if (wall_north(x, y)) draw_square(x, y, north_wall_pos, model, program);
				if (wall_east(x, y))  draw_square(x, y, east_wall_pos,  model, program);
				if (wall_south(x, y)) draw_square(x, y, south_wall_pos, model, program);
				if (wall_west(x, y))  draw_square(x, y, west_wall_pos,  model, program);

				// Show score objects
				glUniform1i(glGetUniformLocation(program, "texUnit"), 3);
				if (get_xy_cell(x, y) == 'S') draw_score(x, y, score, program);
				
				

			}
		}
	}
	

	


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
