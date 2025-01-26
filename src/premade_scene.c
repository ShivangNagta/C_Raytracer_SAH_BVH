#include "Custom/scene.h"
#include <SDL2/SDL.h>
#include <stdlib.h>

Scene* scene_1(){

    Scene *scene = (Scene *) malloc(sizeof(Scene));
    scene->count = 0;
    scene->maxCount = MAX_OBJECTS;
    Sphere sphere1 = create_sphere(((Vec3){0.0f, -100.0f, 30.0f}), 100.0f);
    WorldObject sphereObject1 = {.type = SPHERE, .data.sphere = sphere1};
    WorldObject sphereObject2 = { .type = SPHERE, .data.sphere = create_sphere(((Vec3){0.0f, 10.0f, -20.0f}), 20.0f)};
    add_object_to_scene( scene , &sphereObject1);
    add_object_to_scene ( scene, &sphereObject2);

    return scene;
}

Scene* scene_2(){

    Scene *scene = (Scene *) malloc(sizeof(Scene));
    scene->count = 0;
    scene->maxCount = MAX_OBJECTS;
    Vec3 vertices[3] = {
        {0.0f, 0.0f, -10.0f},
        {20.0f, 0.0f, 10.0f},
        {0.0f, 20.0f, 0.0f}
    };
    SDL_Color color = {0, 0, 0, 255};
    Triangle triangle = create_triangle(vertices, color);
    WorldObject triangleObj = {.type = TRIANGLE, .data.triangle = triangle};
    add_object_to_scene(scene, &triangleObj);

    return scene;
}