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

ALuint score_sound;
ALuint door_sound;

int SCORE = 0;
int FLAG_PICKED = 0;
int CURRENT_LEVEL = 0;

// Same program as in main.c, used to uplaod the lights to the GPU
extern GLuint program;

double camera_bump_evolution = 0;

char mazearray[SIZE][SIZE];

///////////////////////////////////////////////////////////////////
/* Game upate and run functions
 * Move camera, trigger levers and score, flags, ...
 */
///////////////////////////////////////////////////////////////////

// Main function, react to keyboard, moves the camera, check for objectives and collisions
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
    if (glutKeyIsDown(GLUT_KEY_UP)) // Tilt up, need to restrict from going too far
    {
        mat4 vertical_rotate = ArbRotate(CrossProduct(VectorSub(*camera_lookat,*camera_pos), *camera_rot), rotation_speed);
        
        vec3 cam_to_lookpoint = VectorSub(*camera_lookat,*camera_pos);
        vec3 new_cam_to_lookpoint = MultVec3(vertical_rotate, cam_to_lookpoint);
        *camera_lookat = VectorAdd(*camera_pos, new_cam_to_lookpoint); 
        *camera_rot = MultVec3(vertical_rotate, *camera_rot);
         
    }
    if (glutKeyIsDown(GLUT_KEY_DOWN)) // Tilt down, need to restrict from going too far
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
// Check that the current position is valid, along the walls
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
// Check that the current position is valid, for the corners
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
// Check if the flag is picked
void check_flag(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot)
{
    // if not picked, pick it, it picked and on start cell, end level
    if (get_xy_cell(camera_pos->x, camera_pos->z) == 'E' && !FLAG_PICKED)
        FLAG_PICKED = 1;
    else if (get_xy_cell(camera_pos->x, camera_pos->z) == 'B' && FLAG_PICKED)
        end_level(camera_pos, camera_lookat, camera_rot);
}
// Increase score , and remove object when we pick an object
void pickup_score(vec3* camera_pos)
{
    if (get_xy_cell(camera_pos->x, camera_pos->z) == 'S')
    {
        if ((camera_pos->x - (floor(camera_pos->x) + 0.5) < 0.1) && 
            (camera_pos->z - (floor(camera_pos->z) + 0.5) < 0.1))
        {
            PlaySoundInChannel(score_sound, 0);
            set_xy_cell(camera_pos->x, camera_pos->z, '0');
            SCORE++;
            // Update lights to remove the one were the score object was
            set_lights();
        }
    }
}
// React to the action of pressing a lever
void enable_lever(vec3* camera_pos)
{
    if (get_xy_cell(camera_pos->x, camera_pos->z) == 'L' && glutKeyIsDown('e'))
    {
        PlaySoundInChannel(door_sound, 0);
        change_state_doors();
        set_xy_cell(camera_pos->x, camera_pos->z, 'l');
    }
}
// Switch the state of doors
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

///////////////////////////////////////////////////////////////////
/* Utils functions for drawing and update
 * Optimisation, wall and door checks
 */
///////////////////////////////////////////////////////////////////

// Tell what area of the grid should be drawn depending on the orientation of the player
void get_bounds_for_optimisation(vec3* camera_pos, vec3* camera_lookat, int* x_from, int* x_to, int* y_from, int* y_to)
{
    vec3 cross = Normalize(CrossProduct(SetVector(1, 0, 0), Normalize(VectorSub(*camera_lookat, *camera_pos))));
    double view_angle =  DotProduct(Normalize(VectorSub(*camera_lookat, *camera_pos)), SetVector(1, 0, 0));

     // Right area
    if (view_angle >  cos(PI/4) && view_angle <= 1) *x_from = floor(camera_pos->x)-1;

     // Left area
    else if (view_angle >= -1 && view_angle < -cos(PI/4)) *x_to = ceil(camera_pos->x)+1;
    
    else if (view_angle >= -cos(PI/4) && view_angle <= cos(PI/4))
    {
        // Up area
        if (DotProduct(cross, SetVector(0, 1, 0)) > 0)  *y_to = ceil(camera_pos->z)+1;
        // Dow area
        else *y_from = floor(camera_pos->z)-1;
    }
}
// Change the value of a cell
void set_xy_cell(double x, double y, char cell){mazearray[(int)floor(x)][(int)floor(y)] = cell;}
// get the value of a cell
char get_xy_cell(double x, double y) 	       {return mazearray[(int)floor(x)][(int)floor(y)];}

