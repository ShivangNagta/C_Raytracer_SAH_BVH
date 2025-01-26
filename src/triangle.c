#include <Custom/triangle.h>



Triangle create_triangle(Vec3 vertices[3], SDL_Color color){
    Triangle triangle;

    for (int i = 0; i < 3; i++) {
        triangle.vertices[i] = vertices[i];
    }

    triangle.color = color;

    return triangle;
}

Vec3 interpolate_normal(Triangle* triangle, float u, float v, float w) {
    Vec3 normal = {
        u * triangle->normals[0].x + v * triangle->normals[1].x + w * triangle->normals[2].x,
        u * triangle->normals[0].y + v * triangle->normals[1].y + w * triangle->normals[2].y,
        u * triangle->normals[0].z + v * triangle->normals[1].z + w * triangle->normals[2].z
    };
    return vec3_normalize(normal);
}

Triangle create_triangle_with_normals(Vec3 vertices[3], Vec3 normals[3], SDL_Color color){
    Triangle triangle;

    for (int i = 0; i < 3; i++) {
        triangle.vertices[i] = vertices[i];
    }
    for (int i = 0; i < 3; i++) {
        triangle.normals[i] = normals[i];
    }

    triangle.color = color;

    return triangle;
};