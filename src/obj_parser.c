#include "Custom/obj_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


Model load_model_from_obj(const char* filename) {
    Model model = {0};
    FILE* file = fopen(filename, "r");
    
    if (!file) {
        fprintf(stderr, "Error opening OBJ file: %s\n", filename);
        return model;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Vertex parsing
        if (strncmp(line, "v ", 2) == 0) {
            if (model.vertexCount < MAX_VERTICES) {
                Vec3* v = &model.vertices[model.vertexCount++];
                sscanf(line, "v %f %f %f", &v->x, &v->y, &v->z);
            }
        }
        
        // Face parsing (triangles only)
        if (strncmp(line, "f ", 2) == 0) {
            if (model.triangleCount < MAX_TRIANGLES) {
                ObjTriangle* t = &model.triangles[model.triangleCount++];
                // Assumes vertex indices start at 1
                sscanf(line, "f %d %d %d", 
                    &t->v1, &t->v2, &t->v3);
                
                // Default color (black)
                t->color = (SDL_Color){255, 0, 0, 255};
            }
        }
    }

    fclose(file);
    return model;
}

// Convert loaded Model to scene triangles
void convert_model_to_scene_triangles(Model* model, Scene* scene) {
    for (int i = 0; i < model->triangleCount; i++) {
        ObjTriangle* objTri = &model->triangles[i];
        
        // Create triangle vertices from model's vertex data
        Vec3 vertices[3] = {
            {model->vertices[objTri->v1 - 1].x*100, 
             model->vertices[objTri->v1 - 1].y*100, 
             model->vertices[objTri->v1 - 1].z*100},
            {model->vertices[objTri->v2 - 1].x*100, 
             model->vertices[objTri->v2 - 1].y*100, 
             model->vertices[objTri->v2 - 1].z*100},
            {model->vertices[objTri->v3 - 1].x*100, 
             model->vertices[objTri->v3 - 1].y*100, 
             model->vertices[objTri->v3 - 1].z*100}
        };

        Triangle triangle = create_triangle(vertices, objTri->color);
        WorldObject triangleObj = {.type = TRIANGLE, .data.triangle = triangle};
        add_object_to_scene(scene, &triangleObj);
    }
}



// #include "Custom/obj_parser.h"
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>

// Model load_model_from_obj(const char *filename)
// {
//     Model model = {0};
//     FILE *file = fopen(filename, "r");

//     if (!file)
//     {
//         fprintf(stderr, "Error opening OBJ file: %s\n", filename);
//         return model;
//     }

//     char line[MAX_LINE_LENGTH];
//     while (fgets(line, sizeof(line), file))
//     {
//         // Vertex parsing
//         if (strncmp(line, "v ", 2) == 0)
//         {
//             if (model.vertexCount < MAX_VERTICES)
//             {
//                 Vec3 *v = &model.vertices[model.vertexCount++];
//                 sscanf(line, "v %f %f %f", &v->x, &v->y, &v->z);
//             }
//         }

//         if (strncmp(line, "vn ", 3) == 0)
//         {
//             if (model.normalCount < MAX_VERTICES)
//             {
//                 Vec3 *n = &model.normals[model.normalCount++];
//                 sscanf(line, "vn %f %f %f", &n->x, &n->y, &n->z);
//             }
//         }

//         if (strncmp(line, "f ", 2) == 0)
//         {
//             if (model.triangleCount < MAX_TRIANGLES)
//             {
//                 ObjTriangle *t = &model.triangles[model.triangleCount++];
//                 // Face format: f v1//n1 v2//n2 v3//n3 (handles both vertex and normal indices)
//                 sscanf(line, "f %d//%d %d//%d %d//%d",
//                        &t->v1, &t->n1, &t->v2, &t->n2, &t->v3, &t->n3);
//                 t->color = (SDL_Color){255, 0, 0, 255};
//             }
//         }
//     }

//     fclose(file);
//     return model;
// }



// void convert_model_to_scene_triangles(Model* model, Scene* scene) {
//     for (int i = 0; i < model->triangleCount; i++) {
//         ObjTriangle* objTri = &model->triangles[i];

//         Vec3 vertices[3] = {
//             model->vertices[objTri->v1 - 1],
//             model->vertices[objTri->v2 - 1],
//             model->vertices[objTri->v3 - 1]
//         };

//         Vec3 normals[3] = {
//             model->normals[objTri->n1 - 1],
//             model->normals[objTri->n2 - 1],
//             model->normals[objTri->n3 - 1]
//         };

//         Triangle triangle = create_triangle_with_normals(vertices, normals, objTri->color);
//         WorldObject triangleObj = {.type = TRIANGLE, .data.triangle = triangle};
//         add_object_to_scene(scene, &triangleObj);
//     }
// }