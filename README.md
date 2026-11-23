# 3D Card Game Scene (OpenGL & C++)

An interactive 3D room and card-playing scene built from scratch in modern C++ and OpenGL 3.3.

## Features
- **Custom 3D Environment:** Textured wood floor, walls, table, chairs, potted cactus, and custom-mapped torn wall posters.
- **Smooth Card Animations:** Frame-rate independent linear interpolation (`lerp`) for sliding, rotating, and flipping cards onto the center pile.
- **First-Person Camera:** Free-look mouse controls and smooth WASD movement around the room.

## Controls

| Key / Input | Action |
| :--- | :--- |
| **Mouse** | Look around |
| **W / A / S / D** | Move camera forward, left, backward, right |
| **G** | Throw/flip a random card into the center |
| **R** | Reset the cards back to starting positions |
| **ESC** | Close the application |

## Built With
- **C++ & OpenGL 3.3 Core**
- **GLFW** (Window & Input)
- **GLAD** (OpenGL Function Loader)
- **GLM** (3D Vector & Matrix Math)
- **stb_image** (Texture Loading)
