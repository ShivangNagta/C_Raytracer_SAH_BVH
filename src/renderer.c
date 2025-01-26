#include "Custom/renderer.h"
#include "Custom/hit.h"
#include <math.h>
#include <stdio.h>

//--------------------------------------------------------------------------------------------------

// Main function for recursive ray tracing (up to a specified depth).
// The closest intersection of the ray can be calculated using two methods:
// Method 1 - Bounding Volume Hierarchies (BVH) with Surface Area Heuristics (SAH) - O(log n), but building the BVH is O(n log n).
// Method 2 - Brute force, traversing all objects (spheres) - O(n) for intersection tests.

// Recursion ends earlier if the ray directly misses all objects or ends up going towards
// the sky after reflection.
// This is a naive implementation of the microfacet model, considering only material roughness.
// Many other material properties are not included for simplicity.
// A given ray takes the base color of the material it hits and blends it with the color
// of the reflected ray recursively.

//--------------------------------------------------------------------------------------------------

SDL_Color trace_ray(Ray ray, Scene *scene, int num_objects, int depth, BVHNode *bvh)
{
    if (depth <= 0)
        return (SDL_Color){0, 0, 0, 255};

    HitRecord closest_hit = {0};
    closest_hit.t = INFINITY;

    if (bvh)
    {
        closest_hit = ray_bvh_intersect(ray, bvh);
        
    }
    else
    {

        for (int i = 0; i < num_objects; i++)
        {
            HitRecord hit;
            if (scene->objects[i].type == SPHERE){
                hit = ray_sphere_intersect(ray, &scene->objects[i].data.sphere);
            }
            else{
                hit = ray_triangle_intersect(ray, &scene->objects[i].data.triangle);
            }

            if (hit.hit_something && hit.t < closest_hit.t)
            {
                closest_hit = hit;
            }
        }
    }

    if (closest_hit.hit_something)
    {
    
        SDL_Color final_color = {0, 0, 0, 255};
        SDL_Color base_color;
        // printf("%u", closest_hit.type);
        if(closest_hit.type == SPHERE){
            
            base_color = closest_hit.object.sphere.color;
        }
        else if (closest_hit.type == TRIANGLE){
            base_color = closest_hit.object.triangle.color;
        }
        else{
            base_color = (SDL_Color){0.0f, 0.0f, 0.0f};
        }
        
        

        Vec3 reflected_dir = random_on_hemisphere(closest_hit.normal);
        //Vec3 reflected_dir = vec3_reflect(closest_hit.point, closest_hit.normal);

        Ray reflected_ray = {closest_hit.point, reflected_dir};
        SDL_Color reflected_color = trace_ray(reflected_ray, scene, num_objects, depth - 1, bvh);
        final_color.r = (Uint8)(base_color.r * 0.5 + 0.8 * reflected_color.r);
        final_color.g = (Uint8)(base_color.g * 0.5 + 0.8 * reflected_color.g);
        final_color.b = (Uint8)(base_color.b * 0.5 + 0.8 * reflected_color.b);

        final_color.a = 255;

        return final_color;
    }

    float t = 0.5f * (ray.direction.y + 1.0f);
    SDL_Color sky_color = {
        (1.0f - t) * 255 + t * 128,
        (1.0f - t) * 255 + t * 178,
        255,
        255};
    // SDL_Color sky_color = {
    //     0,
    //     0,
    //     0,
    //     255};
    return sky_color;
}
