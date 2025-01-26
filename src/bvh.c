#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Custom/bvh.h"
#include "Custom/ray.h"
#include "Custom/vec3.h"
#include "Custom/hit.h"
#include "Custom/constants.h"
#include "Custom/scene.h"
#include "Custom/world_object.h"
#include "Custom/triangle.h"

//----------------------------------------------------------------------------------------------------

// Main implementation of Bounding Volume Hierarchies construction using Surface Area Heurestics
// Time Complexity - O( nlogn )
// Axis Aligned Bounding Boxes (AABB) are used for storing objects (child AABBs or sphers (if leaf node))
// Surface Area Heurestics (SAH) is used for finding the cost effective split plane for dividing the parent node.

//----------------------------------------------------------------------------------------------------

AABB create_empty_aabb()
{
    return (AABB){
        {INFINITY, INFINITY, INFINITY},
        {-INFINITY, -INFINITY, -INFINITY}};
}

AABB create_aabb_from_sphere(Sphere *sphere)
{
    return (AABB){
        {sphere->center.x - sphere->radius,
         sphere->center.y - sphere->radius,
         sphere->center.z - sphere->radius},
        {sphere->center.x + sphere->radius,
         sphere->center.y + sphere->radius,
         sphere->center.z + sphere->radius}};
}

AABB create_aabb_from_triangle(Triangle *triangle){
    Vec3 v0 = triangle->vertices[0];
    Vec3 v1 = triangle->vertices[1];
    Vec3 v2 = triangle->vertices[2];

    
    Vec3 min = {
        .x = fmin(fmin(v0.x, v1.x), v2.x),
        .y = fmin(fmin(v0.y, v1.y), v2.y),
        .z = fmin(fmin(v0.z, v1.z), v2.z)
    };

    Vec3 max = {
        .x = fmax(fmax(v0.x, v1.x), v2.x),
        .y = fmax(fmax(v0.y, v1.y), v2.y),
        .z = fmax(fmax(v0.z, v1.z), v2.z)
    };

    return (AABB) { .min = min, .max = max};
}

AABB combine_aabb(AABB a, AABB b)
{
    return (AABB){
        {fmin(a.min.x, b.min.x),
         fmin(a.min.y, b.min.y),
         fmin(a.min.z, b.min.z)},
        {fmax(a.max.x, b.max.x),
         fmax(a.max.y, b.max.y),
         fmax(a.max.z, b.max.z)}};
}

float get_aabb_surface_area(AABB box)
{
    Vec3 dimensions = {
        box.max.x - box.min.x,
        box.max.y - box.min.y,
        box.max.z - box.min.z};
    return 2.0f * (dimensions.x * dimensions.y +
                   dimensions.y * dimensions.z +
                   dimensions.z * dimensions.x);
}

float evaluate_sah(Scene *scene, int start, int end, int axis, float split)
{
    int left_count = 0, right_count = 0;
    AABB left_bounds = create_empty_aabb();
    AABB right_bounds = create_empty_aabb();

    for (int i = start; i < end; i++)
    {
        float center = 0;
        switch (axis)
        {
        case 0:
            if (scene->objects->type == SPHERE){
                center = scene->objects[i].data.sphere.center.x;
            }
            else{
                float x1 = scene->objects[i].data.triangle.vertices[0].x;
                float x2 = scene->objects[i].data.triangle.vertices[1].x;
                float x3 = scene->objects[i].data.triangle.vertices[2].x;
                center =  (x1 + x2 + x3) / 3;
            }
            break;
        case 1:
            if (scene->objects->type == SPHERE){
                center = scene->objects[i].data.sphere.center.y;
            }
            else{
                float y1 = scene->objects[i].data.triangle.vertices[0].y;
                float y2 = scene->objects[i].data.triangle.vertices[1].y;
                float y3 = scene->objects[i].data.triangle.vertices[2].y;
                center =  (y1 + y2 + y3) / 3;
            }
            break;
        case 2:
            if (scene->objects->type == SPHERE){
                center = scene->objects[i].data.sphere.center.z;
            }
            else{
                float z1 = scene->objects[i].data.triangle.vertices[0].z;
                float z2 = scene->objects[i].data.triangle.vertices[1].z;
                float z3 = scene->objects[i].data.triangle.vertices[2].z;
                center =  (z1 + z2 + z3) / 3;
            }
            break;
        }

        if (center < split)
        {
            left_count++;
            if (scene->objects->type == SPHERE){
                left_bounds = combine_aabb(left_bounds, create_aabb_from_sphere(&scene->objects[i].data.sphere));
            }
            else{
                left_bounds = combine_aabb(left_bounds, create_aabb_from_triangle(&scene->objects[i].data.triangle));
            }
            
        }
        else
        {
            right_count++;
            if (scene->objects->type == SPHERE){
                right_bounds = combine_aabb(right_bounds, create_aabb_from_sphere(&scene->objects[i].data.sphere));
            }
            else{
                right_bounds = combine_aabb(right_bounds, create_aabb_from_triangle(&scene->objects[i].data.triangle));
            }
        }
    }

    float left_sa = get_aabb_surface_area(left_bounds);
    float right_sa = get_aabb_surface_area(right_bounds);

    return 0.125f + (left_count * left_sa + right_count * right_sa);
}

