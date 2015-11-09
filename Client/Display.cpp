#include "Display.hpp"
#include "Exceptions.hpp"

GomokuDisplay::GomokuDisplay() {
  Window root;
  GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
  XVisualInfo *vi;
  Colormap cmap;
  XSetWindowAttributes swa;

  yaw = 90.0;
  pitch = 0.0;

  color = "white";

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

  /* Loading textures */
  _textures.push_back(loadTexture("./assets/board_edge.raw"));
  _textures.push_back(loadTexture("./assets/board_bot.raw")); // To change maybe
  _textures.push_back(loadTexture("./assets/board_bot.raw"));
  _textures.push_back(loadTexture("./assets/red.raw"));
  _textures.push_back(loadTexture("./assets/orange.raw"));
  _textures.push_back(loadTexture("./assets/yellow.raw"));
  _textures.push_back(loadTexture("./assets/green.raw"));
  _textures.push_back(loadTexture("./assets/blue.raw"));
  _textures.push_back(loadTexture("./assets/purple.raw"));
  _textures.push_back(loadTexture("./assets/pink.raw"));
  _textures.push_back(loadTexture("./assets/black.raw"));
  _textures.push_back(loadTexture("./assets/white.raw"));
}

GLuint GomokuDisplay::loadTexture(const std::string &filename) {
  GLuint texture;
  unsigned char data[64 * 64 * 3];
  FILE *fd;

  fd = fopen(filename.c_str(), "rb");
  if (fd == NULL) {
    std::cout << "Could not load texture " << filename << std::endl;
    return -1;
  }
  fread(data, 64 * 64 * 3, 1, fd);
  fclose(fd);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  return texture;
}

GomokuDisplay::~GomokuDisplay() {
  glXMakeCurrent(dpy, None, NULL);
  glXDestroyContext(dpy, glc);
  XDestroyWindow(dpy, win);
  XCloseDisplay(dpy);
}

void GomokuDisplay::drawTile(int x, int y, bool generic) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  if (generic)
    glBindTexture(GL_TEXTURE_2D, _textures[((x + 9) + (y + 9)) % 7 + 3]);
  else
    glBindTexture(GL_TEXTURE_2D, _textures[BOARDEDGE]);
  glBegin(GL_QUADS);

  glTexCoord2f(0, 0);
  glVertex3f(x +  0.5f, 0.5f,  y +-0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + -0.5f, 0.5f,  y +-0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + -0.5f, 0.5f,  y + 0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x +  0.5f, 0.5f,  y + 0.5f);

  glEnd();
  glBindTexture(GL_TEXTURE_2D, _textures[BOARDBOT]);
  glBegin(GL_QUADS);

  glTexCoord2f(0, 0);
  glVertex3f(x +  0.5f, -0.5f, y +  0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + -0.5f, -0.5f, y +  0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + -0.5f, -0.5f, y + -0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x +  0.5f, -0.5f, y + -0.5f);

  glEnd();
  glBindTexture(GL_TEXTURE_2D, _textures[BOARDSIDE]);
  glBegin(GL_QUADS);

  glTexCoord2f(0, 0);
  glVertex3f(x +  0.5f, 0.5f, y + 0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + -0.5f, 0.5f, y + 0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + -0.5f, -0.5f, y + 0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x +  0.5f, -0.5f, y + 0.5f);

  glTexCoord2f(0, 0);
  glVertex3f(x +  0.5f, -0.5f, y + -0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + -0.5f, -0.5f, y + -0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + -0.5f, 0.5f, y + -0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x +  0.5f, 0.5f, y + -0.5f);

  glTexCoord2f(0, 0);
  glVertex3f(x + -0.5f, 0.5f, y +  0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + -0.5f, 0.5f, y + -0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + -0.5f, -0.5f, y + -0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x + -0.5f, -0.5f, y +  0.5f);

  glTexCoord2f(0, 0);
  glVertex3f(x + 0.5f, 0.5f,  y + -0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + 0.5f, 0.5f,  y +  0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + 0.5f, -0.5f,  y +  0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x + 0.5f, -0.5f,  y + -0.5f);

  glEnd();
}

