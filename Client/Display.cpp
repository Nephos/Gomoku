#include "Display.hpp"
#include "Exceptions.hpp"

GomokuDisplay::GomokuDisplay() {
  Window root;
  GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
  XVisualInfo *vi;
  Colormap cmap;
  XSetWindowAttributes swa;

  yaw = 100.0;
  pitch = 0.0;

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

void GomokuDisplay::drawBoard(const std::map<std::pair<int, int>, char> &) {

}

std::pair<int, int> GomokuDisplay::drawGame(const std::map<std::pair<int, int>, char> &map) {
  XWindowAttributes gwa;

  XGetWindowAttributes(dpy, win, &gwa);
  glViewport(0, 0, gwa.width, gwa.height);

  /* Getting ready to draw */
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Camera positionning */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1, 0.01, 1000);
  glTranslatef(0, 0, -4);
  glRotatef(yaw, 1., 0., 0.);
  glRotatef(pitch, 0., 1., 0.);

  drawBoard(map);
  // Iterate & call drawToken() function
  glXSwapBuffers(dpy, win);
  return handleInputs();
}

std::pair<int, int> GomokuDisplay::handleInputs() {
  XEvent xev;
  std::pair<int, int> p(-1, -1);

  XSelectInput(dpy, win, ButtonPressMask|ButtonReleaseMask|KeyPressMask);
  if (XPending(dpy) > 0)
  {
    XNextEvent(dpy, &xev);
    if (xev.type == KeyPress) {
      std::cout << xev.xkey.keycode << std::endl;
      switch (xev.xkey.keycode) {
        case KEY_ESCAPE:
          p.first = -2;
          p.second = -2;
          break;
        case KEY_LEFT:
          pitch -= CAMERA_SPEED;
          break;
        case KEY_RIGHT:
          pitch += CAMERA_SPEED;
          break;
        case KEY_UP:
          yaw += CAMERA_SPEED;
          break;
        case KEY_DOWN:
          yaw -= CAMERA_SPEED;
          break;
      }
    }
    else if (xev.type == ButtonRelease) {
      p.first = xev.xbutton.x;
      p.second = xev.xbutton.y;
    }
  }
  return p;
}

void GomokuDisplay::setMessage(const std::string &msg) {
  message = msg;
}