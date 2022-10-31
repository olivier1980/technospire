# TechnoSpire

This is the C++ code that was initally used for building the [TechnoSpire](https://crazydescent.itch.io/technospire) game. In the end I rebuilt the game in [Godot](https://godotengine.org/) for release.

Almost all assets/libraries have been removed. 
Libraries used were:
- SDL sound, images etc
- Imgui
- MD5 hashing
- Base64 encoder/decoder
- GLM vector libraries
- SOL for Lua integration
- LUA
- JSON parser

This engine works with Lua scripts, and is comparable to the [LÖVE](https://love2d.org/) engine in its functionality, and has features like:
- Parallax layer animation
- Animated sprite support
- Z sorting of layers
- Reading assets from a spritemap
- Resolution scaling (by setting a PIXEL_SIZE var)
- Save/load game 
- Protecting game script assets integrity by comparing a md5 hash of the complete script
- 'mode7'- like graphic effects

All these features are built for an "adventure point'n'click" style game. I left one of the game scripts as an example, see `assets/scripts` folder.

The code is pretty messy at places, but maybe there's something in it you can use. 
