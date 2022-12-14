# Obsidion

An Open source 3D Game Engine using Vulkan API. Language used is C++. The Vulkan Initialization separate from the SDL windowing. You can effortlessly switch windowing system to GLFW or SFML for example. The Graphics API can also be effortlessly switched to OpenGL or Metal or DX11 or DX12 (That is if you already have a working abstraction of the said API). In  fact, I have a working OpenGL abstraction of the Engine.\\

The Engine uses GLM math library which you can also effortlessly substitute with a custom one. It has standalone Sound System(Based on OpenAL) and Input system(based on SDL input) which are all pluggable.\
The idea is to have seperate abstractions as "plugins" and be able to customise as desired or use a different one entirely.\\

The Compiler in use is GCC. You can use any lightweight text editor. You can compile, build and run your app/game by running simple batch files(This can also be easily automated in editor of choice).\\

The goal is to make it into a code-only Game Engine/Editor which we simply launch, write our games, build and run from within. I have written a similarly light-weight text editor but yet to integrate it with the Engine.\\

Things that Obsidion Engine are currently capable of:\
--    Draw basic hard-coded 3D Geometrical shapes\
--    Load 3D Models OBJ, GLTF supported\
--    Load GLTF skeletal Animations\
--    Play sounds\
--    Lighting system\
--    Material\
--    Skybox\
--    Free fly Camera\
--    Third Person camera with Basic Player movement\\


![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/tangent_test.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/wolf.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/spider.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/scene.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/ninja.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/heightmapped_terrain.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/gltf.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/control.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/camera.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/Cam.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/suzane.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/TPCamera.gif?raw=true)
![](https://raw.githubusercontent.com/nahim-yay/Obsidion/master/Captures/gltf_woman.gif?raw=true)