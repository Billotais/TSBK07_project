#include "utils.h"


#define PI 3.141592
#define SIZE 21
// X are solid walls
// x are not solid walls
// 0 are empty cells
// S is a score object
// B is the beggining cell
// E is the end cell
// D is a door 
// L is an interruptor

#define WOBBLE_HEIGHT 30
#define WOBBLE_SPEED 0.2

int SCORE = 0;
int FLAG_PICKED = 0;

int current_level = 0;

// Same program as in main.c, used to uplaod the lights to the GPU
GLuint program;

double camera_bump_evolution = 0;

char mazearray[SIZE][SIZE];


void set_default_camera(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot)
{
	for (int x = 0; x < SIZE; ++x)
	{
		for (int y = 0; y < SIZE; ++y)
		{
			// Find the start cell
			if (get_xy_cell(x, y) == 'B') 
			{
				// Set the position
				*camera_pos = SetVector(x+0.5, 0.5, y+0.5);

				// Set the looking direction depending on the surounding walls
				vec3 direction;
				if 		(!wall_north(x, y) && !door_north(x, y)) direction = SetVector(0, 0, -3);
				else if (!wall_east(x, y)  && !door_east(x, y))  direction = SetVector(3, 0, 0);
				else if (!wall_south(x, y) && !door_south(x, y)) direction = SetVector(0, 0, 3);
				else  direction = SetVector(-3, 0, 0);
				*camera_lookat = VectorAdd(*camera_pos, direction);

				*camera_rot = SetVector(0.0, 1.0, 0.0);
			}
			
		}
	}
} 

