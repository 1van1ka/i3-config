#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Window *WList(Display *display, u_long *nitems_return) {
    Atom actual_type_return;
    int actual_format_return;
    u_long bytes_after_return = 0;
    u_char *list = NULL;
    Status status = XGetWindowProperty(
        display,
        DefaultRootWindow(display),
        XInternAtom(display, "_NET_CLIENT_LIST", False),
        0L,
        ~0L,
        False,
        XA_WINDOW,
        &actual_type_return,
        &actual_format_return,
        nitems_return,
        &bytes_after_return,
        &list
    );

    if (status != Success) {
        *nitems_return = 0;
        return NULL;
    }

    return (Window *)list;
}

int CDesktop(Display *display) {
    Atom actual_type_return;
    int actual_format_return = 0;
    u_long nitems_return = 0;
    u_long bytes_after_return = 0;
    int *desktop = NULL;
    int ret;
    Status status = XGetWindowProperty(
        display,
        DefaultRootWindow(display),
        XInternAtom(display, "_NET_CURRENT_DESKTOP", False),
        0L,
        ~0L,
        False,
        XA_CARDINAL,
        &actual_type_return,
        &actual_format_return,
        &nitems_return,
        &bytes_after_return,
        (unsigned char **)&desktop
    );

    if (status != Success) {
        return 0;
    }

    ret = desktop[0];
    XFree(desktop);

    return ret;
}

long WDesktop(Display *display, Window window) {
    Atom actual_type_return;
    int actual_format_return;
    u_long nitems_return = 0;
    u_long bytes_after_return = 0;
    long *desktop = 0;
    long ret = 0;
    Status status = XGetWindowProperty(
        display,
        window,
        XInternAtom(display, "_NET_WM_DESKTOP", False),
        0L,
        ~0L,
        False,
        XA_CARDINAL,
        &actual_type_return,
        &actual_format_return,
        &nitems_return,
        &bytes_after_return,
        (unsigned char **)(void *)&desktop
    );

    if (status != Success) {
        return 0;
    }

    ret = desktop[0];
    XFree(desktop);

    return ret;
}

Window WActive(Display *display) {
    Atom actual_type_return;
    int actual_format_return;
    u_long nitems_return = 0;
    u_long bytes_after_return = 0;
    u_char *active = NULL;
    Window ret;
    Status status = XGetWindowProperty(
        display,
        DefaultRootWindow(display),
        XInternAtom(display, "_NET_ACTIVE_WINDOW", False),
        0L,
        ~0L,
        False,
        AnyPropertyType,
        &actual_type_return,
        &actual_format_return,
        &nitems_return,
        &bytes_after_return,
        &active
    );

    if (status != Success) {
        return 0;
    }

    ret = *(Window *)active;
    XFree(active);

    return ret;
}

char *WName(Display *display, Window window) {
    Atom actual_type_return;
    int actual_format_return;
    u_long nitems_return = 0;
    u_long bytes_after_return = 0;
    u_char *name = NULL;
    Status status = XGetWindowProperty(
        display,
        window,
        XInternAtom(display, "WM_CLASS", False),
        0L,
        ~0L,
        False,
        AnyPropertyType,
        &actual_type_return,
        &actual_format_return,
        &nitems_return,
        &bytes_after_return,
        &name
    );

    if (status != Success) {
        return NULL;
    }

    return (char *)name;
}

int main(void) {
    Display *display;

    if ((display = XOpenDisplay(NULL)) == NULL) {
        puts("Can not connect to the X server!\n");
    }

    XEvent event;
    Window rootwindow;
    u_long count;
    unsigned short int maxLength = 67;

    char *out = NULL;
    char *lastOut = NULL;

    while (1) {
        rootwindow = DefaultRootWindow(display);
        XSelectInput(display, rootwindow, SubstructureNotifyMask);
        int status;
        count = 0;
        Window *wlist = WList(display, &count);
        Window wactive = WActive(display);

        out = (char *)malloc(1);
        strcpy(out, "\0");

        typedef struct {
            char *className;
            int count;
        } ClassCount;

        ClassCount *classCounts = NULL;
        int classCountSize = 0;

        for (int i = 0; i < count; ++i) {
            Window w = wlist[i];

            char *wname;
            int cdesktop = CDesktop(display);
            int wdesktop = WDesktop(display, w);

            if ((wname = WName(display, w)) && wdesktop == cdesktop) {
                int found = 0;
                for (int j = 0; j < classCountSize; ++j) {
                    if (strcmp(classCounts[j].className, wname) == 0) {
                        classCounts[j].count++;
                        found = 1;
                        break;
                    }
                }

                if (!found) {
                    classCounts = (ClassCount *)realloc(classCounts, (classCountSize + 1) * sizeof(ClassCount));
                    classCounts[classCountSize].className = strdup(wname);
                    classCounts[classCountSize].count = 1;
                    classCountSize++;
                }

                XFree(wname);
            }
        }

        for (int i = 0; i < classCountSize; ++i) {
            out = (char *)realloc(out, strlen(out) +
                                  strlen(classCounts[i].className) +
                                  strlen(":") + 20
                                  );
            strcat(out, classCounts[i].className);
            char windowCountStr[10];
            sprintf(windowCountStr, "%d", classCounts[i].count);
            strcat(out, ":");
            strcat(out, windowCountStr);
            strcat(out, "  ");
        }

        if (wlist) XFree(wlist);
        for (int i = 0; i < classCountSize; ++i) {
            free(classCounts[i].className);
        }
        free(classCounts);

        if (strlen(out) > maxLength) {
            out[maxLength-3] = '.';
            out[maxLength-2] = '.';
            out[maxLength-1] = '.';
            out[maxLength] = '\0';
        }


        if (lastOut == NULL || strcmp(lastOut, out) != 0) {
            printf("%s\n", out);
            fflush(stdout);
            free(lastOut);
            lastOut = strdup(out);
        }

        free(out);

        XNextEvent(display, &event);
        if (event.type == DestroyNotify) {
            continue;
        }
    }

    XCloseDisplay(display);
    exit(0);
}
