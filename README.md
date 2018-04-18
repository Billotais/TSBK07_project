# TSBK07 project

## 3D Maze Game
### How to run

Go into the *src* folder, and run

```
$ make
```

To compile and run the project.

### Participants:
- Felix Eklöf felek877@student.liu.se
- Loïs Bilat loibi806@student.liu.se

We will make a maze game, in first-person perspective. The maze is 2D and based on a grid.

### Will do

- Skybox **done**
- Textured walls and floor and objects **Mostly done, still need to find great textures for score objects and levers, ...**
- Collission detection camera-walls **done**
- Simple geometry for objects, like boxes and spheres **done, we can pick score objects, flag, ...**
- First-person view **done, with walking animation w/ and w/o flag**
- Light sources **done, might need to reduce radius so it doesn't bleed through walls, one on player, score objects and start/end**
- Spotlight source following the player **done**
- Normal mapping **done for flat surfaces**
- Import OBJ objects **done**
- Score objects **done, text indicator included**
- Dynamic maze (open / close some walls) **done, lever will close opend doors and vice versa**

#### Do later

- Maze descibed by a text file, stored as a 2D array. **done**
- Multi level management **done, with automatic level switching, correct positioning and direction of player on spawn**
- We can pick the flag and take it with us **done**
- Special graphics for the goal

### Will most likely do

- Sound effects **done, just need to find nice sounds**
- Animation during level switching

### Might do

- Enemies
- Obstacles (Jump above a hole or fence), traps
- Drawing optimized **done with a simple calling where we only draw "half" of the maze, depending on where we look**
- Random generated maze

### Type of blocks

X: Wall
0: Floor
B: Beggining
E: End
S: Score object
I: Interruptor not pressed
i: interruptor pressed
D: Door close
d: Door open

### Textures

We used the following [website](https://www.textures.com/browse/3d-scans/114548) to get bumped textures.
