#include "idletime_x11.h"

static XScreenSaverInfo *ss_info;
static Display *display;

static XErrorHandler old_handler = 0;
int xerrhandler(Display* dpy, XErrorEvent* err)
{
    if(err->error_code == BadDrawable)
        return 0;

    return (*old_handler)(dpy, err);
}


int idletime_init(){
    if(ss_info)
            return 1;

    old_handler = XSetErrorHandler(xerrhandler);
    display = XOpenDisplay(0);

    int event_base, error_base;
    if(XScreenSaverQueryExtension(display, &event_base, &error_base)) {
        ss_info = XScreenSaverAllocInfo();
        return 1;
    }

    return 0;
}

void idletime_free(){
    if(ss_info)
        XFree(ss_info);
    if(old_handler) {
        XSetErrorHandler(old_handler);
        old_handler = 0;
    }
}

unsigned int idletime_get(){
    if (!ss_info)
        return 0;
    if (!XScreenSaverQueryInfo(display, DefaultRootWindow(display), ss_info))
        return 0;
    return ss_info->idle;
  //  printf("%u ms\n", info->idle);

}