// Check for wall at a specific position, or next to a position
int check_wall  (int x, int y) {return get_xy_cell(x, y) == 'X';}
int wall_east   (int x, int y) {return (x < SIZE - 1) ? check_wall(x+1, y) : 0;}
int wall_north  (int x, int y) {return (y > 0) ? 		check_wall(x, y-1) : 0;}
int wall_west   (int x, int y) {return (x > 0) ?        check_wall(x-1, y) : 0;}
int wall_south  (int x, int y) {return (y < SIZE - 1) ? check_wall(x, y+1) : 0;}

// Check for door at a specific position, or next to a position
int check_door  (int x, int y) {return get_xy_cell(x, y) == 'D';}
int door_east   (int x, int y) {return (x < SIZE - 1) ? check_door(x+1, y) : 0;}
int door_north  (int x, int y) {return (y > 0) ? 		check_door(x, y-1) : 0;}
int door_west   (int x, int y) {return (x > 0) ?        check_door(x-1, y) : 0;}
int door_south  (int x, int y) {return (y < SIZE - 1) ? check_door(x, y+1) : 0;}

// Tell if we need to draw a ground
int has_ground(int x, int y)
{
    char cell = get_xy_cell(x,y);
    return (cell=='0' || cell=='S' || cell=='I' || cell=='B' || cell== 'E' || cell=='d' || cell=='l' || cell=='L');
}

// Simple getter functions
int flag_picked() {return FLAG_PICKED;}
int get_score()   {return SCORE;}
int get_level()   {return CURRENT_LEVEL;}

///////////////////////////////////////////////////////////////////
/* Initialisation functions
 * For camera, sound level...
 */
///////////////////////////////////////////////////////////////////

// Put the camera at the start cell in the adequate position
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
// Initialize sound API
void init_sound()
{
    // Init sound
    if (!InitCallMeAL(2))
    {
        printf("FAILED TO INIT OPEN AL\n");
        exit(-1);
    }
    // Load all sounds
    score_sound = LoadSound("../sounds/score.wav");
    door_sound = LoadSound("../sounds/open_door.wav"); 
}
// Load a given level into memory
int load_level(int i)
{
    // Open the file
    char file_name[18];
    sprintf(file_name, "../levels/level_%d", i);
    FILE* file = fopen(file_name, "r");
    if (file == NULL) return -1;

    int curr_row = 0;
    int curr_col = 0;
    char next_char;
    
    // read while there is something
    while ((next_char = getc(file)) != EOF)
    {
        if (next_char == '\n') // If new line => new row
        {
            curr_row++;
            curr_col=0;
        }
        else // Else read character
        {	
            mazearray[curr_col][curr_row] = next_char;
            curr_col++;
        }  
    }	
    fclose(file);
    CURRENT_LEVEL = i;
    return i;
} 

////////////////////////////////////////////////////////////////////////////
/* Drawing functions
 * For the 3D objects, lights
 * And for the interface
 */
////////////////////////////////////////////////////////////////////////////

