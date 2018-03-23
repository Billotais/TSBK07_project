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

- Skybox Loï **done**
- Textured walls and floor and objects. Loïs
    Floor, walls, door, bunny
- Collission detection camera-walls. Felix Camera position 0.1 < x < 0.9g
    - get_xy_cell(x, y) => return type of cell
    - get_adgacent(x, y) => return type of 4 adgacents cell (N, W, S, E)
- Simple geometry for objects, like boxes and spheres Felix 0.45 < x < 0.55
- First-person view **done**
- Light sources Loïs Su **done ?**
- Import OBJ objects Loïs**done**
- Score objects Felix Star
- Spotlight source following the player. Loïs Point light on player
- Dynamic maze (open / close some walls) Felix

#### Do later
- Maze descibed by a text file, stored as a 2D array.
- Special graphics for the goal
### Will most likely do


X: Wall
0: Floor
B: Beggining
E: End
S: Score object
I: Interruptor
D: Door

- Random generated maze


### Might do

- Sound effects
- Obstacles (Jump above a hole or fence), traps
- Switch between first and third person view.
- Drawing optimized
