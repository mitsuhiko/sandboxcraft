@echo off

if not exist vs\nul mkdir vs
cd vs

set SDLDIR=win_deps/SDL
set SDLIMAGEDIR=win_deps/SDL_image
cmake ..
cd ..
