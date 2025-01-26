#pragma once

#include "Custom/vec3.h"
#include <SDL2/SDL.h>


typedef struct {

    Vec3 vertices[3];
    SDL_Color color;
    
} Triangle;


Triangle create_triangle(Vec3 vertices[3], SDL_Color color);