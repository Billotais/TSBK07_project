    # TSBK07 project

## The Quest for the Holy Grail - 3D Maze Game

Created by [Felix Eklöf](mailto:felek877@student.liu.se) and [Loïs Bilat](mailto:loibi806@student.liu.se) for the TSBK07 - Computer Graphics course project at Linköping Universitet

### How to run

Go into the *src* folder, and run

```
$ make
```

to compile and run the project. The GCC compiler and OpenGL are needed. All other libraries are included in the repository. Tested on Linux using GCC 7.3.1 - OpenGL 4.5 - Mesa 18.0.1

### About

This is a 3D Maze game, where you have to find the Holy Grail and bring it back. Some paths are closed and require you to find a lever to open the door. To improve your score, find the chest hidden inside the maze. 

To move, use the **WASD** keys. To rotate the camera, use the arrow keys. You can activate levers using the **E** key.

### Sources

We used the [Textures.com](https://www.textures.com/browse/3d-scans/114548) to get our textures.

Models where found on [Turbosquid](https://www.turbosquid.com/), [cgtrader](https://www.cgtrader.com/free-3d-models) and [Free3D](https://free3d.com/).

Sound samples where downloaded from [freesound](https://freesound.org/).

Source code for the maze generator was inspired by [joewing on GitHub](https://github.com/joewing/maze). Source code for Bresenham's line drawing algorithm is taken from [Wikipedia](https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm)

The [MicroGlut API](http://www.ragnemalm.se/lightweight/) was used for this project, as well as the following files and libraries, all provided by [Ingemar Ragnemalm](http://computer-graphics.se/TSBK07/) :

- CallMeAL
- simplefont
- GL_utilities
- loadobj
- laodTGA
- VectorUtils3

### Implemented fonctionalities

- Skybox
- Textured walls and floor and objects
- Collission detection camera-walls
- Simple geometry for objects, like boxes and spheres, abbility to interract with score objects, flag, ...
- First-person view 
- Light sources : one on player, score objects, levers and start/end, updates when objects in the maze change, optimized to only applied to close enough cells
- Spotlight source following the player
- Normal mapping for flat surfaces (walls, ground)
- Import OBJ objects
- Score objects, with UI indicator
- Dynamic maze (open / close some walls) when using the lever
- Maze descibed by a text file, stored as a 2D array. 
- Multi level management, with automatic level switching, correct positioning and direction of player on spawn
- We can pick the flag and take it with us, has an animation and collision with walls
- Special graphics for the goal: Gold coin fountain on the start cell when holding the cup, with the coins staying on the floor for a while
- UI : score, level, objective and interactive button indications shown.
- Sound effects : ambiant and interactions
- Drawing optimized : done using 2D frustum calling, and custom flood-bassed algorithm  
- Random generated maze : maze, start, end and door are generated, as well as lever and scores.
