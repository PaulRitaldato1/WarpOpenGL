# WarpOpenGL

Hobby renderer targeted for OpenGL. The goal of this project was to test ideas, and prototype rendering techniques before I write a 'bigger' renderer. As of now this project is complete, in that it reached its goal . 

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

![Deferred Rendering 500 Lights](https://github.com/PaulRitaldato1/WarpOpenGL/blob/master/500Lights.PNG)

# Post Mortem

First I will talk about the current design, then I will go over how I will change this in later projects. 

Aside from the basic abstractions around the standard buffer types, textures, and other openGL types, there are a few designs that I experimented with. The first one being model/texture loading, and the last being the render pass system.

The model and texture loading system currently uses a thread pool to load all required assets for the given scene. While this does work extremely well for a static scene, if I were to add an editor, or want to add hot loading then this system falls apart a bit. It is a bit clunky in that openGL calls to create textures, and associated openGL buffers need to be within the same context. This prevents the system from being truly parallel, and currently only the literal loading of the meshes/textures into RAM is parallel. Most of this will be resolved by using a more modern API like Dx12 or Vulkan.

The renderpass system was the big experiment of this project. I use it to create a basic scene graph and order the render passes I want (such as gbuffer pass, lighting pass, shadow pass, etc). Currently, there is a bit of clunkiness in that the renderpass manager essentially maps pass inputs and outputs by raw strings. This can probably be optimized a bit more smoothly. The render pass outputs are also using an inheritence chain to cast from a generic type to the actual custom data being passed between passes. I feel this could be avoided by using templates either on the functions adding the passes to the renderpass system OR with C++20 features that allow templated lambdas. This is the area that needs to biggest architectural work. I believe this is a good start but its extremely clunky to use. 

As for other general improvements, I would like too touch on how threading is used in this project. Frankly, its not. OpenGL is extremely limited with threading so the only real performance gains I could muster were from loading models/textures. Now, one use that was not used in this project was the concept of a render, update, and event thread. Currently, all of these things happen in the same thread. This will be addressed in future renderer projects but it is beyond the scope of this one. 

The last architectural improvement I will touch on is allocators. I use the default C++ allocator only. In later projects I will try to define my own in the hopes of performance gains. This is by far not the only improvment that can be made but I would deem it a major upgrade. 
