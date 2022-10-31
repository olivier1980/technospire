# TechnoSpire

This is the C++ code that was initally used for building the [TechnoSpire](https://crazydescent.itch.io/technospire) game. I rebuilt the game in Godot for release.

Almost all assets/libraries have been removed. 

This engine works with Lua scripts, and is comparable to the [LÖVE](https://love2d.org/) engine in its functionality, and has features like:
- Parallax layer animation
- Animated sprite support
- Reading assets from a spritemap
- Resolution scaling (by setting a PIXEL_SIZE var)
- Save/load game 
- Protecting game script assets integrity by comparing a md5 hash of the complete script
- 'mode7'- like graphic effects

All these features are built for an 'adventure point'n'click' style game. 

The code is pretty messy at places, but maybe there's something in it you can use. 
