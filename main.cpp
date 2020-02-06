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
    int width;
    int height;
};

struct Button {
    string text;
    int x;
    int y;
    int width = 100;
    int height = 30;
};

vector<FileShow> getFilesOnDirectory(string directory) {
    DIR *d;
    struct dirent *dir;
    vector<FileShow> files;
    FileShow f;
    int i = 0;
    d = opendir(directory.c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if(dir->d_name[0] != '.') {
                //printf("%s\n", dir->d_name);
                std::string str(dir->d_name);
                f.name = str;
                f.x = 2;
                f.y = 20*(i+1)-10;
                f.file = false;
                f.width = 75;
                f.height = 10;
                files.push_back(f);
                i++;
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
    string previousDirectory = currentDirectory;
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
    XSelectInput(display, window, ExposureMask | ButtonPressMask | KeyPressMask);
 
    /* map (show) the window */
    XMapWindow(display, window);
 
    /* event loop */
    for (;;) {
        XNextEvent(display, &event);
        vector<FileShow> files = getFilesOnDirectory(currentDirectory);
        /* draw or redraw the window */
        if (event.type == Expose) {
            for(int i = 0; i < files.size(); i++) {
                XFillRectangle(display, window, DefaultGC(display, s), 2, 20*(i+1)-10, 15, 10);
                XDrawString(display, window, DefaultGC(display, s), 20, 20*(i+1), 
                        files[i].name.c_str(), strlen(files[i].name.c_str()));
                //button simulation
                XDrawLine(display, window, DefaultGC(display, s), 300, 20, 300, 50); //left
                XDrawLine(display, window, DefaultGC(display, s), 400, 20, 400, 50); //right
                XDrawLine(display, window, DefaultGC(display, s), 300, 20, 400, 20); //up
                XDrawLine(display, window, DefaultGC(display, s), 300, 50, 400, 50); //down
                XDrawString(display, window, DefaultGC(display, s), 305, 40, 
                        "Back", 4);
            }
        }
        /* Click pressed */
        if (event.type == ButtonPress) {
            //left click
            if(event.xbutton.button == 1) {
                for(int i = 0; i < files.size(); i++) {
                    //collision with mouse and a file
                    if(event.xbutton.x >= files[i].x && event.xbutton.x < files[i].x + files[i].width 
                    && event.xbutton.y >= files[i].y && event.xbutton.y < files[i].y + files[i].height) {
                        //cout << "Click was made on " << files[i].name << endl;
                        previousDirectory = currentDirectory;
                        currentDirectory += "/"+files[i].name;
                        vector<FileShow> files = getFilesOnDirectory(currentDirectory);
                        XClearWindow(display, window);
                        for(int j = 0; j < files.size(); j++) {
                            XFillRectangle(display, window, DefaultGC(display, s), 2, 20*(j+1)-10, 15, 10);
                            XDrawString(display, window, DefaultGC(display, s), 20, 20*(j+1), 
                                    files[j].name.c_str(), strlen(files[j].name.c_str()));
                        }
                    }
                }
            }
            //back click
            else if(event.xbutton.button == 8) {
                currentDirectory = previousDirectory;
                vector<FileShow> files = getFilesOnDirectory(currentDirectory);
                XClearWindow(display, window);
                for(int j = 0; j < files.size(); j++) {
                    XFillRectangle(display, window, DefaultGC(display, s), 2, 20*(j+1)-10, 15, 10);
                    XDrawString(display, window, DefaultGC(display, s), 20, 20*(j+1), 
                            files[j].name.c_str(), strlen(files[j].name.c_str()));
                }
            }
            //cout << event.xbutton.x << endl;
            //cout << event.xbutton.y << endl;
        }
    }
    /* close connection to server */
    XCloseDisplay(display);
 
    return 0;
}