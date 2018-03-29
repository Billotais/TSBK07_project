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
- Textured walls and floor and objects **Mostly done, still need to find great textures for score objects, ...**
    Floor, walls, door, bunny
- Collission detection camera-walls. Felix Camera position 0.1 < x < 0.9g
    - get_xy_cell(x, y) => return type of cell
    - get_adgacent(x, y) => return type of 4 adgacents cell (N, W, S, E)
- Simple geometry for objects, like boxes and spheres Loïs 0.45 < x < 0.55 **done, we can pick score objects**
- First-person view **done**
- Light sources **Almost done, fixed light in multiple cells, still need to move the list of lights from CPU to GPU**
- Spotlight source following the player **done**
- Bump mapping **done, on walls, ground, ...**
- Import OBJ objects Loïs **done**
- Score objects **Almost done, we can pick them up and the score increases, but no indicator yet**

- Dynamic maze (open / close some walls) **done, lever will close opend doors and vice versa**

#### Do later

- Maze descibed by a text file, stored as a 2D array.
- Special graphics for the goal

### Will most likely do

- Sound effects
- Animation during level switching

### Might do

- Enemies
- Obstacles (Jump above a hole or fence), traps
- Switch between first and third person view.
- Drawing optimized
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