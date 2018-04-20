/* Maze generator in C.
 * Joe Wingbermuehle
 * 19990805
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define EMPTY 0
#define WALL 1
#define SOLVE 2
#define START 3
#define END 4 
#define FLOOD 5
#define OTHER 6
#define OPEN 7
#define CLOSE 8

#define SIZE 21

int max_dist = 0;
int max_x = 0;
int max_y = 0;

/* Display the maze. */


void print_maze(const char *maze, int width, int height) {
   int x, y;
   for(y = 0; y < height; y++) {
      for(x = 0; x < width; x++) {
         switch(maze[y * width + x]) {
         case WALL:   printf("X");  break;
         case SOLVE:  printf("-");  break;
         case EMPTY:  printf(" ");  break;
         case START:  printf("B");  break;
         case END:    printf("E");  break;
         case OPEN:   printf("D");  break;
         case CLOSE:  printf("d");  break;
         default:     printf(" ");  break;
         }
      }
      printf("\n");
   }
}

/*  Carve the maze starting at x, y. */
void carve_maze(char *maze, int width, int height, int x, int y) {

   int x1, y1;
   int x2, y2;
   int dx, dy;
   int dir, count;

   dir = rand() % 4;
   count = 0;
   while(count < 4) {
      dx = 0; dy = 0;
      switch(dir) {
      case 0:  dx = 1;  break;
      case 1:  dy = 1;  break;
      case 2:  dx = -1; break;
      default: dy = -1; break;
      }
      x1 = x + dx;
      y1 = y + dy;
      x2 = x1 + dx;
      y2 = y1 + dy;
      if(   x2 > 0 && x2 < width && y2 > 0 && y2 < height
         && maze[y1 * width + x1] == 1 && maze[y2 * width + x2] == 1) {
         maze[y1 * width + x1] = 0;
         maze[y2 * width + x2] = 0;
         x = x2; y = y2;
         dir = rand() % 4;
         count = 0;
      } else {
         dir = (dir + 1) % 4;
         count += 1;
      }
   }

}

/* Generate maze in matrix maze with size width, height. */
void generate_empty(char *maze, int width, int height) {

   int x, y;

   /* Initialize the maze. */
   for(x = 0; x < width * height; x++) {
      maze[x] = 1;
   }
   maze[1 * width + 1] = 0;

   /* Seed the random number generator. */
   srand(time(0));

   /* Carve the maze. */
   for(y = 1; y < height; y += 2) {
      for(x = 1; x < width; x += 2) {
         carve_maze(maze, width, height, x, y);
      }
   }

   /* Set up the entry and exit. */

}


/* Solve the maze. */
void solve_maze(char *maze, int width, int height) {

   int dir, count;
   int x, y;
   int dx, dy;
   int forward;

   /* Remove the entry and exit. */
   maze[1 * width + 1] = EMPTY;
   maze[max_y * width + max_x] = EMPTY;

   forward = 1;
   dir = 0;
   count = 0;
   x = 1;
   y = 1;
   while(x != max_x || y != max_y) {
      dx = 0; dy = 0;
      switch(dir) {
      case 0:  dx = 1;  break;
      case 1:  dy = 1;  break;
      case 2:  dx = -1; break;
      default: dy = -1; break;
      }
      if(   (forward  && maze[(y + dy) * width + (x + dx)] == EMPTY)
         || (!forward && maze[(y + dy) * width + (x + dx)] == SOLVE)) {
         maze[y * width + x] = forward ? SOLVE : OTHER;
         x += dx;
         y += dy;
         forward = 1;
         count = 0;
         dir = 0;
      } else {
         dir = (dir + 1) % 4;
         count += 1;
         if(count > 3) {
            forward = 0;
            count = 0;
         }
      }
   }

   /* Replace the entry and exit. */
   maze[1 * width + 1] = START;
   maze[max_y * width + max_x] = END;
   
}



void reset_generate_end(char *maze, char* maze_flood, int width, int height)
{
    for (int x = 0; x < height; ++x)
        for (int y = 0; y < width; ++y)
            maze_flood[y*width+x] = maze[y*width+x];
}
void generate_end(char *maze_flood, int width, int height, int x, int y, int count)
{
    if (maze_flood[y*width+x] == EMPTY)
    {
        maze_flood[y*width+x] = FLOOD;
        if (count > max_dist)
        {
            max_dist = count;
            max_x = x;
            max_y = y;
        }
        generate_end(maze_flood, width,height, x+1, y, count+1);
        generate_end(maze_flood, width,height, x-1, y, count+1);
        generate_end(maze_flood, width,height, x, y+1, count+1);
        generate_end(maze_flood, width,height, x, y-1, count+1);
    }
}

void generate_doors(char *maze, int width, int height, int n)
{
    int o_c = 0;
    int steps = (int)((double)max_dist / ((double)n));
    int curr_step = 0;
    int curr = START;
    int x = 1;
    int y = 1;
    

    while (curr != END)
    {
        if (curr != START) maze[y*width+x] = EMPTY;
        if (curr_step == steps) 
        {
            maze[y*width+x] = (o_c ? CLOSE : OPEN);
            o_c = !o_c;
            curr_step = 0;
        }
        
        if (maze[(y+1)*width+x] == SOLVE || maze[(y+1)*width+x] == END) y++;
        else if (maze[(y-1)*width+x] == SOLVE || maze[(y-1)*width+x] == END) y--;
        else if (maze[y*width+x+1] == SOLVE || maze[y*width+x+1] == END) x++;
        else if (maze[y*width+x-1] == SOLVE || maze[y*width+x-1] == END) x--;
        

        curr = maze[y*width+x];
        curr_step++;
    }

}

int create_maze() {

    int width = SIZE, height = SIZE;
    char *maze;
    char *maze_flood;

   

    /* Allocate the maze array. */
    maze = (char*)malloc(SIZE * SIZE * sizeof(char));
    maze_flood = (char*)malloc(SIZE * SIZE  * sizeof(char));
    if(maze == NULL || maze_flood == NULL) {
        printf("error: not enough memory\n");
        exit(EXIT_FAILURE);
    }

    /* Generate and display the maze. */
    generate_empty(maze, width, height);

    reset_generate_end(maze, maze_flood, width, height);
    generate_end(maze_flood, width,height,1,1,0);

    maze[1*width+1] = START;
    maze[max_y*width+max_x] = END;
    //print_maze(maze, width, height);

    solve_maze(maze, width, height);
    //print_maze(maze, width, height);
    generate_doors(maze, width, height, 3);
    print_maze(maze, width, height);

    /* Clean up. */
    free(maze);
    free(maze_flood);
    

}
int main()
{
    create_maze();
    return 0;
}

