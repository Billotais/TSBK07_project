---
title: Computer Graphics - TSBK07 - Project report
author: Loïs Bilat, Felix Eklöf
date: \today
--- 

# Introduction

We wanted to create a game, and a maze seemed to be an idea that could be relatively easily achieved, while allowing us to add as many features as we wanted if we had enough time. So we decided to develop a 3D - first person maze game, where the maze itself is based on a 2D grid. In the original specification list, we planned to implement at least basic graphical functionalities as done in the labs (skybox, lightning, import OBJ files, textures). For the gameplay, we wanted collision with the walls, some score objects we could interact with, special graphics for the goal and we wanted to maze to be described in a text file, allowing us to maybe implement a random maze generator. We were thinking about making the maze dynamic, adding some sound effects and doing some drawing optimization, but this wasn't in our *Will do* list. 

In the end, we ended implementing all the functionalities described above, as well as a user interface, and some improvments to the lightning system compared to the one done in the labs.

What we have now : TODO

# Background information
 ? 

# Implementation details

The whole project was done in C, using the lab libraries (MicroGLut, GL_utilities, VectorUtils3, loadobj, loadTGA) as well as aditional ones from the course website (CallMeAL, simplefont).

When we started coding, we planned on writing all the code in two files : *main.c* and *utils.c*

## main.c 

In this, we had the OpenGL initialization, and the main drawing loop. Our goal was to make this file as small a possible, and to do most of the work using functions defined in *utils.c*.

In the *init* function, we do the following actions :

- We initialize the CallMeAL and simplefont libraries
- We load all the models and textures, and bind them correctly to the texture units. For each texture that will be applied on a flat surface (walls, floor, ...), we have two textures (Albedo and normal map). This will be used in the shaders to create a better lighting effect. Each texture has its own texture units. Since we don't have to much textures, we can allow ourselves to use one unit for each texture. This will remove the need to change textures associated to each unit too many times. 
- We set a few default matrices (The camera matrix is initialized depending on the first level, the projection matrices are uploaded to th GPU, and the created some base position and rotation matricies for the maze "building blocs").

The *display* function is were all the interesting code of the main file can be found. It can be split into three parts :

- First, we upload some data to the GPU that will be needed later. We update the camera acording to the keyboard inputs, upload matrices for the skybox
- Then, we have to main drawing loop. We go through each cell of our maze to draw it. If the current cell is inside a wall, we simply ignore it. Otherwise, we start to draw all the elements one by one. We draw the floor, walls on the sides that require one, eventually doors, and then, once we have to maze structure drawn, we can draw some objects; levers to open the door, score objects and special goal graphics. For every cell, we simply need to look into the 2D maze array to see what we have to draw. For walls, we created functions such as *wall_north, wall_east*, to easily know if we have to draw walls on the side of the cell.
- The last section isn't necessary to the project, since it simply prints some statistics about the drawing optimisation we implemented. But we though it was nice to have it inside the game to be able to more easily understand and demonstrate was is happening. 

# Interessting problems

# Conclusion