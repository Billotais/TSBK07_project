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
#include "simplefont.h"


// For optimization statistics
double total_cell = 0;
double total_count = 0;

// Models
Model *skybox;
Model *model;
Model *score;
Model *lever;
Model *flag;
Model *part;


// Textures
GLuint groundTex;
GLuint wallTex;
GLuint skyTex;
GLuint scoreTex;
GLuint doorTex;
GLuint leverTex;
GLuint objectiveTex;
GLuint flagTex;
GLuint particleTex;

// Bump map used for normal vectors
GLuint groundBumpTex;
GLuint wallBumpTex;
GLuint doorBumpTex;
GLuint objectiveBumpTex;

// Reference to shader program
GLuint program;
GLuint program_sky;

// Camera 
mat4 camera;
vec3 camera_pos;
vec3 camera_rot;
vec3 camera_lookat;

// Models
mat4 sky_scale;
mat4 sky_transform;

// Base values for different walls
mat4 north_wall_pos;
mat4 east_wall_pos;
mat4 south_wall_pos;
mat4 west_wall_pos;
mat4 ground_pos;

// Array of particles and spawn position of them
extern particle** particles;
extern int part_x, part_y;

void reshape(GLsizei w, GLsizei h)
{
	// Viewport is a separate setting
	glViewport(0, 0, w, h);
	sfSetRasterSize(w/2, h/2);
}

void init(void)
{
	 // init font and sound
	sfMakeRasterFont();
	init_sound();
	
	// Try to load the level
	if (load_level(0) != 0) exit(-1);

	// Default camera position and frostum coordinates
	mat4 projectionMatrix = perspective(90, 16.0/9.0, 0.01, 500);
	set_default_camera(&camera_pos, &camera_lookat, &camera_rot);
	
	// Load models
	model = LoadModelPlus("../models/square.obj");
	skybox = LoadModelPlus("../models/skybox.obj");
	score = LoadModelPlus("../models/box.obj");
	lever = LoadModelPlus("../models/key.obj");
	flag = LoadModelPlus("../models/trophy.obj");
	part = LoadModelPlus("../models/coin.obj");

	// Load textures and bump maps
	LoadTGASetMipmapping(true);
	LoadTGATextureSimple("../textures/ground_albedo.tga", &groundTex);
	LoadTGATextureSimple("../textures/wood_albedo.tga", &doorTex);
	LoadTGATextureSimple("../textures/wall_albedo.tga", &wallTex);
	LoadTGATextureSimple("../textures/skybox.tga", &skyTex);
	LoadTGATextureSimple("../textures/box.tga", &scoreTex);
	LoadTGATextureSimple("../textures/key.tga", &leverTex);
	LoadTGATextureSimple("../textures/plates_albedo.tga", &objectiveTex);
	LoadTGATextureSimple("../textures/gold.tga", &flagTex);
	LoadTGATextureSimple("../textures/gold.tga", &particleTex);

	LoadTGATextureSimple("../textures/wall_normal.tga", &wallBumpTex);
	LoadTGATextureSimple("../textures/wood_normal.tga", &doorBumpTex);
	LoadTGATextureSimple("../textures/ground_normal.tga", &groundBumpTex);
	LoadTGATextureSimple("../textures/plates_normal.tga", &objectiveBumpTex);

	


	// Bind texture to GL_TEXTURE
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, groundTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, wallTex);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, scoreTex);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, doorTex);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, leverTex);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, objectiveTex);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, flagTex);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, particleTex);


	// Bind bump maps to GL_TEXTURE
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, wallBumpTex);
	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, doorBumpTex);
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, groundBumpTex);
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, objectiveBumpTex);

	// GL inits
	dumpInfo();
	glClearColor(0.2,0.2,0.5,0);
	glutHideCursor();
	
	//glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader- Switch between first and third person view.
	program = loadShaders("project.vert", "project.frag");
	program_sky = loadShaders("sky.vert", "sky.frag");
	
	printError("init shader");

	// Upload projection matrix to GPU, by first selecting the porgram to use
	glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUseProgram(program_sky);
    glUniformMatrix4fv(glGetUniformLocation(program_sky, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);

	// We upload the list of light sources
	set_lights();

	// Init default rotation :
	north_wall_pos = Ry(0.0);
	east_wall_pos = Mult(T(1, 0, 0),Ry(-PI/2));
	south_wall_pos = Mult(T(1, 0, 1), Ry(PI));
	west_wall_pos = Mult(T(0, 0, 1),Ry(PI/2));
	ground_pos = Mult(T(0, 0, 1), Rx(-PI/2));

	printError("init arrays");
	
	// Allocate particles
	get_start_cell_position(&part_x, &part_y);
	allocate_particles(&particles, part_x + 0.5, part_y + 0.5);

}
void OnTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(10, &OnTimer, value);
}

