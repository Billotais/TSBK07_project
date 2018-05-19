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

When we started coding, we planned on writing all the code in two files : *main.c* and *utils.c*. We didn't planned for *utils.c* to become so big, and we should probably have organized our code differently, but it would have required too much changes to correct this, so we decided stay to keep it like that. Of course, we also have some code for the shaders : *project.frag and project.vert* for the majority of what we draw, and *sky.frag and sky.vert* specificaly for the skybox.

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

## utils.c

In this file, we implemented the biggest part of our code. From the camera update, to the object interaction, maze generation, sound and drawing optimiztation, we all put it in there. The most important function is *update*. The function is called from main.c in the display loop, and will handle the correct functionning of the game.

All the key actions will be handled here. We can exit the game, go to the next level, move the player and the camera, interact with the objects, and other. All the transformations done to the camera are done in this function, as well as the walking animation.

Following this, we have the function that make the interactions with the maze possible. *check_position* and *check_corner* implement collisions with the walls and check_flag, pickup_score, enable_lever and change_state_door allow the player to change the state of the game.

*reset_flood* and *flood_from_position* implement the drawing optimisation that looks at which cells might be visible from the current position. We then have a few utility functions, to reset the camera, load a level, ...

You can then find all the functions used to draw objects in the maze. There is one for each kind of object, as well as functions to upload the lights to th GPU and to draw the user interface.

To end this, we have function to simulate the particles, followed by the maze generation code and the frustum culling drawing optimization.

# Interessting problems

We will now focus on a a few interresting implementation details, mostly some that weren't really mentionned during the presentation. The presentation is available on the git repository if needed.

## Lightning

For our lightning, we used ambiant, diffuse and specular lighting. We also added attenuation and a spotlight effect, but the most interessting element in the normal mapping. We used a few tricks to make it easier to implement, and we will try to explain them. 

When looking at a normal map texture, we see that it's mostly blue, meaning that the normal vector are oriented to the *z* axis (Normal vector is $(R, G, B)$). This means that the normal map is made for a flat mesh, and that some special calculations are required to map it correctly to the in-game polygons. What we decided to do is to ignore this issue, and only apply normal mapping on flat surfaces. We created the model for a wall in the exact orientation that would correpond to the one in the normal map texture. That, way we could directly use the normal vectors, without transformation. Of course, when we rotate our wall model, wee also rotate the normal vectore, using adequate matrices. 

## Particle system

For our particle system, we decided to put the simulation in the CPU, instead of in the GPU. On the first look, it might be a bad idea if we want to have a lot of particles, but it actually doesn't matter in our case. The biggest problem when increasing the number of partivles is actually the GPU. Indeed, each one of the particle is actually is complete 3d model, and putting 10'000 objects, each one with lighting computation is too much for a integerated GPU. If we would have wanted to have a huge number of particles, we would have had to change the design of them completly, so they would simply be triangles with no lighting. Only once this is done we can start increasing the number of particles and potentially move the simluation to the GPU to, this time, reduce the bottleneck from the CPU.



# Conclusion