void update(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot, float horizontal_speed, float rotation_speed, float vertical_speed)
{
	// Check if we are on a lever before moving
	int was_on_lever = (get_xy_cell(camera_pos->x, camera_pos->z) == 'L' ||
						get_xy_cell(camera_pos->x, camera_pos->z) == 'l');

	int old_x = camera_pos->x;
	int old_y = camera_pos->z;

	// Used for the camera wobble effect
	double previous_camera_bump_evolution = camera_bump_evolution;

	if (glutKeyIsDown('w')) // Go forward
	{
		vec3 move = ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed);
		*camera_pos = VectorAdd(*camera_pos, SetVector(move.x, 0, move.z));
		*camera_lookat = VectorAdd(*camera_lookat, SetVector(move.x, 0, move.z));
		camera_bump_evolution+=WOBBLE_SPEED;
	}
	if (glutKeyIsDown('s')) // Go backward
	{
		vec3 move = ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed);
		*camera_pos = VectorAdd(*camera_pos, SetVector(-move.x, 0, -move.z));
		*camera_lookat = VectorAdd(*camera_lookat, SetVector(-move.x, 0, -move.z));
		camera_bump_evolution+=WOBBLE_SPEED;
	}
	if (glutKeyIsDown('a')) // Go left
	{
		vec3 move = MultVec3(Ry(PI/2),ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed));
		*camera_pos = VectorAdd(*camera_pos, SetVector(move.x, 0, move.z));
		*camera_lookat = VectorAdd(*camera_lookat, SetVector(move.x, 0, move.z));
		camera_bump_evolution+=WOBBLE_SPEED;
	}
	if (glutKeyIsDown('d')) // Go right
	{
		vec3 move = MultVec3(Ry(PI/2),ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed));
		*camera_pos = VectorAdd(*camera_pos, SetVector(-move.x, 0, -move.z));
		*camera_lookat = VectorAdd(*camera_lookat, SetVector(-move.x, 0, -move.z));
		camera_bump_evolution+=WOBBLE_SPEED;
	}
	if (glutKeyIsDown(GLUT_KEY_RIGHT)) // rotate right
	{
		
		vec3 cam_to_lookpoint = VectorSub(*camera_lookat,*camera_pos);
		vec3 new_cam_to_lookpoint = MultVec3(Ry(-rotation_speed), cam_to_lookpoint);
		*camera_lookat = VectorAdd(*camera_pos, new_cam_to_lookpoint); 
		*camera_rot = MultVec3(Ry(-rotation_speed), *camera_rot);
	}
	if (glutKeyIsDown(GLUT_KEY_LEFT)) // Rotate left
	{
		
		vec3 cam_to_lookpoint = VectorSub(*camera_lookat,*camera_pos);
		vec3 new_cam_to_lookpoint = MultVec3(Ry(rotation_speed), cam_to_lookpoint);
		*camera_lookat = VectorAdd(*camera_pos, new_cam_to_lookpoint); 
		*camera_rot = MultVec3(Ry(rotation_speed), *camera_rot);
	}
	if (glutKeyIsDown(GLUT_KEY_UP)) // Go up
	{
		mat4 vertical_rotate = ArbRotate(CrossProduct(VectorSub(*camera_lookat,*camera_pos), *camera_rot), rotation_speed);
		
		vec3 cam_to_lookpoint = VectorSub(*camera_lookat,*camera_pos);
		vec3 new_cam_to_lookpoint = MultVec3(vertical_rotate, cam_to_lookpoint);
		*camera_lookat = VectorAdd(*camera_pos, new_cam_to_lookpoint); 
		*camera_rot = MultVec3(vertical_rotate, *camera_rot);
		 
	}
	if (glutKeyIsDown(GLUT_KEY_DOWN)) // Go down
	{
		mat4 vertical_rotate = ArbRotate(CrossProduct(VectorSub(*camera_lookat,*camera_pos), *camera_rot), -rotation_speed);
		
		vec3 cam_to_lookpoint = VectorSub(*camera_lookat,*camera_pos);
		vec3 new_cam_to_lookpoint = MultVec3(vertical_rotate, cam_to_lookpoint);
		*camera_lookat = VectorAdd(*camera_pos, new_cam_to_lookpoint); 
		*camera_rot = MultVec3(vertical_rotate, *camera_rot);
		
	}

	// Add woble to camera
	*camera_pos = MultVec3(T(0, (sin(camera_bump_evolution) - sin(previous_camera_bump_evolution))/WOBBLE_HEIGHT, 0), *camera_pos);
	*camera_lookat = MultVec3(T(0, (sin(camera_bump_evolution) - sin(previous_camera_bump_evolution))/WOBBLE_HEIGHT, 0), *camera_lookat);

	// Check different actions
	enable_lever(camera_pos);
	pickup_score(camera_pos);
	check_flag(camera_pos, camera_lookat, camera_rot);

	// If we left a lever cell, we reset the lever to be enable again
	if (was_on_lever && get_xy_cell(camera_pos->x, camera_pos->z) != 'l')
	{
		set_xy_cell(old_x, old_y, 'L');
	}
	
	// Check collisions if enabled, can disbale them for debuggin purposes
	if (!glutKeyIsDown('q'))
	{
		check_position(camera_pos, camera_lookat);
		check_corner(camera_pos,camera_lookat);
	}
	
}
//Ceck if too close to wall.
void check_position(vec3 *camera_pos, vec3 *camera_lookat)
{
	
	int walle, wallw, walls, walln;

	float x1 = floor(camera_pos->x);
	float y1 = floor(camera_pos->z);
	int x2 = (int)x1;
	int y2 = (int)y1;

	x1=x1-camera_pos->x;
	y1=y1-camera_pos->z;

	//make it in to an absolute number
	if (x1<0) x1=-1*x1;
	if (y1<0) y1=-1*y1;

	//check adjacent walls
	walle = wall_east(x2, y2)  || door_east(x2, y2);
	wallw = wall_west(x2, y2)  || door_west(x2, y2);
	walls = wall_south(x2, y2) || door_south(x2, y2);
	walln = wall_north(x2, y2) || door_north(x2, y2);

	//check each wall in turn

	double old_x = camera_pos->x; 
	double old_y = camera_pos->z;

	// Correct position of too close
    if (walle && x1>0.9){
        camera_pos->x = floor(camera_pos->x) + 0.9;
		camera_lookat->x += (camera_pos->x - old_x);
    }
    if (wallw && x1<0.1){
        camera_pos->x = floor(camera_pos->x) + 0.1;
		camera_lookat->x += (camera_pos->x - old_x);
    }
    if (walls && y1>0.9){
        camera_pos->z = floor(camera_pos->z) + 0.9;
		camera_lookat->z += (camera_pos->z - old_y);
    }
    if (walln && y1<0.1){
        camera_pos->z = floor(camera_pos->z) + 0.1;
		camera_lookat->z += (camera_pos->z - old_y);
    } 
}

