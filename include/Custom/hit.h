#pragma once

#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "bvh.h"
#include "Custom/world_object.h"

typedef struct {
    float t;
    Vec3 point;
    Vec3 normal;
    int hit_something;
    ObjectType type;
    Object object;
    // void *object;

} HitRecord;

HitRecord ray_sphere_intersect(Ray ray, Sphere *sphere);
HitRecord ray_triangle_intersect(Ray ray, Triangle *Triangle);
int ray_aabb_intersect(Ray ray, AABB box);
HitRecord ray_bvh_intersect(Ray ray, BVHNode* node);