#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <GL/gl.h>

#define MAX 10000

class Obj
{
  public:
    Obj(std::string const &);
    ~Obj() {};

    void draw(float, float, GLuint);
    void storeVertices();

  private:
    std::string filename;
    float _x[MAX];
    float _y[MAX];
    float _z[MAX];
    std::vector<int> _one;
    std::vector<int> _two;
    std::vector<int> _three;
    std::vector<int> _four;
};