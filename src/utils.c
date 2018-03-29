#include "utils.h"
#define PI 3.141592
#define SIZE 20
// X are solid walls
// x are not solid walls
// 0 are empty cells
// S is a score object
// B is the beggining cell
// E is the end cell
// D is a door 
// I is an interruptor
int SCORE = 0;
char mazearray[SIZE][SIZE] = {
	{'X','X','X','X','X','X','X','X','X','X','X','X','X','X','X','X','X','X','X','X'},
	{'X','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','S','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','0','S','0','0','0','S','0','0','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','0','0','X','X','X','D','X','X','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','0','0','0','0','0','0','0','X','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','0','0','X','0','0','0','0','X','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','0','0','d','0','0','0','0','X','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','0','0','d','0','0','0','0','D','0','0','0','0','0','0','0','0','0','X'},
	{'X','S','0','0','X','X','X','d','X','X','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','X','0','X','0','0','0','0','0','B','0','0','0','0','0','0','0','0','X'},
	{'X','0','0','L','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','X','0','X','0','0','0','0','0','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','d','0','d','0','0','0','0','0','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','X','D','X','0','0','0','0','0','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','X'},
	{'X','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','X'},
	{'X','X','X','X','X','X','X','X','X','X','X','X','X','X','X','X','X','X','X','X'}
};


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
void set_default_camera(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot)
{
	for (int x = 0; x < SIZE; ++x)
	{
		for (int y = 0; y < SIZE; ++y)
		{
			if (get_xy_cell(x, y) == 'B') 
			{
				camera_pos->x = x; camera_pos->y = 0.5;camera_pos->z=y;
				camera_lookat->x = 0.0; camera_lookat->y = 0.5; camera_lookat->z = 5.0;
				camera_rot->x = 0.0; camera_rot->y = 1.0; camera_rot->z = 0.0;
			}
			
		}
	}
   
} 
void move_camera(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot, float horizontal_speed, float rotation_speed, float vertical_speed)
{
	// Check if we are on a lever before moving
	int was_on_lever = (get_xy_cell((int)floor(camera_pos->x), (int)floor(camera_pos->z)) == 'L' ||
						get_xy_cell((int)floor(camera_pos->x), (int)floor(camera_pos->z)) == 'l');
	int old_x = floor(camera_pos->x);
	int old_y = floor(camera_pos->z);

	if (glutKeyIsDown('w')) // Go forward
	{
		vec3 move = ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed);
		*camera_pos = VectorAdd(*camera_pos, SetVector(move.x, 0, move.z));
		*camera_lookat = VectorAdd(*camera_lookat, SetVector(move.x, 0, move.z));
	}
	if (glutKeyIsDown('s')) // Go backward
	{
		vec3 move = ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed);
		*camera_pos = VectorAdd(*camera_pos, SetVector(-move.x, 0, -move.z));
		*camera_lookat = VectorAdd(*camera_lookat, SetVector(-move.x, 0, -move.z));
	}
	if (glutKeyIsDown('a')) // Go left
	{
		vec3 move = MultVec3(Ry(PI/2),ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed));
		*camera_pos = VectorAdd(*camera_pos, SetVector(move.x, 0, move.z));
		*camera_lookat = VectorAdd(*camera_lookat, SetVector(move.x, 0, move.z));
	}
	if (glutKeyIsDown('d')) // Go right
	{
		vec3 move = MultVec3(Ry(PI/2),ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed));
		*camera_pos = VectorAdd(*camera_pos, SetVector(-move.x, 0, -move.z));
		*camera_lookat = VectorAdd(*camera_lookat, SetVector(-move.x, 0, -move.z));
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
		//*camera_lookat = VectorAdd(*camera_lookat, SetVector(0.0, vertical_speed, 0.0));
		//*camera_pos = VectorAdd(*camera_pos, SetVector(0.0, vertical_speed, 0.0));  
	}
	if (glutKeyIsDown(GLUT_KEY_DOWN)) // Go down
	{
		mat4 vertical_rotate = ArbRotate(CrossProduct(VectorSub(*camera_lookat,*camera_pos), *camera_rot), -rotation_speed);
		
		vec3 cam_to_lookpoint = VectorSub(*camera_lookat,*camera_pos);
		vec3 new_cam_to_lookpoint = MultVec3(vertical_rotate, cam_to_lookpoint);
		*camera_lookat = VectorAdd(*camera_pos, new_cam_to_lookpoint); 
		*camera_rot = MultVec3(vertical_rotate, *camera_rot);
		//*camera_lookat = VectorAdd(*camera_lookat, SetVector(0.0, -vertical_speed, 0.0));
		//*camera_pos = VectorAdd(*camera_pos, SetVector(0.0, -vertical_speed, 0.0));  
	}
	enable_lever(camera_pos);
	pickup_score(camera_pos);

	// If we left a lever cell, we reset the lever to be enable again
	if (was_on_lever && get_xy_cell((int)floor(camera_pos->x), (int)floor(camera_pos->z)) != 'l')
	{
		mazearray[old_x][old_y] = 'L';
	}
	// Call cehck_position
	// if not ok, restore value
	// Call check objective and doors
}

// Pick a score object if standing on it
void pickup_score(vec3* camera_pos)
{
	if (get_xy_cell(camera_pos->x, camera_pos->z) == 'S')
	{
		if ((camera_pos->x - (floor(camera_pos->x) + 0.5) < 0.1) && 
			(camera_pos->z - (floor(camera_pos->z) + 0.5) < 0.1))
		{
			mazearray[(int)floor(camera_pos->x)][(int)floor(camera_pos->z)] = '0';
			SCORE++;
		}

	}
}
void enable_lever(vec3* camera_pos)
{
	if (get_xy_cell(camera_pos->x, camera_pos->z) == 'L' && glutKeyIsDown('e'))
	{

		change_state_doors();
		mazearray[(int)floor(camera_pos->x)][(int)floor(camera_pos->z)] = 'l';

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
char get_xy_cell(int x, int y)
{
	return mazearray[x][y];
}


int check_wall(int x, int y) 
{
	char cell = get_xy_cell(x,y);
	if (cell=='X') return 1;
	else return 0;
}

int has_ground(int x, int y)
{
	char cell = get_xy_cell(x,y);
	if (cell=='0' || cell=='S' || cell=='I' || cell=='B' || cell=='d' || cell=='l' || cell=='L') return 1;
	else return 0;
}

int wall_east(int x, int y)
{
	if (x < SIZE - 1) return check_wall(x+1, y);
	else return 0;
}

int wall_north(int x, int y)
{
	if (y > 0) return check_wall(x, y-1);
	else return 0;
}

int wall_west(int x, int y)
{
	if (x > 0) return check_wall(x-1, y);
	else return 0;
}

int wall_south(int x, int y)
{
	if (y < SIZE - 1) return check_wall(x, y+1);
	else return 0;
}



