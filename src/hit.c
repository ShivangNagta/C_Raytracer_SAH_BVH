#include "Custom/hit.h"
#include "Custom/vec3.h"
#include "Custom/constants.h"
#include "Custom/bvh.h"
#include <math.h>
#include <stdio.h>

//

// ISSUE : A NODE CAN CONTAIN MORE THAN ONE PRIMTIVE, BUT ONLY SINGLE INTERSECTION TEST IS BEING PERFORMED;

//

//--------------------------------------------------------------------------------------------------

// ray_sphere_intersect() - Returns the Hitrecord of the ray with the sphere
// Hitrecord is a struct containing
// - t (distance of the hitpoint from the camera initially, then the distance of the next hitpoint from the current hitpoint)
// - point (position of the current hitpoint)
// - normal (normal to the surface at hitpoint - point)
// - hit_something ( bool in terms of int)
// - sphere* (pointer to current sphere object)

//--------------------------------------------------------------------------------------------------

HitRecord ray_sphere_intersect(Ray ray, Sphere *sphere) {
    HitRecord rec = {0};
    Vec3 oc = vec3_sub(ray.origin, sphere->center);
    float a = vec3_dot(ray.direction, ray.direction);
    float b = 2.0f * vec3_dot(oc, ray.direction);
    float c = vec3_dot(oc, oc) - sphere->radius * sphere->radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant > 0) {
        float t = (-b - sqrt(discriminant)) / (2.0f * a);
        if (t > EPSILON) {
            rec.hit_something = 1;
            rec.t = t;
            rec.point = vec3_add(ray.origin, vec3_multiply(ray.direction, t));
            rec.normal = vec3_normalize(vec3_sub(rec.point, sphere->center));
            rec.type = SPHERE;
            rec.object.sphere = *sphere;
            return rec;
        }
    }
    return rec;
}

//

// Ray Triangle Intersection

//

HitRecord ray_triangle_intersect(Ray ray, Triangle *triangle) {
    HitRecord rec = {0};
    
    // Edge vectors
    Vec3 edge1 = vec3_sub(triangle->vertices[1], triangle->vertices[0]);
    Vec3 edge2 = vec3_sub(triangle->vertices[2], triangle->vertices[0]);
    
    // Calculate determinant (ray direction cross edge2)
    Vec3 h = vec3_cross(ray.direction, edge2);
    float a = vec3_dot(edge1, h);
    
    // Check if ray is parallel to triangle
    if (fabs(a) < EPSILON) {
        return rec;
    }
    
    // Calculate barycentric coordinate u
    float f = 1.0f / a;
    Vec3 s = vec3_sub(ray.origin, triangle->vertices[0]);
    float u = f * vec3_dot(s, h);
    
    // Check if intersection point is outside triangle
    if (u < 0.0f || u > 1.0f) {
        return rec;
    }
    
    // Calculate barycentric coordinate v
    Vec3 q = vec3_cross(s, edge1);
    float v = f * vec3_dot(ray.direction, q);
    
    // Check if intersection point is outside triangle
    if (v < 0.0f || u + v > 1.0f) {
        return rec;
    }
    
    // Calculate distance to intersection point
    float t = f * vec3_dot(edge2, q);
    
    // Check if intersection point is in front of ray origin
    if (t > EPSILON) {
        rec.hit_something = 1;
        rec.t = t;
        rec.point = vec3_add(ray.origin, vec3_multiply(ray.direction, t));
        
        // Calculate surface normal
        rec.normal = vec3_normalize(vec3_cross(edge1, edge2));

        rec.type = TRIANGLE;
        
        rec.object.triangle = *triangle;
        return rec;
    }
    
    return rec;
}


// HitRecord ray_triangle_intersect(Ray ray, Triangle *triangle) {
//     HitRecord rec = {0};
    
//     // Edge vectors
//     Vec3 edge1 = vec3_sub(triangle->vertices[1], triangle->vertices[0]);
//     Vec3 edge2 = vec3_sub(triangle->vertices[2], triangle->vertices[0]);
    
//     // Calculate determinant (ray direction cross edge2)
//     Vec3 h = vec3_cross(ray.direction, edge2);
//     float a = vec3_dot(edge1, h);
    
//     // Check if ray is parallel to triangle
//     if (fabs(a) < EPSILON) {
//         return rec;
//     }
    
