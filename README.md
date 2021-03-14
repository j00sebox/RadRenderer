# radrenderer

## Description

An implementation of a 3D renderer. Can load objects from .obj files. This project is built off of a project done by the OneLoneCoder called the olcConsoleGameEngine. Big thanks to him for making it publicly available to use.

## How it Works

1. Creates triangles based off the data from the .obj file. 
2. Applies the rotation matrices to each triangle point.
3. Calculates the normal of each triangle, then the angle between each normal and the camera projection to determine if that triangle is visible.
4. Calculates waht colour the triangle should be based on the dot product of the normal and the lighting vector.
5. If it is visible it converts the object to camera space.
6. Runs through a triangle clipping algorithm to break triangles down if apart of it is off screen.
7. Sorts the triangles based on the z value. Triangles that are farther away in 3D space will get renderer first.
8. Draws and colours the triangles onto the screen

End Result:

![Teapot](/screenshots/teapot.PNG)

All the functions for actually drawing and colouring the triangles onto the screen are provided by olcConsoleGameEngine.h by the OneLoneCoder. 
