#include "utils.h"


#define PI 3.141592
#define SIZE 21
#define N_PARTICLES 1000
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
#define DIST_TO_WALL 0.1

#define HOR_SPEED 0.02
#define VERT_SPEED 0.01
#define ROT_SPEED 0.03

#define FLOOD_SIZE 0.75*SIZE

ALuint score_sound;
ALuint door_sound;
ALuint ambiance_sound;
ALuint victory_sound;
ALuint cup_picked_sound;


int SCORE = 0;
int FLAG_PICKED = 0;
int CURRENT_LEVEL = 0;

// Same program as in main.c, used to uplaod the lights to the GPU
extern GLuint program;

extern particle** particles;
extern int part_x, part_y;

double camera_bump_evolution = 0;

// Real maze
char mazearray[SIZE][SIZE];
// Maze use for the flood algorithm
char mazearray_flood[SIZE][SIZE];


///////////////////////////////////////////////////////////////////
/* Game upate and run functions
 * Move camera, trigger levers and score, flags, ...
 */
///////////////////////////////////////////////////////////////////

// Main function, react to keyboard, moves the camera, check for objectives and collisions
void update(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot)
{
    if (!glutKeyIsDown('q'))
    {
        reset_flood();
        flood_from_position((int)floor(camera_pos->x),(int)floor(camera_pos->z),0, camera_pos, camera_lookat, 0, 0, 0, 0);
        set_lights();
    }
    
    float horizontal_speed = HOR_SPEED; 
    float rotation_speed = ROT_SPEED;
    float vertical_speed = VERT_SPEED;

    // Allow faster travel for debugging and demonstration purpose
    if (glutKeyIsDown('c')) horizontal_speed = 2*HOR_SPEED;
    
    // Check if we are on a lever before moving
    int was_on_lever = (get_xy_cell(camera_pos->x, camera_pos->z) == 'L' ||
                        get_xy_cell(camera_pos->x, camera_pos->z) == 'l');

    int old_x = camera_pos->x;
    int old_y = camera_pos->z;

    // Used for the camera wobble effect
    double previous_camera_bump_evolution = camera_bump_evolution;


    // Move player position

    // Compute how much we must move
    vec3 move = SetVector(0, 0, 0);

    if (glutKeyIsDown('w')) // Go forward
        move = VectorAdd(move, VectorSub(*camera_lookat,*camera_pos));
    if (glutKeyIsDown('s')) // Go backward
        move = VectorAdd(move, VectorSub(*camera_pos,*camera_lookat));
    if (glutKeyIsDown('a')) // Go left
        move = VectorAdd(move, MultVec3(Ry(PI/2),VectorSub(*camera_lookat,*camera_pos)));
    if (glutKeyIsDown('d')) // Go right
        move = VectorAdd(move, MultVec3(Ry(PI/2),VectorSub(*camera_pos,*camera_lookat)));
    

    // if we had a move, to prevent segfault when normalizing
    if (move.x != 0 || move.y != 0 || move.z != 0) 
    {
        // Scale it so we don't go faster if two keys pressed
        move = ScalarMult(Normalize(move), horizontal_speed);
        // Make the wobble evolve
        camera_bump_evolution+=WOBBLE_SPEED;
    }
    // Update the position

    *camera_pos = VectorAdd(*camera_pos, SetVector(move.x, 0, move.z));
    *camera_lookat = VectorAdd(*camera_lookat, SetVector(move.x, 0, move.z));

    // Move player view

    vec3 cam_to_lookpoint = VectorSub(*camera_lookat,*camera_pos);
    mat4 rotation = IdentityMatrix();
    
    // COmpute the rotation matrix
    if (glutKeyIsDown(GLUT_KEY_RIGHT)) // rotate right
        rotation = Mult(rotation, Ry(-rotation_speed));
    if (glutKeyIsDown(GLUT_KEY_LEFT)) // Rotate left
        rotation = Mult(rotation, Ry(rotation_speed));
    if (glutKeyIsDown(GLUT_KEY_UP)) // Tilt up, need to restrict from going too far
        rotation = Mult(rotation, ArbRotate(CrossProduct(VectorSub(*camera_lookat,*camera_pos), *camera_rot), rotation_speed));
    if (glutKeyIsDown(GLUT_KEY_DOWN)) // Tilt down, need to restrict from going too far
        rotation = Mult(rotation, ArbRotate(CrossProduct(VectorSub(*camera_lookat,*camera_pos), *camera_rot), -rotation_speed));
    
    // Keep old value if we rotate too far
    vec3 old_lookat = *camera_lookat;
    vec3 old_rot    = *camera_rot;

    // Rotate the camera
    *camera_lookat = VectorAdd(*camera_pos, MultVec3(rotation, cam_to_lookpoint)); 
    *camera_rot = MultVec3(rotation, *camera_rot);

    // Check the current angle with the vertical
    double angle_with_vert = DotProduct(Normalize(VectorSub(*camera_lookat,*camera_pos)), SetVector(0, 1, 0));

    // If too small, we block the camera
    if (angle_with_vert < -0.9 || angle_with_vert > 0.9)
    {   
        *camera_lookat = old_lookat;
        *camera_rot = old_rot;
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
    if (!ChannelIsPlaying(1)) PlaySoundInChannel(ambiance_sound, 1);
    
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
    if (walle && x1>(1-DIST_TO_WALL)){
        camera_pos->x = floor(camera_pos->x) + (1-DIST_TO_WALL);
        camera_lookat->x += (camera_pos->x - old_x);
    }
    if (wallw && x1<DIST_TO_WALL){
        camera_pos->x = floor(camera_pos->x) + DIST_TO_WALL;
        camera_lookat->x += (camera_pos->x - old_x);
    }
    if (walls && y1>(1-DIST_TO_WALL)){
        camera_pos->z = floor(camera_pos->z) + (1-DIST_TO_WALL);
        camera_lookat->z += (camera_pos->z - old_y);
    }
    if (walln && y1<DIST_TO_WALL){
        camera_pos->z = floor(camera_pos->z) + DIST_TO_WALL;
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
    if(corner1 && x>(1-DIST_TO_WALL) && y>(1-DIST_TO_WALL) )
    {
        if (x > y)
        {
            camera_pos->z = floor(camera_pos->z) + (1-DIST_TO_WALL);
            camera_lookat->z += (camera_pos->z - old_y);
        }
        else
        {
            camera_pos->x = floor(camera_pos->x) + (1-DIST_TO_WALL);
            camera_lookat->x += (camera_pos->x - old_x);
        }
    }

    else if(corner2 && x<DIST_TO_WALL && y>(1-DIST_TO_WALL) )
    {
        if ((1-x) > y)
        {
            camera_pos->z = floor(camera_pos->z) + (1-DIST_TO_WALL);
            camera_lookat->z += (camera_pos->z - old_y);
        }
       
        else
        {
            camera_pos->x = floor(camera_pos->x) + DIST_TO_WALL;
            camera_lookat->x += (camera_pos->x - old_x);
        }
    }
    else if(corner3 && x<DIST_TO_WALL && y<DIST_TO_WALL )
    {
        if (x < y)
        {
            camera_pos->z = floor(camera_pos->z) + DIST_TO_WALL;
            camera_lookat->z += (camera_pos->z - old_y);
        }
       
        else
        {
            camera_pos->x = floor(camera_pos->x) + DIST_TO_WALL;
            camera_lookat->x += (camera_pos->x - old_x);
        }
    }
    else if(corner4 && x>(1-DIST_TO_WALL) && y<DIST_TO_WALL )
    {
        if (x > (1-y))
        {
            camera_pos->z = floor(camera_pos->z) + DIST_TO_WALL;
            camera_lookat->z += (camera_pos->z - old_y);
        }
       
        else
        {
            camera_pos->x = floor(camera_pos->x) + (1-DIST_TO_WALL);
            camera_lookat->x += (camera_pos->x - old_x);
        }
    }
}
// Check if the flag is picked
void check_flag(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot)
{
    // if not picked, pick it, it picked and on start cell, end level
    if (get_xy_cell(camera_pos->x, camera_pos->z) == 'E' && !FLAG_PICKED)
    {
        if ((abs(camera_pos->x - (floor(camera_pos->x) + 0.5)) < 0.1) && 
            (abs(camera_pos->z - (floor(camera_pos->z) + 0.5)) < 0.1))
        {
            FLAG_PICKED = 1;
            set_lights();
            PlaySoundInChannel(cup_picked_sound, 0);
        }   
        
    }
    else if (get_xy_cell(camera_pos->x, camera_pos->z) == 'B' && FLAG_PICKED)
        end_level(camera_pos, camera_lookat, camera_rot);
}
// Increase score , and remove object when we pick an object
void pickup_score(vec3* camera_pos)
{
    if (get_xy_cell(camera_pos->x, camera_pos->z) == 'S')
    {
        if ((abs(camera_pos->x - (floor(camera_pos->x) + 0.5)) < 0.1) && 
            (abs(camera_pos->z - (floor(camera_pos->z) + 0.5)) < 0.1))
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
            if (get_xy_cell(x, y) == 'D') 
            {
                mazearray[x][y] = 'd';
            }
            else if (get_xy_cell(x, y) == 'd') 
            {
                mazearray[x][y] = 'D';
            }
        }
    }  
}

///////////////////////////////////////////////////////////////////
/* Utils functions for drawing and update
 * Optimisation, wall and door checks
 */
///////////////////////////////////////////////////////////////////


// Flood functions

void reset_flood()
{
    for (int x = 0; x < SIZE; ++x)
        for (int y = 0; y < SIZE; ++y)
            mazearray_flood[x][y] = mazearray[x][y];
}
void flood_from_position(int x, int y, int count, vec3* camera_pos, vec3* camera_lookat, int up, int left, int right, int down)

{   if (count > FLOOD_SIZE) return;
    if (mazearray_flood[x][y] == 'F' || mazearray_flood[x][y] == 'D' || mazearray_flood[x][y] == 'X') return;   
    mazearray_flood[x][y] = 'F';

    vec3 dir = VectorSub(*camera_lookat, *camera_pos);
    dir = SetVector(dir.x, 0, dir.z);
    vec3 cross = Normalize(CrossProduct(SetVector(1, 0, 0), Normalize(dir)));
    double view_angle =  DotProduct(Normalize(dir), SetVector(1, 0, 0));

    int look_up = (view_angle >= -cos(PI/4) && view_angle <= cos(PI/4) && DotProduct(cross, SetVector(0, 1, 0)) > 0);
    int look_down = (view_angle >= -cos(PI/4) && view_angle <= cos(PI/4) && DotProduct(cross, SetVector(0, 1, 0)) < 0);
    int look_left = (view_angle >= -1 && view_angle < -cos(PI/4));
    int look_right = (view_angle >  cos(PI/4) && view_angle <= 1);

    // unless Right area, go left with flood
    if ((!look_right && !right) || (look_right && !left && !right))
        flood_from_position(x-1, y, count+1, camera_pos, camera_lookat, up, 1, right, down);

     // unless Left area, go right with flood
    if ((!look_left && !left)  || (look_left && !right && !left))
        flood_from_position(x+1, y, count+1, camera_pos, camera_lookat, up, left, 1, down);
        
     // unless up area, go down with flood
    if ((!look_up && !up) || (look_up && !down && !up))
        flood_from_position(x, y+1, count+1,camera_pos, camera_lookat, up, left, right, 1);
    
    // unless down area, go up with flood
    if ((!look_down && !down)  || (look_down && !up && !down))
       flood_from_position(x, y-1, count+1, camera_pos, camera_lookat, 1, left, right, down);

}
int is_flood(int x, int y)
{
    return (mazearray_flood[x][y] == 'F');
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
                //reset_flood();
                //flood_from_position(x, y, 0, camera_pos, camera_lookat, 0, 0, 0, 0);
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
    door_sound = LoadSound("../sounds/door.wav"); 
    ambiance_sound = LoadSound("../sounds/creepy_ambiance.wav");
    victory_sound = LoadSound("../sounds/victory.wav");
    cup_picked_sound = LoadSound("../sounds/cup_picked.wav");
    

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
            if ((get_xy_cell(x, y) == 'B' || (get_xy_cell(x, y) == 'E' && !flag_picked()) || 
                get_xy_cell(x, y) == 'S' || get_xy_cell(x, y) == 'L' || get_xy_cell(x, y) == 'l') && is_flood(x, y))
            {
                array[3*(*nb)] = x + 0.5;
                array[3*(*nb)+1] = 0.3;
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
void draw_text(vec3* camera_pos)
{
    // Create strings for each texts
    char level_name[15];
    sprintf(level_name, "Level : %d", get_level());
    char score_name[15];
    sprintf(score_name, "Score : %d/3", get_score());

    // Draw them
    sfDrawString(20, 40, level_name);
    sfDrawString(20, 60, score_name);

    if (flag_picked()) sfDrawString(20, 20, "Bring the cup back to the starting cell");
    else sfDrawString(20, 20, "Find the cup");

    // Draw information about interaction
    if (get_xy_cell(camera_pos->x, camera_pos->z) == 'L') sfDrawString(480, 200, "E");

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
    mat4 model_scale = S(0.2, 0.2, 0.2);
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
    mat4 model_scale;

    if (flag_picked())
    {
        // Displacement compared to the camera position
        vec3 diff = VectorSub(*camera_lookat, *camera_pos);
        diff = SetVector(diff.x, 0, diff.z);
        vec3 dir = Normalize(MultVec3(Ry(0.5),diff));

        double x_ = camera_pos->x+dir.x/15.0;
        double y_ = camera_pos->y-0.04;
        double z_ = camera_pos->z+dir.z/15.0;

        vec3 flag_pos = SetVector(x_, y_, z_);
        vec3 flag_lookat = SetVector(0, 0, 0);
        check_position(&flag_pos, &flag_lookat);
        check_corner(&flag_pos, &flag_lookat);
        // We add a wobble to give a nicer effect
        model_pos = T(flag_pos.x, flag_pos.y + sin(camera_bump_evolution+3)/(8*WOBBLE_HEIGHT), flag_pos.z);

        // Compute the current absolute angle of the player to rotate the flag in the correct orientation
        double angle = acos(DotProduct(Normalize(diff), SetVector(0, 0, 1)));
        if (DotProduct(Normalize(diff), SetVector(1, 0, 0)) < 0) angle = -angle;
        //printf("angle : %f\n", angle);
        model_rot = Ry(angle);
        model_scale = S(0.01, 0.01, 0.01);
    } 
    else // Just put it on the end cell
    {
        model_pos = T(x+0.5, z+0.1, y+0.5);
        model_rot = Ry(0);
        model_scale = S(0.04, 0.04, 0.04);
    }
   
    
    mat4 model_transform = Mult(model_pos, Mult(model_rot, model_scale));
    
    glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
    DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
    
}

void draw_particles(particle** particles, Model *model, GLuint program)
{
    for (int i = 0; i < N_PARTICLES ; ++i)  
    {
        if (particles[i]->y < 0) continue;
        mat4 model_pos = T(particles[i]->x, particles[i]->y, particles[i]->z);
        mat4 model_rot = Mult(Rx(particles[i]->angle), Rz(2*particles[i]->angle));
        mat4 model_scale = S(0.0001, 0.0001, 0.0001);
        mat4 model_transform = Mult(model_pos, Mult(model_rot, model_scale));
        glUniformMatrix4fv(glGetUniformLocation(program, "transformMatrix"), 1, GL_TRUE, model_transform.m);
        DrawModel(model, program, "in_vertex",  "in_normal", "in_texture");
        
    }

}

//////////////////////////////////////////////////////////////

// End the level, go to the next one
void end_level(vec3* camera_pos, vec3* camera_lookat, vec3* camera_rot)
{	
    // Try to go to the next level

    PlaySoundInChannel(victory_sound, 0);
    free_particles(particles);
    int try = load_level(++CURRENT_LEVEL);
    if (try >= 0) // If there is a next level
    {
        get_start_cell_position(&part_x, &part_y);
	    allocate_particles(&particles, part_x + 0.5, part_y + 0.5);
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


////////////////////////////////////////////////////////////////////////////
/* Particle functions
 * To make the particle simulations
 */
////////////////////////////////////////////////////////////////////////////

void allocate_particles(particle*** array, double x, double y)
{
    // init array
    *array = malloc(N_PARTICLES * sizeof(particle*));

    // Alloc each particle
    for (int i = 0; i < N_PARTICLES ; ++i)
    {
        particle* p = malloc(sizeof(particle));
        reset_particle(p, x, y, i);
        (*array)[i] = p;    
    }
}
void reset_particle(particle* p, double x, double y, int i)
{
    // Put the particle position at the given coordinate, with default values
   
    p->x = x + 0.02 - (double)(rand() % 1000) / 25000.0;
    p->y = -((double)(rand() % 1000) / 200.0);
    p->z = y + 0.02 - (double)(rand() % 1000) / 25000.0;
    p->angle = rand() % 360;
    p->vx = 0.0025 - (double)(rand() % 1000) / 200000.0; 
    p->vy = 0.012 + (double)(rand() % 1000) / 200000.0; 
    p->vz = 0.0025 - (double)(rand() % 1000) / 200000.0; 

}
void simulate_particules(particle** particles, double x, double y)
{
    for (int i = 0; i < N_PARTICLES; ++i)
    {
        particle* p = particles[i];
       
        if (p->y >= 0)
        {
            p->vy-=0.0002; 
            p->x += p->vx + 0.00125 - (double)(rand() % 1000) / 400000.0;
            p->z += p->vz + 0.00125 - (double)(rand() % 1000) / 400000.0;
        } 
        
        // Even if under make it go up
        p->y += p->vy;
        
        // If back to the ground, reset
        if (p->y < 0 && p->vy < 0) 
        {
            reset_particle(p, x, y, i);
        }

        // If close to the ground, stop them to give the effect that they stay a little bit
        if (p->y < 0.03 && p->vy < 0) 
        {
            p->vy = -0.001;
            p->vx*=0.99;
            p->vz*=0.99;
        }
    }
}
void free_particles(particle** particles)
{
    // free particle
    for (int i = 0; i < N_PARTICLES; ++i)
    {
        free(particles[i]);
    }
    // free array
    free(particles);
}
void get_start_cell_position(int* x, int* y)
{
    for (int i = 0; i < SIZE; ++i)
        for (int j = 0; j < SIZE; ++j)
            if (get_xy_cell(i, j) == 'B') {*x = i; *y = j;}     
}
