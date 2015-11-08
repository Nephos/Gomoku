#pragma once

#include <utility>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <map>

#define CAMERA_SPEED 10

enum keyCodes {
  KEY_ESCAPE = 9,
  KEY_SPACE = 65, // Not used (for now)
  KEY_LEFT = 113,
  KEY_RIGHT = 114,
  KEY_DOWN = 116,
  KEY_UP = 111
};

class GomokuDisplay {
  public:
    GomokuDisplay();
    ~GomokuDisplay();

    std::pair<int, int> drawGame(const std::map<std::pair<int, int>, char> &); // Displays the game board, iterates on it & calls drawToken() then drawUI()
    void drawBoard(const std::map<std::pair<int, int>, char> &);
    void drawToken(); // Draws a token
    void drawUI(); // Draws the UI : scores + message
    void newToken(); // Displays a token spawn
    std::pair<int, int> handleInputs(); // Handles user inputs
    void setMessage(std::string const &);

  private:
    Display *dpy;
    Window win;
    GLXContext glc;
    float yaw, pitch;
    std::string message;
};