//     // Calculate barycentric coordinate u
//     float f = 1.0f / a;
//     Vec3 s = vec3_sub(ray.origin, triangle->vertices[0]);
//     float u = f * vec3_dot(s, h);
    
//     // Check if intersection point is outside triangle
//     if (u < 0.0f || u > 1.0f) {
//         return rec;
//     }
    
//     // Calculate barycentric coordinate v
//     Vec3 q = vec3_cross(s, edge1);
//     float v = f * vec3_dot(ray.direction, q);
    
//     // Check if intersection point is outside triangle
//     if (v < 0.0f || u + v > 1.0f) {
//         return rec;
//     }
    
//     // Calculate distance to intersection point
//     float t = f * vec3_dot(edge2, q);
    
//     // Check if intersection point is in front of ray origin
//     if (t > EPSILON) {
//         rec.hit_something = 1;
//         rec.t = t;
//         rec.point = vec3_add(ray.origin, vec3_multiply(ray.direction, t));
        
//         // Calculate barycentric coordinate w
//         float w = 1.0f - u - v;
        
//         // Interpolate normal using barycentric coordinates
//         rec.normal = vec3_normalize(vec3_add(
//             vec3_add(
//                 vec3_multiply(triangle->normals[0], w),
//                 vec3_multiply(triangle->normals[1], u)),
//             vec3_multiply(triangle->normals[2], v)
//         ));

//         rec.type = TRIANGLE;
//         rec.object.triangle = *triangle;
//         rec.barycentric_u = u; // Store u for later use
//         rec.barycentric_v = v; // Store v for later use
//         rec.barycentric_w = w; // Store w for later use
//         return rec;
//     }
    
//     return rec;
// }




//--------------------------------------------------------------------------------------------------

// ray_aabb_intersect() - Returns 1 if the given AABB is hit with the ray otherwise 0
// Performs slab test to find the intersection

//--------------------------------------------------------------------------------------------------


int ray_aabb_intersect(Ray ray, AABB box) {
    float tx1, tx2, ty1, ty2, tz1, tz2;
    float tmin, tmax;
    

    if (ray.direction.x == 0.0f) {
        tx1 = -INFINITY;
        tx2 = INFINITY;
    } else {
        tx1 = (box.min.x - ray.origin.x) / ray.direction.x;
        tx2 = (box.max.x - ray.origin.x) / ray.direction.x;
    }
    
    if (ray.direction.y == 0.0f) {
        ty1 = -INFINITY;
        ty2 = INFINITY;
    } else {
        ty1 = (box.min.y - ray.origin.y) / ray.direction.y;
        ty2 = (box.max.y - ray.origin.y) / ray.direction.y;
    }
    
    if (ray.direction.z == 0.0f) {
        tz1 = -INFINITY;
        tz2 = INFINITY;
    } else {
        tz1 = (box.min.z - ray.origin.z) / ray.direction.z;
        tz2 = (box.max.z - ray.origin.z) / ray.direction.z;
    }
    
    tmin = fmax(fmin(tx1, tx2), fmax(fmin(ty1, ty2), fmin(tz1, tz2)));
    tmax = fmin(fmax(tx1, tx2), fmin(fmax(ty1, ty2), fmax(tz1, tz2)));
    
    return tmax >= tmin && tmax > EPSILON;  
}
//--------------------------------------------------------------------------------------------------

// ray_bvh_intersect() - Returns the hitrecord for the given ray
// Main function for intersection test by traversing Bounding Volume Hierarchies (BVH) using DFS

//--------------------------------------------------------------------------------------------------


HitRecord ray_bvh_intersect(Ray ray, BVHNode* node) {
    HitRecord rec = {0};
    
    if (!ray_aabb_intersect(ray, node->bounds)) {
        return rec;
    }
    
    if (node->primitive != NULL) {
        // printf("%u", node->type);
        switch (node->type)
        {
        case SPHERE:
            return ray_sphere_intersect(ray, node->primitive);
            break;
        case TRIANGLE:
            return ray_triangle_intersect(ray, node->primitive);
            break;
        default:
            break;
        }
    }
    
    HitRecord left_hit = ray_bvh_intersect(ray, node->left);
    HitRecord right_hit = ray_bvh_intersect(ray, node->right);
    
    if (!left_hit.hit_something) return right_hit;
    if (!right_hit.hit_something) return left_hit;
    
    return (left_hit.t < right_hit.t) ? left_hit : right_hit;
}