// return location of all light sources
void get_light_sources(GLfloat* array, int* nb)
{
    for (int x = 0; x < SIZE; ++x)
    {
        for (int y = 0; y < SIZE; ++y)
        {	
            // Draw lights for all special cells
            if (get_xy_cell(x, y) == 'B' || get_xy_cell(x, y) == 'E' || 
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
// Get and upload light sources to GPU
void set_lights()
{
    glUseProgram(program);
    GLfloat light_sources[50];
    int number_light_sources = 0;

    // Get the lights
    get_light_sources(light_sources, &number_light_sources); 

    // Upload them to the GPU
    glUniform3fv(glGetUniformLocation(program, "lightSources"), number_light_sources, light_sources);
    glUniform1i(glGetUniformLocation(program, "lightCount"), number_light_sources);
}
// Draw UI text
void draw_text()
{
    // Create strings for each texts
    char level_name[15];
    sprintf(level_name, "Level : %d", get_level());
    char score_name[15];
    sprintf(score_name, "Score : %d/3", get_score());

    // Draw them
    sfDrawString(20, 40, level_name);
    sfDrawString(20, 60, score_name);

    if (flag_picked()) sfDrawString(20, 20, "Bring the flag back to the starting cell");
    else sfDrawString(20, 20, "Find the flag");
}
// Draw a square using base matrix
void draw_square(int x, int y, mat4 base, Model *model, GLuint program)
{
    mat4 model_pos = T(x, 0, y);
    mat4 model_rot = base;
    mat4 model_transform = Mult(model_pos, model_rot);
    glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
    DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
}
// Draw a score object
void draw_score(int x, int y, Model *model, GLuint program)
{
    mat4 model_pos = T(x+0.5, 0, y+0.5);
    mat4 model_scale = S(0.001, 0.001, 0.001);
    mat4 model_transform = Mult(model_pos, model_scale);
    glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
    DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
}
// Draw the lever in its non-trigger position
void draw_up_lever(int x, int y, Model *model, GLuint program)
{
    mat4 model_pos = T(x+0.5, 0.5, y+0.5);
    mat4 model_scale = S(0.1, 0.1, 0.1);
    mat4 model_transform = Mult(model_pos, model_scale);
    glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
    DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
}
// Draw the lever in its trigger positeion
void draw_down_lever(int x, int y, Model *model, GLuint program)
{
    mat4 model_pos = T(x+0.5, 0.5, y+0.5);
    mat4 model_scale = S(0.06, 0.06, 0.06);
    mat4 model_transform = Mult(model_pos, model_scale);
    glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
    DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
}
// Draw the flag, either on the end cell or with the player
void draw_flag(double x, double z, double y, Model *model, GLuint program, vec3* camera_pos, vec3* camera_lookat)
{
    mat4 model_pos;
    mat4 model_rot;

    if (flag_picked())
    {
        // Displacement compared to the camera position
        vec3 dir = MultVec3(Ry(0.5),VectorSub(*camera_lookat, *camera_pos));
        double x_ = camera_pos->x+dir.x/10.0;
        double y_ = camera_pos->y-0.2;
        double z_ = camera_pos->z+dir.z/10.0;

        // We add a wobble to give a nicer effect
        model_pos = T(x_, y_ + sin(camera_bump_evolution+3)/(3*WOBBLE_HEIGHT), z_);

        // Compute the current absolute angle of the player to rotate the flag in the correct orientation
        double angle = acos(DotProduct(Normalize(VectorSub(*camera_lookat, *camera_pos)), SetVector(0, 0, 1)));
        if (DotProduct(Normalize(VectorSub(*camera_lookat, *camera_pos)), SetVector(1, 0, 0)) < 0) angle = -angle;
        model_rot = Ry(angle);
    } 
    else // Just put it on the end cell
    {
        model_pos = T(x+0.5, z+0.1, y+0.5);
        model_rot = Ry(0);
    }
    mat4 model_scale = S(0.04, 0.04, 0.04);
    
    mat4 model_transform = Mult(model_pos, Mult(model_rot, model_scale));
    
    glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
    DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
    
}

//////////////////////////////////////////////////////////////

// End the vel, go to the next one
void end_level(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot)
{	
    // Try to go to the next level
    int try = load_level(++CURRENT_LEVEL);
    if (try >= 0) // If there is a next level
    {
        // reset some values
        SCORE = 0;
        FLAG_PICKED = 0;
        // reset the camera and light positions
        set_default_camera(camera_pos, camera_lookat, camera_rot);
        set_lights();
    }
    else 
    {	// Clear sound API
        alDeleteBuffers(1,&score_sound);
        HaltCallMeAL();

        // Exit
        exit(0);
    }
}