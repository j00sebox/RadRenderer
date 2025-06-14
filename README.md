# radrenderer

## Description

Trying to apply my knowledge of rendering to make a rasterizer from scratch. This is just for fun and learning not planning on making any triple A games.
So far it can load vertex position info from an .obj file and render it on screen. The only outside library I am currently using is SFML to create a
window and display the image; I also use it to handle events. 

## How it Works

1. Creates triangles based off the data from the .obj file. 
2. Applies model and view transformations to object
3. Does all necessary clipping
4. Performs a perpective projection on all vertices
5. Does back face culling of triangles
6. Rasterizes each triangle using an edge function to figure out what pixels are in the triangle
7. Determines if pixels should be drawn using a depth buffer
8. Writes pixels to a frame buffer that is displayed by SFML

End Result:

![Teapot](/screenshots/teapot.png)


