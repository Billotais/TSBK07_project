

#ifndef _CAMERA_UTILS_H_
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include <math.h>

void draw_square(int x, int y, mat4 base,Model *model, GLuint program);
void draw_score(int x, int y, Model *model, GLuint program);
void set_default_camera(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot);
void move_camera(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot, float horizontal_speed, float rotation_speed, float vertical_speed);
void pickup_score(vec3* camera_pos);
// Need this, return 1 if wall at the adgacent position, 0 otherwise
int wall_north(int x, int y);
int wall_east(int x, int y);
int wall_south(int x, int y);
int wall_west(int x, int y);
char get_xy_cell(int x, int y);

// returns 1 if has some ground (=> only ground or ground with objective on it, start and end)
int has_ground(int x, int y);


#endif
