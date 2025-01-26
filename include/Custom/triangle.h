#pragma once

#include "Custom/vec3.h"
#include <SDL2/SDL.h>


typedef struct {

    Vec3 vertices[3];
    Vec3 normals[3];
    SDL_Color color;
    
} Triangle;


Triangle create_triangle(Vec3 vertices[3], SDL_Color color);
Triangle create_triangle_with_normals(Vec3 vertices[3], Vec3 normals[3], SDL_Color color);

Vec3 interpolate_normal(Triangle* triangle, float u, float v, float w);