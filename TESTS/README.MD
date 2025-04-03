## VAL TESTS ##
This folder contains a series of tests to validate the functionality of the Vulkan Abstraction Library.
Included within this is a python script to automatically compile GLSL shaders to SPIR-V, tools/compileShaders.py

The following tests are incomplete and may require updates to the VAL:
```
raytracingTest
forwardRendering
immutableSamplerTest
multithreadingTest
UBO_SSBO_ArrayTest
```

The following tests are outdated and need to be updated before they can be run:
```
renderpassToImage
specializationConstants
shaderInput
MipMapTest
Z-PrepassTest
```