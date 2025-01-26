#include <Custom/triangle.h>



Triangle create_triangle(Vec3 vertices[3], SDL_Color color){
    Triangle triangle;

    for (int i = 0; i < 3; i++) {
        triangle.vertices[i] = vertices[i];
    }

    triangle.color = color;

    return triangle;
}