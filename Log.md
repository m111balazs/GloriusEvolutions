# Progress

## Set up basic OpenGL project


* Created a basic window using GLFW.
* Initialized OpenGL functions with GLAD.
* Successfully rendered a **triangle** on screen.
* Fixed shader compilation and linking errors.
* Learned how to load shader source files from disk.

## Implemented a Shader System


* Created functions to **load, compile, and link shaders** (vertex and fragment).
* Added support for passing uniforms (like matrices) to shaders.

## Built a Camera Class


* Implemented a **first-person style camera** with mouse look.
* Allowed keyboard input (WASD) to move the camera in 3D space.
* Added the ability to toggle mouse capture with the Alt key.

## Applied Transformations Using glm


Used `glm` for matrix math:

* `model` matrix for object transformations.
* `view` matrix from the camera position and orientation.
* `projection` matrix for perspective projection.

## Ground plane added


## Debug GUI added


* Shows FPS
* Shows camera position
* Shows camera angle
* Shows camera zoom
* Shows camera movement speed

## To do next

* Render 3D cube
* Add lighting (directional, point lights) for realistic shading.
* Implement textured materials.
* Expand the camera system (jump, gravity, collisions).
* Build a scene graph or entity system.
* Start designing simple multiplayer sync logic.
