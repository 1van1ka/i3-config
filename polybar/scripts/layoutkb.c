#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    Display *display;
    XkbStateRec state;
    char *layout = NULL;
    unsigned int statekb;

    // Открываем соединение с X-сервером
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Не удалось открыть соединение с X-сервером.\n");
        return 1;
    }

    // Инициализируем XKB
    int xkb_event_base, xkb_error_base, xkb_major = XkbMajorVersion, xkb_minor = XkbMinorVersion;
    int xkb_opcode;
    if (!XkbQueryExtension(display, &xkb_opcode, &xkb_event_base, &xkb_error_base, &xkb_major, &xkb_minor)) {
        fprintf(stderr, "XKB расширение не доступно.\n");
        XCloseDisplay(display);
        return 1;
    }

    // Получаем текущее состояние XKB
    if (XkbGetState(display, XkbUseCoreKbd, &state) != Success) {
        fprintf(stderr, "Не удалось получить состояние XKB.\n");
        XCloseDisplay(display);
        return 1;
    }

    if (XkbGetIndicatorState(display, XkbUseCoreKbd, &statekb) != Success) {
        fprintf(stderr, "Не удалось получить состояние индикаторов.\n");
        XCloseDisplay(display);
        return 1;
    }

    // Получаем описание клавиатуры
    XkbDescPtr xkb = XkbGetKeyboard(display, XkbAllComponentsMask, XkbUseCoreKbd);
    if (xkb == NULL) {
        fprintf(stderr, "Не удалось получить раскладки клавиатуры.\n");
        XCloseDisplay(display);
        return 1;
    }

    // Получаем текущую раскладку
    if (xkb->names != NULL) {
        Atom layout_atom = xkb->names->groups[state.group];
        layout = XGetAtomName(display, layout_atom);
    }

    // Выводим результат
    if (layout != NULL) {
        if (!strcmp(layout, "English (US)")) {
            layout = "us";
        } else if (!strcmp(layout, "Russian")) {
            layout = "ru";
        }
    } else {
        printf("Не удалось определить раскладку.\n");
    }

    // Проверяем состояние Caps Lock
    if (statekb& 1 << 0) {
        printf("%%{F#dec800}%s%%{F-}\n", layout);
    } else {
        printf("%s", layout);
    }

    // // Проверяем состояние Num Lock
    // if (statekb& 1 << 1) {
    //     printf("Num Lock активен\n");
    // } else {
    //     printf("Num Lock не активен\n");
    // }

    // Освобождаем ресурсы
    XkbFreeKeyboard(xkb, XkbAllComponentsMask, True);
    XCloseDisplay(display);

    return 0;
}