//----------------------------------------------------------------------------------------------------

// BVH (Bounding Volume Hierarchy) construction using a top-down approach.
// Top-Down BVH Construction :
// - Root Node Creation : contains all spheres as AABB.
// - Object Partition : AABB partition with Surface Area Heurestics (SAH).
// - Child Nodes Creation: Recursive creation and partitioning of child node.
// - Repeat Until Leaf Nodes: Partioning until each subset contains a single sphere
//                           or depth limit is reached (20 here)

//----------------------------------------------------------------------------------------------------

// static void debug_aabb(AABB box, const char* label) {
//     printf("%s: min=(%f,%f,%f), max=(%f,%f,%f)\n", 
//            label, box.min.x, box.min.y, box.min.z,
//            box.max.x, box.max.y, box.max.z);
// }

BVHNode *build_bvh_node(Scene *scene, int start, int end, int depth)
{
    BVHNode *node = (BVHNode *)malloc(sizeof(BVHNode));
    node->bounds = create_empty_aabb();

    for (int i = start; i < end; i++)
    {
        if (scene->objects[i].type == SPHERE){
            node->bounds = combine_aabb(node->bounds, create_aabb_from_sphere(&scene->objects[i].data.sphere));
            
        }
        else{
            node->bounds = combine_aabb(node->bounds, create_aabb_from_triangle(&scene->objects[i].data.triangle));
        } 
        
    }

    int num_objects = end - start;
    // debug_aabb(node->bounds, "Node bounds");


    if (num_objects <= 1 || depth >= 20) {
        node->left = node->right = NULL;
        node->primitive = &scene->objects[start].data;
        node->type = scene->objects[start].type;
        node->object_count = num_objects;
        // printf("Leaf node with %d spheres\n", num_spheres);
        return node;
    }

    float best_cost = INFINITY;
    int best_axis = 0;
    float best_split = 0;

    for (int axis = 0; axis < 3; axis++)
    {
        for (int i = 1; i < 8; i++)
        {
            float split;
            if (axis == 0)
            {
                split = node->bounds.min.x + (i / 8.0f) * (node->bounds.max.x - node->bounds.min.x);
            }
            else if (axis == 1)
            {
                split = node->bounds.min.y + (i / 8.0f) * (node->bounds.max.y - node->bounds.min.y);
            }
            else
            {
                split = node->bounds.min.z + (i / 8.0f) * (node->bounds.max.z - node->bounds.min.z);
            }

            float cost = evaluate_sah(scene, start, end, axis, split);

            if (cost < best_cost)
            {
                best_cost = cost;
                best_axis = axis;
                best_split = split;
            }
        }
    }

    int mid = start;
    for (int i = start; i < end;)
    {
        float center = 0;
        switch (best_axis)
        {
        case 0:
            if (scene->objects->type == SPHERE){
                center = scene->objects[i].data.sphere.center.x;
            }
            else{
                float x1 = scene->objects[i].data.triangle.vertices[0].x;
                float x2 = scene->objects[i].data.triangle.vertices[1].x;
                float x3 = scene->objects[i].data.triangle.vertices[2].x;
                center =  (x1 + x2 + x3) / 3;
            }
            break;
        case 1:
            if (scene->objects->type == SPHERE){
                center = scene->objects[i].data.sphere.center.y;
            }
            else{
                float y1 = scene->objects[i].data.triangle.vertices[0].y;
                float y2 = scene->objects[i].data.triangle.vertices[1].y;
                float y3 = scene->objects[i].data.triangle.vertices[2].y;
                center =  (y1 + y2 + y3) / 3;
            }
            break;
        case 2:
            if (scene->objects->type == SPHERE){
                center = scene->objects[i].data.sphere.center.z;
            }
            else{
                float z1 = scene->objects[i].data.triangle.vertices[0].z;
                float z2 = scene->objects[i].data.triangle.vertices[1].z;
                float z3 = scene->objects[i].data.triangle.vertices[2].z;
                center =  (z1 + z2 + z3) / 3;
            }
            break;
        }

        if (center < best_split)
        {
            Object temp = scene->objects[i].data;
            scene->objects[i].data = scene->objects[mid].data;
            scene->objects[mid].data = temp;
            mid++;
            i++;
        }
        else
        {
            i++;
        }
    }

    node->left = build_bvh_node(scene, start, mid, depth + 1);
    node->right = build_bvh_node(scene, mid, end, depth + 1);
    node->primitive = NULL;
    node->object_count = 0;

    return node;
}
