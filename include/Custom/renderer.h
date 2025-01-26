#pragma once

#include <SDL2/SDL.h>
#include "Custom/ray.h"
#include "Custom/sphere.h"
#include "Custom/bvh.h"
#include "Custom/scene.h"

SDL_Color trace_ray(Ray ray, Scene *scene, int num_objects, int depth, BVHNode* bvh);