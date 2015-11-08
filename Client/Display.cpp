#include "Display.hpp"
#include "Exceptions.hpp"

GomokuDisplay::GomokuDisplay() {
  Window root;
  GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
  XVisualInfo *vi;
  Colormap cmap;
  XSetWindowAttributes swa;

  dpy = XOpenDisplay(NULL);
  if (dpy == NULL)
    throw Gomoku::DisplayException("Cannot connect to X server");
  root = DefaultRootWindow(dpy);
  vi = glXChooseVisual(dpy, 0, att);
  if (vi == NULL)
    throw Gomoku::DisplayException("No visual found");
  cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
  swa.colormap = cmap;
  swa.event_mask = ExposureMask | KeyPressMask;
  win = XCreateWindow(dpy, root, 0, 0, 1000, 1000, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
  XMapWindow(dpy, win);
  XStoreName(dpy, win, "Gomoku");
  glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
  glXMakeCurrent(dpy, win, glc);
  glEnable(GL_DEPTH_TEST);
}

GomokuDisplay::~GomokuDisplay() {
  glXMakeCurrent(dpy, None, NULL);
  glXDestroyContext(dpy, glc);
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
}

std::pair<int, int> GomokuDisplay::drawMap() {
  XWindowAttributes gwa;

  XGetWindowAttributes(dpy, win, &gwa);
  glViewport(0, 0, gwa.width, gwa.height);
  // Draw the board
  // Iterate & call drawToken() function
  glXSwapBuffers(dpy, win);
  return handleInputs();
}

std::pair<int, int> GomokuDisplay::handleInputs() {
  XEvent xev;
  std::pair<int, int> p(-1, -1);

  XSelectInput(dpy, win, ButtonPressMask|ButtonReleaseMask);
  if (XPending(dpy) > 0)
  {
    XNextEvent(dpy, &xev);
    if (xev.type == Expose)
      drawMap();
    else if (xev.type == KeyPress)
      std::cout << "yay, keyboard input !" << std::endl;
    else if (xev.type == ButtonRelease) {
      p.first = xev.xbutton.x;
      p.second = xev.xbutton.y;
    }
  }
  return p;
}