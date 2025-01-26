#pragma once

#include "Custom/vec3.h"
#include "Custom/sphere.h"
#include "Custom/ray.h"
#include "Custom/scene.h"
#include "Custom/triangle.h"

typedef struct AABB {
    Vec3 min;
    Vec3 max;
} AABB;

typedef struct BVHNode {
    AABB bounds;
    struct BVHNode* left;
    struct BVHNode* right;
    void *primitive;
    ObjectType type;
    int object_count;
} BVHNode;


AABB create_empty_aabb();
AABB create_aabb_from_sphere(Sphere* sphere);
AABB create_aabb_from_triangle(Triangle* triangle);
AABB combine_aabb(AABB a, AABB b);
float get_aabb_surface_area(AABB box);
float evaluate_sah(Scene* scene, int start, int end, int axis, float split);
BVHNode* build_bvh_node(Scene* scene, int start, int end, int depth);

