# radrenderer

## Description

An implementation of a 3D renderer. Can load objects from .obj files.

## How it Works

1. Creates triangles based off the data from the .obj file. 
2. Applies transformations to object
3. Does all necessary clipping
4. Performs a perpective projection on all vertices
5. Sorts triangles based on z values
6. Rasterizes each triangle from back to front using an edge function
7. Writes pixels to a frame buffer that is displayed by SFML

End Result:

![Teapot](/screenshots/teapot.PNG)

## Things I Want To Do

- Add depth buffer
- Implement Gouraud shading
- SIMD math operations
- Textures