void GomokuDisplay::drawToken(float x, float y, bool black) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  if (!black)
    glBindTexture(GL_TEXTURE_2D, _textures[WHITE]);
  else
    glBindTexture(GL_TEXTURE_2D, _textures[BLACK]);
  glBegin(GL_QUADS);

  glTexCoord2f(0, 0);
  glVertex3f(x +  0.5f, 1.0f,  y +-0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + -0.5f, 1.0f,  y +-0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + -0.5f, 1.0f,  y + 0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x +  0.5f, 1.0f,  y + 0.5f);

  glTexCoord2f(0, 0);
  glVertex3f(x +  0.5f, 0.0f, y +  0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + -0.5f, 0.0f, y +  0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + -0.5f, 0.0f, y + -0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x +  0.5f, 0.0f, y + -0.5f);

  glTexCoord2f(0, 0);
  glVertex3f(x +  0.5f, 1.0f, y + 0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + -0.5f, 1.0f, y + 0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + -0.5f, 0.0f, y + 0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x +  0.5f, 0.0f, y + 0.5f);

  glTexCoord2f(0, 0);
  glVertex3f(x +  0.5f, 0.0f, y + -0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + -0.5f, 0.0f, y + -0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + -0.5f, 1.0f, y + -0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x +  0.5f, 1.0f, y + -0.5f);

  glTexCoord2f(0, 0);
  glVertex3f(x + -0.5f, 1.0f, y +  0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + -0.5f, 1.0f, y + -0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + -0.5f, 0.0f, y + -0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x + -0.5f, 0.0f, y +  0.5f);

  glTexCoord2f(0, 0);
  glVertex3f(x + 0.5f, 1.0f,  y + -0.5f);
  glTexCoord2f(1, 0);
  glVertex3f(x + 0.5f, 1.0f,  y +  0.5f);
  glTexCoord2f(1, 1);
  glVertex3f(x + 0.5f, 0.0f,  y +  0.5f);
  glTexCoord2f(0, 1);
  glVertex3f(x + 0.5f, 0.0f,  y + -0.5f);

  glEnd();
}

void GomokuDisplay::drawBoard(const std::map<std::pair<int, int>, char> &map) {
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 20; x++) {
      /* First we draw the board tile */
      if (x != 19 && y != 19 && x != 0 && y != 0)
        drawTile(x - 9, y - 9, true);
      else
        drawTile(x - 9, y - 9, false);
      /* Then, if there is a token on it, we draw the token */
      std::pair<int, int> p(y, x);
      if (x != 19 && y != 19 && map.at(p) == '0') {
      /* Don't forget to put the token on the intersections */
        drawToken(x - 8.5, y - 8.5, false);
      }
      else if (x != 19 && y != 19 && map.at(p) == '1') {
        drawToken(x - 8.5, y - 8.5, true);
      }
    }
  }
}

std::pair<int, int> GomokuDisplay::transformInputs(std::pair<float, float> &click) {
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  GLdouble modelview[16];
  GLdouble projection[16];
  GLfloat winX, winY, winZ;
  GLdouble posX, posY, posZ;

  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  glGetIntegerv(GL_VIEWPORT, viewport);

  winX = click.first;
  winY = (float)viewport[3] - click.second;
  glReadPixels(click.first, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

  gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

  std::pair<int, int> ret((int)(posX + 9), (int)(posZ + 9));
  if (ret.first < 0 || ret.first > 18 || ret.second < 0 || ret.second > 18) {
    ret.first = -1;
    ret.second = -1;
  }
  return ret;
}

std::pair<int, int> GomokuDisplay::drawGame(const std::map<std::pair<int, int>, char> &map) {
  XWindowAttributes gwa;

  XGetWindowAttributes(dpy, win, &gwa);
  glViewport(0, 0, gwa.width, gwa.height);

  /* Getting ready to draw */
  if (color.compare("black") == 0)
    glClearColor(0.0, 0.0, 0.0, 0.0);
  else
    glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Camera positionning */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60, 1, 0.01, 1000);
  glTranslatef(-0.5, 0.5, -20);
  glRotatef(yaw, 1., 0., 0.);
  glRotatef(pitch, 0., 1., 0.);

  drawBoard(map);
  drawUI();

  glXSwapBuffers(dpy, win);
  std::pair<float, float> p = handleInputs();
  if (p.first >= 0)
    return transformInputs(p);
  return p;
}

void GomokuDisplay::setColor(const std::string &c) {
  color = c;
}

void GomokuDisplay::drawUI() {
  XGCValues val;
  XFontStruct *font_info = XLoadQueryFont(dpy, "fixed");
  val.font = font_info->fid;
  GC gc = XCreateGC(dpy, win, 0, &val);

  char str[256];
  sprintf(str, "%s", message.c_str());
  int width = XTextWidth(font_info, str, message.length());
  int font_height = font_info->ascent + font_info->descent;
  XDrawString(dpy, win, gc, (1000 - width) / 2, font_height, str, message.length());
}

std::pair<float, float> GomokuDisplay::handleInputs() {
  XEvent xev;
  std::pair<float, float> p(-1.0, -1.0);

  XSelectInput(dpy, win, ButtonPressMask|ButtonReleaseMask|KeyPressMask);
  if (XPending(dpy) > 0)
  {
    XNextEvent(dpy, &xev);
    if (xev.type == KeyPress) {
      switch (xev.xkey.keycode) {
        case KEY_ESCAPE:
          p.first = -2;
          p.second = -2;
          break;
        case KEY_SPACE:
          p.first = -3;
          p.second = -3;
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