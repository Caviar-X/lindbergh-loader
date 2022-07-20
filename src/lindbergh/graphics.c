#include <GL/freeglut.h>
#include <GL/glx.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "jvs.h"
#include "securityboard.h"

int gameModeWidth = -1;
int gameModeHeight = -1;

FGAPI int FGAPIENTRY glutEnterGameMode()
{
  char gameTitle[256] = {0};
  strcat(gameTitle, getGameName());
  strcat(gameTitle, " (GLUT)");
  glutCreateWindow(gameTitle);
  return 1;
}

FGAPI void FGAPIENTRY glutLeaveGameMode()
{
  glutDestroyWindow(glutGetWindow());
  return;
}

FGAPI void FGAPIENTRY glutSetCursor(int cursor)
{
  return;
}

FGAPI void FGAPIENTRY glutGameModeString(const char *string)
{
  printf("glutGameModeString: %s\n", string);

  char gameModeString[1024];
  strcpy(gameModeString, string);

  char *widthString = gameModeString;
  char *heightString = NULL;

  for (int i = 0; i < 1024; i++)
  {
    if (gameModeString[i] == 'x')
    {
      gameModeString[i] = 0;
      heightString = &gameModeString[i + 1];
    }

    if (gameModeString[i] == ':')
    {
      gameModeString[i] = 0;
      break;
    }
  }

  int width = atoi(widthString);
  int height = atoi(heightString);

  if (getConfig()->width != width || getConfig()->height != height)
  {
    printf("Warning: Game is overriding resolution settings to %dX%d\n", width, height);
    getConfig()->width = width;
    getConfig()->height = height;
  }
}

Window XCreateWindow(Display *display, Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int border_width, int depth, unsigned int class, Visual *visual, unsigned long valueMask, XSetWindowAttributes *attributes)
{

  Window (*_XCreateWindow)(Display * display, Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int border_width, int depth, unsigned int class, Visual *visual, unsigned long valueMask, XSetWindowAttributes *attributes) = dlsym(RTLD_NEXT, "XCreateWindow");

  width = getConfig()->width;
  height = getConfig()->height;

  // Ensure that the windows will respond with keyboard and mouse events
  attributes->event_mask = attributes->event_mask | KeyPressMask | KeyReleaseMask | PointerMotionMask;
  attributes->override_redirect = 0;

  Window window = _XCreateWindow(display, parent, x, y, width, height, border_width, depth, class, visual, valueMask, attributes);
  printf("%d %d %d %d\n", x, y, width, height);
  return window;
}

int XGrabPointer(Display *display, Window grab_window, Bool owner_events, unsigned int event_mask, int pointer_mode, int keyboard_mode, Window confine_to, Cursor cursor, Time time)
{
  int (*_XGrabPointer)(Display * display, Window grab_window, Bool owner_events, unsigned int event_mask, int pointer_mode, int keyboard_mode, Window confine_to, Cursor cursor, Time time) = dlsym(RTLD_NEXT, "XGrabPointer");
  int returnValue = _XGrabPointer(display, grab_window, owner_events, event_mask, pointer_mode, keyboard_mode, confine_to, cursor, time);
  XUngrabPointer(display, time);
  return returnValue;
}

int XGrabKeyboard(Display *display, Window grab_window, Bool owner_events, int pointer_mode, int keyboard_mode, Time time)
{
  int (*_XGrabKeyboard)(Display * display, Window grab_window, Bool owner_events, int pointer_mode, int keyboard_mode, Time time) = dlsym(RTLD_NEXT, "XGrabKeyboard");
  int returnValue = _XGrabKeyboard(display, grab_window, owner_events, pointer_mode, keyboard_mode, time);
  XUngrabKeyboard(display, time);
  return returnValue;
}

int XDefineCursor(Display *display, Window w, Cursor cursor)
{
  return 0;
}

int XStoreName(Display *display, Window w, const char *window_name)
{
  int (*_XStoreName)(Display * display, Window w, const char *window_name) = dlsym(RTLD_NEXT, "XStoreName");
  return _XStoreName(display, w, getGameName());
}

int XNextEvent(Display *display, XEvent *event_return)
{

  int (*_XNextEvent)(Display * display, XEvent * event_return) = dlsym(RTLD_NEXT, "XNextEvent");
  int returnValue = _XNextEvent(display, event_return);
  switch (event_return->type)
  {
  case KeyPress:
    switch (event_return->xkey.keycode)
    {
    case 28:
      securityBoardSetSwitch(BUTTON_TEST, 1);
      break;
    case 39:
      securityBoardSetSwitch(BUTTON_SERVICE, 1);
      break;
    default:
      break;
    }
    break;
  case KeyRelease:
    switch (event_return->xkey.keycode)
    {
    case 28:
      securityBoardSetSwitch(BUTTON_TEST, 0);
      break;
    case 39:
      securityBoardSetSwitch(BUTTON_SERVICE, 0);
      break;
    default:
      break;
    }
    break;
  }

  return returnValue;
}

int XSetStandardProperties(Display *display, Window window, const char *window_name, const char *icon_name, Pixmap icon_pixmap, char **argv, int argc, XSizeHints *hints)
{
  int (*_XSetStandardProperties)(Display * display, Window window, const char *window_name, const char *icon_name, Pixmap icon_pixmap, char **argv, int argc, XSizeHints *hints) = dlsym(RTLD_NEXT, "XSetStandardProperties");
  char gameTitle[256] = {0};
  strcat(gameTitle, getGameName());
  strcat(gameTitle, " (X11)");
  return _XSetStandardProperties(display, window, gameTitle, icon_name, icon_pixmap, argv, argc, hints);
}