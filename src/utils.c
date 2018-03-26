#include "utils.h"
#define PI 3.141592
#define SIZE 20

void draw_square(int x, int y, mat4 base, Model *model, GLuint program)
{
	mat4 model_pos = T(x, 0, y);
	mat4 model_rot = base;
	mat4 model_transform = Mult(model_pos, model_rot);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
	DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
}

void set_default_camera(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot)
{
    camera_pos->x = 0;camera_pos->y = 0.5;camera_pos->z=0.0;
	camera_lookat->x = 0.0; camera_lookat->y = 0.5; camera_lookat->z = 5.0;
	camera_rot->x = 0.0; camera_rot->y = 1.0; camera_rot->z = 0.0;
} 
void move_camera(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot, float horizontal_speed, float rotation_speed, float vertical_speed)
{
	// Save camera posiitin

	if (glutKeyIsDown('w')) // Go forward
	{
		vec3 move = ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed);
		*camera_pos = VectorAdd(*camera_pos, move);
		*camera_lookat = VectorAdd(*camera_lookat, move);
	}
	if (glutKeyIsDown('s')) // Go backward
	{
		vec3 move = ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed);
		*camera_pos = VectorSub(*camera_pos, move);
		*camera_lookat = VectorSub(*camera_lookat, move);
	}
	if (glutKeyIsDown('a')) // Go left
	{
		vec3 move = MultVec3(Ry(PI/2),ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed));
		*camera_pos = VectorAdd(*camera_pos, move);
		*camera_lookat = VectorAdd(*camera_lookat, move);
	}
	if (glutKeyIsDown('d')) // Go right
	{
		vec3 move = MultVec3(Ry(PI/2),ScalarMult(VectorSub(*camera_lookat,*camera_pos), horizontal_speed));
		*camera_pos = VectorSub(*camera_pos, move);
		*camera_lookat = VectorSub(*camera_lookat, move);
	}
	if (glutKeyIsDown('e')) // rotate right
	{
		
		vec3 cam_to_lookpoint = VectorSub(*camera_lookat,*camera_pos);
		vec3 new_cam_to_lookpoint = MultVec3(Ry(-rotation_speed), cam_to_lookpoint);
		*camera_lookat = VectorAdd(*camera_pos, new_cam_to_lookpoint); 
	}
	if (glutKeyIsDown('q')) // Rotate left
	{
		
		vec3 cam_to_lookpoint = VectorSub(*camera_lookat,*camera_pos);
		vec3 new_cam_to_lookpoint = MultVec3(Ry(rotation_speed), cam_to_lookpoint);
		*camera_lookat = VectorAdd(*camera_pos, new_cam_to_lookpoint); 
	}
	if (glutKeyIsDown('r')) // Go up
	{
		
		*camera_lookat = VectorAdd(*camera_lookat, SetVector(0.0, vertical_speed, 0.0));
		*camera_pos = VectorAdd(*camera_pos, SetVector(0.0, vertical_speed, 0.0));  
	}
	if (glutKeyIsDown('f')) // Go down
	{
		
		*camera_lookat = VectorAdd(*camera_lookat, SetVector(0.0, -vertical_speed, 0.0));
		*camera_pos = VectorAdd(*camera_pos, SetVector(0.0, -vertical_speed, 0.0));  
	}

	// Call cehck_position
	// if not ok, restore value
	// Call check objective and doors
}
char get_xy_cell(int x int y){
char cell;
// unsure if this is the best way or the correct way to get the character
cell = mazearray[x][y];
reurn cell;
}


int check_wall(int x, int y) {
char cell;
cell = get_xy_cell(x,y);
if (cell='X')
    return 1;
if (cell='0')
    return 0;
else if
    return 0;
}

int has_ground(int x, int y){
char cell;
cell = get_xy_cell(x,y);
if (cell='0')
    return 1;
else if (cell='S')
    return 1;
else if (cell='I')
    return 1;
else if (cell='B')
    return 1;
else return 0;
}

int wall_south(int x, int y){
int r=0;
if (y<20)
    y=y+1;
r= check_wall(int x, int y);
return r
}

int wall_west(int x, int y){
int r=0;
if (x>0)
    x=x-1;
r= check_wall(int x, int y);
return r
}

int wall_south(int x, int y){
int r=0;
if (x<20)
    x=x+1;
r= check_wall(int x, int y);
return r
}