void check_corner(vec3 *camera_pos, vec3 *camera_lookat)
{
	float x = floor(camera_pos->x);
	float y = floor(camera_pos->z);
	int xp =(int)x+1;
	int xm = (int)x-1;
	int yp = (int)y+1;
	int ym = (int)y-1;
	x = x - camera_pos->x;
	y = y - camera_pos->z;
	//make it in to an absolute number
	if (x<0) x=-1*x;
	if (y<0) y=-1*y;
	double old_x = camera_pos->x;
	double old_y = camera_pos->z;
	//The different corner positions.
	// 12
	// 34
	int corner1=check_wall(xp,yp);
	int corner2=check_wall(xm,yp);
	int corner3=check_wall(xm,ym);
	int corner4=check_wall(xp,ym);
	//No doors in corners.

	// Check the four different corners
    if(corner1 && x>0.9 && y>0.9 )
	{
		if (x > y)
		{
			camera_pos->z = floor(camera_pos->z) + 0.9;
			camera_lookat->z += (camera_pos->z - old_y);
		}
        else
		{
			camera_pos->x = floor(camera_pos->x) + 0.9;
			camera_lookat->x += (camera_pos->x - old_x);
		}
    }

    else if(corner2 && x<0.1 && y>0.9 )
	{
		if ((1-x) > y)
		{
			camera_pos->z = floor(camera_pos->z) + 0.9;
			camera_lookat->z += (camera_pos->z - old_y);
		}
       
		else
		{
			camera_pos->x = floor(camera_pos->x) + 0.1;
			camera_lookat->x += (camera_pos->x - old_x);
		}
    }
    else if(corner3 && x<0.1 && y<0.1 )
	{
        if (x < y)
		{
			camera_pos->z = floor(camera_pos->z) + 0.1;
			camera_lookat->z += (camera_pos->z - old_y);
		}
       
		else
		{
			camera_pos->x = floor(camera_pos->x) + 0.1;
			camera_lookat->x += (camera_pos->x - old_x);
		}
    }
    else if(corner4 && x>0.9 && y<0.1 )
	{
        if (x > (1-y))
		{
			camera_pos->z = floor(camera_pos->z) + 0.1;
			camera_lookat->z += (camera_pos->z - old_y);
		}
       
		else
		{
			camera_pos->x = floor(camera_pos->x) + 0.9;
			camera_lookat->x += (camera_pos->x - old_x);
		}
    }
}

void check_flag(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot)
{
	// if not picked, pick it, it picked and on start cell, end level
	if (get_xy_cell(camera_pos->x, camera_pos->z) == 'E' && !FLAG_PICKED)
		FLAG_PICKED = 1;
	else if (get_xy_cell(camera_pos->x, camera_pos->z) == 'B' && FLAG_PICKED)
		end_level(camera_pos, camera_lookat, camera_rot);
}

// Pick a score object if standing on it
void pickup_score(vec3* camera_pos)
{
	if (get_xy_cell(camera_pos->x, camera_pos->z) == 'S')
	{
		if ((camera_pos->x - (floor(camera_pos->x) + 0.5) < 0.1) && 
			(camera_pos->z - (floor(camera_pos->z) + 0.5) < 0.1))
		{
			set_xy_cell(camera_pos->x, camera_pos->z, '0');
			SCORE++;
			// Update lights to remove the one were the score object was
			set_lights();
		}

	}
}
void enable_lever(vec3* camera_pos)
{
	if (get_xy_cell(camera_pos->x, camera_pos->z) == 'L' && glutKeyIsDown('e'))
	{
		change_state_doors();
		set_xy_cell(camera_pos->x, camera_pos->z, 'l');
	}
}
void change_state_doors()
{
	for (int x = 0; x < SIZE; ++x)
	{
		for (int y = 0; y < SIZE; ++y)
		{
			if (get_xy_cell(x, y) == 'D') mazearray[x][y] = 'd';
			else if (get_xy_cell(x, y) == 'd') mazearray[x][y] = 'D';
		}
	}
}

int has_ground(int x, int y)
{
	char cell = get_xy_cell(x,y);
	return (cell=='0' || cell=='S' || cell=='I' || cell=='B' || cell== 'E' || cell=='d' || cell=='l' || cell=='L');
}

