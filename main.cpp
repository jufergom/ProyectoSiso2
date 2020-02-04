#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//for listing files on directory
#include <dirent.h> 
//for string and vector
#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct FileShow {
    string name;
    int x;
    int y;
    bool file; //true if it represents a file, false if it represents a folder
};

vector<string> getFilesOnDirectory(string directory) {
    DIR *d;
    struct dirent *dir;
    vector<string> files;
    d = opendir(directory.c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir->d_name[0] != '.') {
                //printf("%s\n", dir->d_name);
                std::string str(dir->d_name);
                files.push_back(str);
            }
                
        }
        closedir(d);
    } 
    return files;
}

int main() {
    Display *display;
    Window window;
    XEvent event;
    string currentDirectory = getenv("HOME");
    int s;
 
    /* open connection with the server */
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }
 
    s = DefaultScreen(display);
 
    /* create window */
    window = XCreateSimpleWindow(display, RootWindow(display, s), 10, 10, 800, 600, 1,
                           BlackPixel(display, s), WhitePixel(display, s));

    XGrabPointer(display, window, false, ButtonPressMask, GrabModeAsync, 
            GrabModeAsync, None, None, CurrentTime);
 
    /* select kind of events we are interested in */
    XSelectInput(display, window, ExposureMask | ButtonPressMask);
 
    /* map (show) the window */
    XMapWindow(display, window);
 
    /* event loop */
    for (;;) {
        XNextEvent(display, &event);
 
        /* draw or redraw the window */
        if (event.type == Expose) {
            vector<string> files = getFilesOnDirectory(currentDirectory);
            for(int i = 0; i < files.size(); i++) {
                XFillRectangle(display, window, DefaultGC(display, s), 2, 20*(i+1)-10, 15, 10);
                XDrawString(display, window, DefaultGC(display, s), 20, 20*(i+1), 
                        files[i].c_str(), strlen(files[i].c_str()));
            }
            //XFillRectangle(display, window, DefaultGC(display, s), 20, 20, 10, 10);
            //XDrawString(display, window, DefaultGC(display, s), 50, 50, msg, strlen(msg));

        }
        /* exit on key press */
        if (event.type == ButtonPress) {
            cout << event.xbutton.x << endl;
            cout << event.xbutton.y << endl;
        }
    }
 
    /* close connection to server */
    XCloseDisplay(display);
 
    return 0;
}