void display(void)
{
	printError("pre display");

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Main call that will update the position of the player and the state of the maze
	update(&camera_pos, &camera_lookat, &camera_rot);
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

	// Upload camera matrix, aswell as camera position and direction for use in the shader
	glUniformMatrix4fv(glGetUniformLocation(program, "cameraMatrix"), 1, GL_TRUE, camera.m);
	GLfloat pos[3] = {camera_pos.x, camera_pos.y, camera_pos.z};
	glUniform3fv(glGetUniformLocation(program, "cameraPosition"),  1, pos);
	GLfloat dir[3] = {camera_lookat.x - camera_pos.x, camera_lookat.y - camera_pos.y, camera_lookat.z - camera_pos.z};
	glUniform3fv(glGetUniformLocation(program, "cameraOrientation"),  1, dir);
	
	// Go through each cell of the maze
	int count = 0;
	for (int y = 1; y < SIZE-1; ++y)
	{
		for (int x = 1; x < SIZE-1; ++x)
		{	
			if (has_ground(x, y))
			{
				if ((in_culling(x, y) || glutKeyIsDown('g')) && (is_flood(x, y) || glutKeyIsDown('f')))
				{
					count++;
					// Upload current cell data to GPU to optimise lightning
					glUniform1i(glGetUniformLocation(program, "x"), x);
					glUniform1i(glGetUniformLocation(program, "y"), y);
					// For the following draws, we will use the bump map
					if (glutKeyIsDown('n')) glUniform1i(glGetUniformLocation(program, "bumpMap"), false);
					else glUniform1i(glGetUniformLocation(program, "bumpMap"), true);

					// Draw ground
					glUniform1i(glGetUniformLocation(program, "texUnit"), 1);
					glUniform1i(glGetUniformLocation(program, "bumpUnit"), 12);
					if (get_xy_cell(x, y) != DOOR_OPEN && get_xy_cell(x, y) != START && get_xy_cell(x, y) != END) 
						draw_square(x, y, ground_pos, model, program);

					// Draw walls
					glUniform1i(glGetUniformLocation(program, "texUnit"), 2);
					glUniform1i(glGetUniformLocation(program, "bumpUnit"), 10);
					if (wall_north(x, y)) draw_square(x, y, north_wall_pos, model, program);
					if (wall_east(x, y))  draw_square(x, y, east_wall_pos,  model, program);
					if (wall_south(x, y)) draw_square(x, y, south_wall_pos, model, program);
					if (wall_west(x, y))  draw_square(x, y, west_wall_pos,  model, program);

					// Draw doors
					glUniform1i(glGetUniformLocation(program, "texUnit"), 4);
					glUniform1i(glGetUniformLocation(program, "bumpUnit"), 11);
					if (get_xy_cell(x+1, y) == DOOR_CLOSE) draw_square(x, y, east_wall_pos, model, program);
					if (get_xy_cell(x-1, y) == DOOR_CLOSE) draw_square(x, y, west_wall_pos, model, program);
					if (get_xy_cell(x, y+1) == DOOR_CLOSE) draw_square(x, y, south_wall_pos, model, program);
					if (get_xy_cell(x, y-1) == DOOR_CLOSE) draw_square(x, y, north_wall_pos, model, program);
					if (get_xy_cell(x, y) == DOOR_OPEN) draw_square(x, y, ground_pos, model, program);

					// Draw beggining/end cell
					if (get_xy_cell(x, y) == START || get_xy_cell(x, y) == END)
					{
						glUniform1i(glGetUniformLocation(program, "texUnit"), 6);
						glUniform1i(glGetUniformLocation(program, "bumpUnit"), 13);
						draw_square(x, y, ground_pos, model, program);
					}
					

					// Now we don't want to use bump mapping for props
					glUniform1i(glGetUniformLocation(program, "bumpMap"), false);

					// Draw the flag
					glUniform1i(glGetUniformLocation(program, "texUnit"), 7);
					if (get_xy_cell(x, y) == END || flag_picked())
						draw_flag(x, 0, y, flag, program, &camera_pos, &camera_lookat);
					
					// draw score objects
					glUniform1i(glGetUniformLocation(program, "texUnit"), 3);
					if (get_xy_cell(x, y) == SCORE) draw_score(x, y, score, program);

					// draw lever
					glUniform1i(glGetUniformLocation(program, "texUnit"), 5);
					if (get_xy_cell(x, y) == LEVER) draw_up_lever(x, y, lever, program);
					if (get_xy_cell(x, y) == LEVER_PRESSED) draw_down_lever(x, y, lever, program);

					// Draw gold fountain
					if (get_xy_cell(x, y) == START && flag_picked())
					{
						glUniform1i(glGetUniformLocation(program, "texUnit"), 8);
						simulate_particules(particles, part_x + 0.5, part_y + 0.5);
						draw_particles(particles, part, program);
					}
				}
				
			}
		}
	}
	// for demonstration purposes, show % of drawn cells

	total_cell += (double)count/4.41;
	total_count += 1;

	char stats[50];
	sprintf(stats, "%d/441 cells drawn, = %.2f%%", count, (double)count/4.41);
	sfDrawString(650, 20, stats);

	char stats_avg[50];
	sprintf(stats_avg, "Average : %.2f%%", total_cell/total_count);
	sfDrawString(650, 40, stats_avg);

	if (glutKeyIsDown('f')) sfDrawString(650, 60, "Flood algorithm disabled");
	if (!glutKeyIsDown('f')) sfDrawString(650, 60, "Flood algorithm enabled");
	if (glutKeyIsDown('g')) sfDrawString(650, 80, "Culling algorithm disabled");
	if (!glutKeyIsDown('g')) sfDrawString(650, 80, "Culling algorithm enabled");

	// Draw UI
	draw_text(&camera_pos);
	
	printError("display");
	glutSwapBuffers();
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1920, 1080);
	glutCreateWindow ("TSBK07 project");
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	init ();
	glutTimerFunc(10, &OnTimer, 0);

	glutMainLoop();
	return 0;
}
