#pragma once

#include <utility>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <Imlib2.h>
#include <map>
#include <vector>
#include "Obj.hpp"

#define CAMERA_SPEED 5

enum keyCodes {
  KEY_ESCAPE = 9,
  KEY_SPACE = 65,
  KEY_LEFT = 113,
  KEY_RIGHT = 114,
  KEY_DOWN = 116,
  KEY_UP = 111
};

enum textures {
  BOARDEDGE,
  BOARDWHITE,
  BOARDBLACK,
  BOARDSIDE,
  BOARDBOT,
  RED,
  ORANGE,
  YELLOW,
  GREEN,
  BLUE,
  PURPLE,
  PINK,
  BLACK,
  WHITE
};

class GomokuDisplay {
  public:
    GomokuDisplay(bool rainbows = true, bool rotate = false);
    ~GomokuDisplay();

    std::pair<int, int> drawGame(char[19][19]); // Displays the game board, iterates on it & calls drawToken() then drawUI()
    std::pair<int, int> transformInputs(std::pair<float, float> &);
    void drawBoard(char[19][19]);
    void drawTile(int, int, bool, int);
    void drawToken(float, float, bool); // Draws a token
    void drawPony(float, float, bool);
    void drawUI(); // Draws the UI : scores + message
    void newToken(); // Displays a token spawn
    std::pair<float, float> handleInputs(); // Handles user inputs
    GLuint loadTexture(std::string const &);
    void loadOBJ(std::string const &);

    void setColor(const std::string &);
    void setMessage(std::string const &);
    void setBlackScore(int);
    void setWhiteScore(int);

    bool getRulesChanged() { bool tmp = _rulesChanged; _rulesChanged = false; return tmp; };
    bool getFree() const { return _free; };
    bool getBreak() const { return _break; };

  private:
    Display *dpy;
    Window win;
    GLXContext glc;
    float yaw, pitch;
    std::string message;
    std::vector<GLuint> _textures;
    std::string color;
    int loop;
    bool rainbow;
    bool rotate;
    int whiteScore;
    int blackScore;
    std::vector<GLuint> _texturesP1;
    std::vector<GLuint> _texturesP2;
    std::vector<Obj> _modelsP1;
    std::vector<Obj> _modelsP2;

    bool _rulesChanged;
    bool _free;
    bool _break;
};
