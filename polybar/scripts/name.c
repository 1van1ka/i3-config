#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>

int printWindowTitle(Display *display, Window window){
	XClassHint classHint;
	XTextProperty textProp;
	char *window_name;

	int status = XGetClassHint(display,window,&classHint);
	if(status != 0){
		printf("(%s)",classHint.res_class);
		XFree(classHint.res_name);
		XFree(classHint.res_class);
	}

	status = XGetWMName(display, window, &textProp);
	if(status != 0){
		printf(" %s",textProp.value);
		XFree(textProp.value);
	}else{
		status = XFetchName(display, window, &window_name);
		if(status != 0) {
			printf("%s",window_name);
		}
	}	
	printf("\n");
	return 1;
}

int main(int argc, char *argv[])
{
Display *display;
Window focus;
int revert;


display = XOpenDisplay(NULL);

XGetInputFocus(display, &focus, &revert);

printWindowTitle(display, focus);

return 0;
}