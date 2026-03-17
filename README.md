# RadRenderer

## Description

My attempt to make a software renderer to learn more about the algorithms behind graphics. It can load gltf or obj file formats.

I implemented a simple rasterization algorithm that does the classic World -> View -> Projection transforms to get the vertices into screen space where I then figure out how to colour each pixel. The lighting is nothing fancy just a simple diffuse lighting calculation that is baked into the rasterization step. To improve performance I am using a vectorized way of doing the vertex transform with the desired matrix and am using a tile-based approach with multi-threading.

End Result:

<img width="2560" height="1440" alt="image" src="https://github.com/user-attachments/assets/8822026b-d5f5-4b24-9424-aec792483dca" />

<img width="2560" height="1440" alt="image" src="https://github.com/user-attachments/assets/2acc8023-2bb3-4b90-8e75-9977eecd82a9" />

## Controls

- Hold down RMB and use the mouse to look around.

- While holding RMB can move around using WASD.

- Q will open a file dialog that allows selection of a model to load.

- ESC to close the window.

## Assets Included

OBJ models (Teapot, Stanford Bunny) from [common-3d-test-models](https://github.com/alecjacobson/common-3d-test-models).

glTF models (Sponza, Avocado) from [glTF-Sample-Models](https://github.com/KhronosGroup/glTF-Sample-Models).
