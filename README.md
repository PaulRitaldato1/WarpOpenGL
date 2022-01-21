# WarpOpenGL

Hobby renderer targeted for OpenGL. The goal of this project was to test ideas, and prototype rendering techniques before I write a 'bigger' renderer. As of now this projectt is complete, in that it reached its goal . 

# Current features/experiments 
- Deferred rendering: This is a first attempt at running a deferred renderer using light volumes. A basic defferred renderer with a gbuffer consisting of albedo, spec, normal, and position information is what is currently used.
- Instanced rendering: The ability to set a mesh as instanced
- Basic render pass system: Allows easy construction of a frame
- Scene management: Basic scene management that the render passes can modify/update
- Asyncronous model and texture loading: allows for highly parallel loading of large models and textures

# Tools
- glad
- GLFW
- stb_image.h
- Assimp
- C++
- openGL
- glm

![Deferred Rendering 500 Lights](https://github.com/PaulRitaldato1/WarpOpenGL/500Lights.PNG)

#Post Mortem

