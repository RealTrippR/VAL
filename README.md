# Vulkan Abstraction Library
### VAL is still in the prototype stage and likely contains bugs; use it at your own risk.

The Vulkan Abstraction Library is an experimental abstraction layer written atop the Vulkan Graphics API written almost exclusively in C++ 20. It was built to simplify the process of developing and maintaining Vulkan programs while still keeping most of the functionality and performance that Vulkan provides.

<img src="https://github.com/user-attachments/assets/fdfbd756-829b-446d-a9b2-ecf52f594aab" 
        alt="Picture" 
        width="500" 
        height="500" 
        style="display: block; margin: 0 auto" />
        

# Examples
VAL/Tests contains several basic examples of the features provided by VAL, including but not limited to:

- Graphics Pipelines
- Compute Pipelines
- Subpasses
- Render Targets
- Push Constants
- Image Samplers (Combined, Immutable, Standalone), Image Views, Images
- Mipmapping & Anisotropy
- Depth Buffering
- Instancing
- Dynamic States (SCISSOR, VIEWPORT, LINE_WIDTH, DEPTH BIAS, DEPTH_BOUNDS, BLEND_CONSTANTS, CULL_MODE, TOPOLOGY)
- SSBOS
- UBOS
- Array bindings
- Utilities for loading .obj meshes
- Specialization Constants

# Building and Linking
Compiling VAL is rather straighforward; included a python script to install all external dependencies (except for the Vulkan SDK, the instructions to install this are given in the dependency installer)
```
VAL\TOOLS> py installDeps.py
```
VAL builds to a linked library titled "VAL.lib" located in your configuration directory.
For example, if you're building for x64 and debug, the VAL .lib will be located under "VAL/x64/Debug"

See VAL/Tests or VAL-Samples
# Contributing
The Vulkan Abstraction Library is in a relatively early stage of development and help is greatly appreciated.
To get started with development, refer to VAL/notes/to-do.txt and the issues page.

The Vulkan Abstraction Library is missing the following key features:
```
- Proper optimization for Semaphore wait stages
- Raytracing support
- Multi-threaded rendering
- SSBO & UBO array binding
- Externally Sourced Buffers
- Dynamic Rendering
```
