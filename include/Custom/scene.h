#pragma once

#define MAX_OBJECTS 100000

#include "Custom/world_object.h"
#include <stdbool.h>

typedef struct{
    WorldObject objects[MAX_OBJECTS];
    size_t count;
    size_t maxCount;
} Scene;

bool add_object_to_scene(Scene* scene, const WorldObject* object);