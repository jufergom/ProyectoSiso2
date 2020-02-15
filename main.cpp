#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//for listing files on directory
#include <dirent.h> 
//for string and vector
#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include "Button.h"
#include "FileShow.h"

using namespace std;

void drawFiles(Display *display, Window *window, int s, vector<FileShow> files);
void drawButtons(Display *display, Window *window, int s, vector<Button> buttons);
void drawInput(Display *display, Window *window, int s);
vector<FileShow> getFilesOnDirectory(string directory);
//combine the window title with the current directory
void changeWindowTitle(Display *display, Window *window, char *title, string currentDirectory);
//change input text
void changeTextInput(XEvent event, Display *display);

string textInput;

int main() {
    Display *display;
    Window window;
    XEvent event;
    stack<string> navigation; //stores all paths of navigated directories
    navigation.push(getenv("HOME"));
    int s;

    char *title = new char(); //the window title
 
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

    changeWindowTitle(display, &window, title, navigation.top());

    //create buttons and vector of buttons
    vector<Button> buttons;
    Button b1, b2, b3;
    b1.x = 500;
    b1.y = 60;
    b1.text = "Back";
    buttons.push_back(b1);
    b2.x = 500;
    b2.y = 100;
    b2.text = "Nueva carpeta";
    buttons.push_back(b2);
    b3.x = 500;
    b3.y = 140;
    b3.text = "Nuevo archivo";
    buttons.push_back(b1);
    
 
    /* event loop */
    for (;;) {
        XNextEvent(display, &event);
        changeTextInput(event, display);
        vector<FileShow> files = getFilesOnDirectory(navigation.top());
        /* draw or redraw the window */
        if (event.type == Expose) {
            drawFiles(display, &window, s, files);
            drawButtons(display, &window, s, buttons);
            drawInput(display, &window, s);
        }
        /* Click pressed */
        if (event.type == ButtonPress) {
            //left click
            if(event.xbutton.button == 1) {
                //collision with mouse and a file
                for(int i = 0; i < files.size(); i++) {
                    if(event.xbutton.x >= files[i].x && event.xbutton.x < files[i].x + files[i].width 
                    && event.xbutton.y >= files[i].y && event.xbutton.y < files[i].y + files[i].height) {
                        //if collision is made with a folder
                        if(files[i].type == 4) {
                            //pushes navigation to stack
                            navigation.push(navigation.top()+"/"+files[i].name);
                            vector<FileShow> files = getFilesOnDirectory(navigation.top());
                            XClearWindow(display, window);
                            drawFiles(display, &window, s, files);
                            drawButtons(display, &window, s, buttons);
                            drawInput(display, &window, s);
                            changeWindowTitle(display, &window, title, navigation.top());
                        }
                        //if collision is made with a file
                        else if(files[i].type == 8) {
                            string command = "xdg-open "+navigation.top()+"/"+files[i].name;
                            system(command.c_str()); //open file
                        }
                    }
                }
                //collision with mouse and button
                for(int i = 0; i < buttons.size(); i++) {
                    if(event.xbutton.x >= buttons[i].x && event.xbutton.x < buttons[i].x + buttons[i].width 
                    && event.xbutton.y >= buttons[i].y 
                    && event.xbutton.y < buttons[i].y + buttons[i].height) {
                        //go back
                        if(buttons[i].text == "Back") {
                            //make sure that we cannot go back from home
                            if(navigation.size() > 1) {
                                navigation.pop(); //pop current directory from navigation
                                vector<FileShow> files = getFilesOnDirectory(navigation.top());
                                XClearWindow(display, window);
                                drawFiles(display, &window, s, files);
                                drawButtons(display, &window, s, buttons);
                                drawInput(display, &window, s);
                                changeWindowTitle(display, &window, title, navigation.top());
                            }
                            
                        }
                    }
                }
            }
        }
        else if(event.type == KeyPress) {
            XClearWindow(display, window);
            drawFiles(display, &window, s, files);
            drawButtons(display, &window, s, buttons);
            drawInput(display, &window, s);
        }
    }
    /* close connection to server */
    XCloseDisplay(display);

    return 0;
}

void drawFiles(Display *display, Window *window, int s, vector<FileShow> files) {
    for(int i = 0; i < files.size(); i++) {
        //folder
        if(files[i].type == 4) {
            XFillRectangle(display, *window, DefaultGC(display, s), 2, 20*(i+1)-10, 15, 10);
        }
        else {
            XFillRectangle(display, *window, DefaultGC(display, s), 2, 20*(i+1)-10, 15, 5);
        }
        //name of file or directory
        XDrawString(display, *window, DefaultGC(display, s), 20, 20*(i+1), 
                files[i].name.c_str(), strlen(files[i].name.c_str()));
    }
}

void drawButtons(Display *display, Window *window, int s, vector<Button> buttons) {
    for(int i = 0; i < buttons.size(); i++) {
        XDrawLine(display, *window, DefaultGC(display, s), buttons[i].x, buttons[i].y, 
                buttons[i].x, buttons[i].y + buttons[i].height); //left
        XDrawLine(display, *window, DefaultGC(display, s),  buttons[i].x+buttons[i].width, buttons[i].y, 
                buttons[i].x+buttons[i].width, buttons[i].y + buttons[i].height); //right
        XDrawLine(display, *window, DefaultGC(display, s), buttons[i].x, buttons[i].y, 
                buttons[i].x + buttons[i].width, buttons[i].y); //up
        XDrawLine(display, *window, DefaultGC(display, s), buttons[i].x, buttons[i].y + buttons[i].height, 
                buttons[i].x + buttons[i].width, buttons[i].y + buttons[i].height); //down
        XDrawString(display, *window, DefaultGC(display, s), buttons[i].x + 5, buttons[i].y + 20, 
                buttons[i].text.c_str(), strlen(buttons[i].text.c_str())); //text shown inside button
    }
}

void drawInput(Display *display, Window *window, int s) {
    XDrawString(display, *window, DefaultGC(display, s), 500, 20, 
            textInput.c_str(), strlen(textInput.c_str())); 
    XDrawLine(display, *window, DefaultGC(display, s), 500, 25, 700, 25);
}

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
                f.type = static_cast<unsigned>(dir->d_type);
                f.name = str;
                f.x = 2;
                f.y = 20*(i+1)-10;
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

//combine the window title with the current directory
void changeWindowTitle(Display *display, Window *window, char *title, string currentDirectory) {
    //combine the window title with the current directory
    strcpy(title, "Explorador de archivos: ");
    strcat(title, currentDirectory.c_str());
    XStoreName(display, *window, title);
}

void changeTextInput(XEvent event, Display *display) {
    if(event.type == KeyPress) {
        KeySym id = XkbKeycodeToKeysym(display, event.xkey.keycode, 0,
            event.xkey.state & ShiftMask ? 1 : 0);
        char stringData[1];
        XComposeStatus x;
        XLookupString(&event.xkey,stringData, 1, &id, &x);
        if(id == 65288) {
            if(textInput.size()>0){
                textInput = textInput.substr(0, textInput.size()-1);
            }
        }
        else {
            textInput = textInput + stringData;
        }
    }
}