#pragma once

#include <utility>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

class GomokuDisplay {
  public:
    GomokuDisplay();
    ~GomokuDisplay();

    std::pair<int, int> drawMap(/* int array or something */); // Displays the game board, iterates on it & calls drawToken() then drawUI()
    void drawToken(); // Draws a token
    void drawUI(); // Draws the UI
    void newToken(); // Displays a token spawn
    std::pair<int, int> handleInputs(); // Handles user inputs

  private:
    Display *dpy;
    Window win;
    GLXContext glc;
};