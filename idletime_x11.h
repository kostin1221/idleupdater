#ifndef IDLETIME_X11_H
#define IDLETIME_X11_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>

int idletime_init();
unsigned int idletime_get();

#endif // IDLETIME_X11_H
