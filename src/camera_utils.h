

#ifndef _CAMERA_UTILS_H_
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include <math.h>


void set_default_camera(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot);
void move_camera(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot, float horizontal_speed, float rotation_speed, float vertical_speed);

#endif
