#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include "GL/freeglut.h"

using namespace std;

class Material {
    public:
        string Name;
        float *ambient;
        float *diffuse;
        float *specular;
        GLuint texture;

        Material(string name, float *ambient, float *diffuse, float *specular) {
            Name = name;
            this->ambient = ambient;
            this->diffuse = diffuse;
            this->specular = specular;
            this->texture = 0;
        }
  };

#endif