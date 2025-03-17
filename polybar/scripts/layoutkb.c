#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *GetLayoutName(Display *display) {
    XkbStateRec state;
    XkbGetState(display, XkbUseCoreKbd, &state);
    
    int layoutNumber = state.group;
    
    const char *layoutNames[] = {
        "US", "RU"
    };
    
    if (layoutNumber >= 0 && layoutNumber < sizeof(layoutNames) / sizeof(layoutNames[0])) {
        return strdup(layoutNames[layoutNumber]);
    } else {
        return strdup("Unknown");
    }
}

int main(void) {
    Display *display;
    unsigned int statekb;

    if ((display = XOpenDisplay(NULL)) == NULL) {
        puts("Can not connect to the X server!\n");
        return 1;
    }

    char *layoutName = GetLayoutName(display);
    
    if (XkbGetIndicatorState(display, XkbUseCoreKbd, &statekb) != Success) {
        fprintf(stderr, "Не удалось получить состояние индикаторов.\n");
        free(layoutName);
        XCloseDisplay(display);
        return 1;
    }

    if (statekb & (1 << 0)) {
        printf("%%{F#ff0000}%s%%{F-}\n", layoutName);
    } else {
        printf("%s\n", layoutName);
    }

    free(layoutName);
    
    XCloseDisplay(display);
    return 0;
}
