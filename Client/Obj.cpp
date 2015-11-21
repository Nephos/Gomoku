#include "Obj.hpp"

Obj::Obj(const std::string &f) {
  std::ifstream file;
  file.open(f);
  filename = f;

  std::string str;

  while(!file.eof()) {
    std::getline(file, str);
    if(str[0] == 'v')
      break;
  }

  int v = 0;

  while(str[0] == 'v') {
    int i = 0;

    while(true) {
      while(str[i] == ' ')
        i++;
      i += 2;
      int j = i, k = i;
      while(str[i] != ' ') {  
          i++;
          k = i;
      }
      _x[v] = atof(str.substr(j, k - j).c_str());

      while(str[i] == ' ' ) {
        i++;
      }

      int q = i, w = i;
      while(str[i] != ' ' ) {
        i++;
        w = i;
      }
      _y[v] = atof(str.substr(q, w - q).c_str());

      while(str[i] == ' ' ) {
        i++;
      }

      int a = i, s = i;
      while(str[i] != ' ' ) {
        i++;
        s = i;
      }
      _z[v] = atof(str.substr(a, s - a).c_str());
      break;
    }
    v++; 
    getline(file, str);
  }
  file.close();
  storeVertices();
  storeMap();
}

void Obj::storeMap() {
  std::ifstream file;
  file.open(filename);

  std::string str;

  while(!file.eof()) {
    std::getline(file, str);
    if(str[0] == 'v' && str[1] && str[1] == 't')
      break;
  }

  int v = 0;

  while(str[0] == 'v' && str[1] && str[1] == 't') {
    int i = 0;

    while(true) {
      while(str[i] == ' ')
        i++;
      i += 2;
      int j = i, k = i;
      while(str[i] != ' ') {  
          i++;
          k = i;
      }
      _vtx[v] = atof(str.substr(j, k - j).c_str());

      while(str[i] == ' ' ) {
        i++;
      }

      int q = i, w = i;
      while(str[i] != ' ' ) {
        i++;
        w = i;
      }
      _vty[v] = atof(str.substr(q, w - q).c_str());

      break;
    }
    v++; 
    getline(file, str);
  }
  file.close();
}

void Obj::storeVertices() {
  std::ifstream file;
  file.open(filename);

  std::string str;

  while(true) {
    std::getline(file, str);
    if(str[0] == 'f') {
      break;
    }
  }

  int i = 0;

  while(str[0] == 'f') {
    while(str[i] == 'f')
      i++;
    while(str[i] == ' ')
      i++;
    int j = i, k = i;

    while(str[i] != ' ') {  
      i++;
      k = i;
    }

    int one = atof(str.substr(j, k - j).c_str());

    i++;
    j = i;
    k = i;
    while(str[i] != ' ') { 
      i++;
      k = i;
    }
    int two = atof(str.substr(j, k - j).c_str());

    i++;
    j = i;
    k = i;
    while(str[i] != ' ') {  
      i++;
      k = i;
    }
    int three = atof(str.substr(j, k - j).c_str());

    i++;
    j = i;
    k = i;
    while(str[i] != ' ') {
      i++;
      k = i;
    }
    int four = atof(str.substr(j, k - j).c_str());

    _one.push_back(one - 1);
    _two.push_back(two - 1);
    _three.push_back(three - 1);
    _four.push_back(four - 1);

    std::getline(file, str);
    i = 0;
  }
  file.close();
}

void Obj::draw(float x, float y, GLuint texture) {
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glBindTexture(GL_TEXTURE_2D, texture);
  glBegin(GL_POLYGON);

  std::vector<int>::iterator it1, it2, it3, it4;
  it2 = _two.begin();
  it3 = _three.begin();
  it4 = _four.begin();
  for (it1 = _one.begin(); it1 != _one.end(); it1++) {
    glTexCoord2f(0, 0);
    glVertex3d(_x[*it1] / OBJ_SIZE + x, _y[*it1] / OBJ_SIZE + 1, _z[*it1] / OBJ_SIZE + y);
    glTexCoord2f(0, 1);
    glVertex3d(_x[*it2] / OBJ_SIZE + x, _y[*it2] / OBJ_SIZE + 1, _z[*it2] / OBJ_SIZE + y);
    glTexCoord2f(1, 0);
    glVertex3d(_x[*it3] / OBJ_SIZE + x, _y[*it3] / OBJ_SIZE + 1, _z[*it3] / OBJ_SIZE + y);
    glTexCoord2f(1, 1);
    glVertex3d(_x[*it4] / OBJ_SIZE + x, _y[*it4] / OBJ_SIZE + 1, _z[*it4] / OBJ_SIZE + y);
    it2++;
    it3++;
    it4++;
  }
  glEnd();
}