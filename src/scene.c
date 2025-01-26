

#include "Custom/scene.h"


bool add_object_to_scene(Scene* scene, const WorldObject* object){
    if (scene->count >= scene->maxCount) return false;
    scene->objects[scene->count++] = *object;
    return true;
};


