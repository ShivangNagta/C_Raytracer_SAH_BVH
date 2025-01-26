#pragma once

#include <SDL2/SDL.h>
#include "Custom/vec3.h"
#include "Custom/triangle.h"
#include "Custom/scene.h"

#define MAX_VERTICES 50000
#define MAX_TRIANGLES 50000
#define MAX_LINE_LENGTH 256


typedef struct {
    int v1, v2, v3;   
    int n1, n2, n3;  
    SDL_Color color;
} ObjTriangle;


typedef struct {
    Vec3 vertices[MAX_VERTICES];
    Vec3 normals[MAX_VERTICES]; 
    ObjTriangle triangles[MAX_TRIANGLES];
    int normalCount;
    int vertexCount;
    int triangleCount;
} Model;





Model load_model_from_obj(const char* filename);
void convert_model_to_scene_triangles(Model* model, Scene* scene);

