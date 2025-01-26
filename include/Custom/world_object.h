#pragma once

#include "Custom/sphere.h"
#include "Custom/triangle.h"

typedef enum {
    SPHERE,
    TRIANGLE

} ObjectType;

typedef union {
    Sphere sphere;
    Triangle triangle;
} Object;



typedef struct{
    ObjectType type;
    Object data;

} WorldObject;