void set_xy_cell(double x, double y, char cell){mazearray[(int)floor(x)][(int)floor(y)] = cell;}
char get_xy_cell(double x, double y) 	{return mazearray[(int)floor(x)][(int)floor(y)];}
int check_wall  (int x, int y) {return get_xy_cell(x, y) == 'X';}
int wall_east   (int x, int y) {return (x < SIZE - 1) ? check_wall(x+1, y) : 0;}
int wall_north  (int x, int y) {return (y > 0) ? 		check_wall(x, y-1) : 0;}
int wall_west   (int x, int y) {return (x > 0) ?        check_wall(x-1, y) : 0;}
int wall_south  (int x, int y) {return (y < SIZE - 1) ? check_wall(x, y+1) : 0;}
int check_door  (int x, int y) {return get_xy_cell(x, y) == 'D';}
int door_east   (int x, int y) {return (x < SIZE - 1) ? check_door(x+1, y) : 0;}
int door_north  (int x, int y) {return (y > 0) ? 		check_door(x, y-1) : 0;}
int door_west   (int x, int y) {return (x > 0) ?        check_door(x-1, y) : 0;}
int door_south  (int x, int y) {return (y < SIZE - 1) ? check_door(x, y+1) : 0;}
	
int flag_picked() {return FLAG_PICKED;}

void end_level(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot)
{	
	// Try to go to the next level
	int try = load_level(++current_level);
	if (try >= 0) // If there is a next level
	{
		// reset some values
		printf("Score for level %d : %d\n", current_level, SCORE);
		SCORE = 0;
		FLAG_PICKED = 0;
		// reset the camera and light positions
		set_default_camera(camera_pos, camera_lookat, camera_rot);
		set_lights();
	}
	else exit(0);
}

///////////////////////////////////////////////////////////////////////////////

// Different drawing functions
void draw_square(int x, int y, mat4 base, Model *model, GLuint program)
{
	mat4 model_pos = T(x, 0, y);
	mat4 model_rot = base;
	mat4 model_transform = Mult(model_pos, model_rot);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
	DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
}
void draw_score(int x, int y, Model *model, GLuint program)
{
	mat4 model_pos = T(x+0.5, 0, y+0.5);
	mat4 model_scale = S(0.001, 0.001, 0.001);
	mat4 model_transform = Mult(model_pos, model_scale);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
	DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
}
void draw_up_lever(int x, int y, Model *model, GLuint program)
{
	mat4 model_pos = T(x+0.5, 0.5, y+0.5);
	mat4 model_scale = S(0.1, 0.1, 0.1);
	mat4 model_transform = Mult(model_pos, model_scale);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
	DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
}
void draw_down_lever(int x, int y, Model *model, GLuint program)
{
	mat4 model_pos = T(x+0.5, 0.5, y+0.5);
	mat4 model_scale = S(0.06, 0.06, 0.06);
	mat4 model_transform = Mult(model_pos, model_scale);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
	DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
}

void get_light_sources(GLfloat* array, int* nb)
{
	for (int x = 0; x < SIZE; ++x)
	{
		for (int y = 0; y < SIZE; ++y)
		{	
			// Draw lights for all special cells
			if (get_xy_cell(x, y) == 'B' || get_xy_cell(x, y) == 'E' || 
				get_xy_cell(x, y) == 'L' || get_xy_cell(x, y) == 'l' ||
				get_xy_cell(x, y) == 'S')
			{
				array[3*(*nb)] = x + 0.5;
				array[3*(*nb)+1] = 0.5;
				array[3*(*nb)+2] = y + 0.5;
				*nb = *nb + 1;
			}	
		}
	}
}

int load_level(int i)
{
	char file_name[18];
    sprintf(file_name, "../levels/level_%d", i);
	FILE* file = fopen(file_name, "r");
	
    if (file == NULL) return -1;

    int curr_row = 0;
	int curr_col = 0;
    char next_char;
	
    while ((next_char = getc(file)) != EOF)
    {
        if (next_char == '\n')
        {
            curr_row++;
            curr_col=0;
        }
        else
        {	
            mazearray[curr_col][curr_row] = next_char;
            curr_col++;
        }  
    }	
	fclose(file);
	current_level = i;
    return i;
} 

void set_program(GLuint* p)
{
	program = *p;
}

void set_lights()
{
	glUseProgram(program);
	GLfloat light_sources[50];
	int number_light_sources = 0;
	get_light_sources(light_sources, &number_light_sources); 

	glUniform3fv(glGetUniformLocation(program, "lightSources"), number_light_sources, light_sources);
	glUniform1i(glGetUniformLocation(program, "lightCount"), number_light_sources);
}

