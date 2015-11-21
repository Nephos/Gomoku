#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <GL/gl.h>

#define MAX 10000
#define OBJ_SIZE 7

class Obj
{
  public:
    Obj(std::string const &);
    ~Obj() {};

    void draw(float, float, GLuint);
    void storeVertices();
    void storeMap();

  private:
    std::string filename;
    float _x[MAX];
    float _y[MAX];
    float _z[MAX];
    float _vtx[MAX];
    float _vty[MAX];
    std::vector<int> _one;
    std::vector<int> _two;
    std::vector<int> _three;
    std::vector<int> _four;
};