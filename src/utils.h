#ifndef _UTILS_H_
#define _UTILS_H_
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"
#include <math.h>
#include <stdio.h>
#include "CallMeAL.h"
#include "simplefont.h"
#include <time.h>

#define SIZE 21
#define PI 3.1415

#define EMPTY '0'
#define WALL 'X'
#define SOLVE '-'
#define START 'B'
#define END 'E' 
#define FLOOD 'F'
#define OTHER 'K'
#define DOOR_CLOSE 'D'
#define DOOR_OPEN 'd'
#define LEVER 'L'
#define LEVER_PRESSED 'l'
#define SCORE 'S'

typedef struct {
    double x, y, z;
    double angle;
    double vx, vy, vz;
} particle;

// Variables used by both main.c and utils.c
GLuint program;
particle** particles;
int part_x, part_y;

void draw_flag(double x, double z, double y, Model *model, GLuint program, vec3* camera_pos, vec3* camera_lookat);
void draw_square(int x, int y, mat4 base,Model *model, GLuint program);
void draw_score(int x, int y, Model *model, GLuint program);
void draw_up_lever(int x, int y, Model *model, GLuint program);
void draw_down_lever(int x, int y, Model *model, GLuint program);
void draw_particles(particle** particles, Model *model, GLuint program);
void set_default_camera(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot);
void update(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot);
void pickup_score(vec3* camera_pos);
void enable_lever(vec3* camera_pos);
void check_flag(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot);
void change_state_doors();
int flag_picked();
int get_score();
int get_level();
void end_level(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot);
int load_level(int i);
void draw_text(vec3* camera_pos);



void get_light_sources(GLfloat* array, int* nb);
// Need this, return 1 if wall at the adgacent position, 0 otherwise

int check_wall(int x, int y);
int check_door(int x, int y);
int wall_north(int x, int y);
int wall_east (int x, int y);
int wall_south(int x, int y);
int wall_west (int x, int y);
int door_north(int x, int y);
int door_east (int x, int y);
int door_south(int x, int y);
int door_west (int x, int y);
char get_xy_cell(double x, double y);
void set_xy_cell(double x, double y, char cell);
void check_position(vec3 *camera_pos, vec3 *camera_lookat);
void check_corner(vec3 *camera_pos, vec3 *camera_lookat);

// returns 1 if has some ground (=> only ground or ground with objective on it, start and end)
int has_ground(int x, int y);

void set_lights();

void init_sound();

int is_flood(int x, int y);
void flood_from_position(int x, int y, int count, vec3* camera_pos, vec3 * camera_lookat, int up, int left, int right, int down);
void reset_flood();

void allocate_particles(particle*** array, double x, double y);
void reset_particle(particle* p, double x, double y, int i);
void simulate_particules(particle** particles, double x, double y);
void free_particles(particle** particles);

void get_start_cell_position(int* x, int* y);

int create_maze();
void carve_maze(int x, int y);
void generate_empty();
void solve_maze();
void reset_generate_end();
void generate_end(int x, int y, int count);
void generate_door();
void replace_other_by_empty();
#endif
