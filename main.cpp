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
#include <stack>

using namespace std;

struct FileShow {
    string name;
    int x;
    int y;
    unsigned int type; //4 for directory, 8 for file
    int width;
    int height;
};

void drawFiles(Display *display, Window *window, int s, vector<FileShow> files) {
    for(int i = 0; i < files.size(); i++) {
        //icon of file or directory
        XFillRectangle(display, *window, DefaultGC(display, s), 2, 20*(i+1)-10, 15, 10);
        //name of file or directory
        XDrawString(display, *window, DefaultGC(display, s), 20, 20*(i+1), 
                files[i].name.c_str(), strlen(files[i].name.c_str()));
    }
}

struct Button {
    string text;
    int x;
    int y;
    int width = 100;
    int height = 30;
};

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
    /*
    XDrawLine(display, window, DefaultGC(display, s), 300, 20, 300, 50); //left
    XDrawLine(display, window, DefaultGC(display, s), 400, 20, 400, 50); //right
    XDrawLine(display, window, DefaultGC(display, s), 300, 20, 400, 20); //up
    XDrawLine(display, window, DefaultGC(display, s), 300, 50, 400, 50); //down
    XDrawString(display, window, DefaultGC(display, s), 305, 40, 
            "Back", 4);
    x = 300, y = 20
    */
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
    Button b;
    b.x = 500;
    b.y = 20;
    b.text = "Back";
    buttons.push_back(b);
 
    /* event loop */
    for (;;) {
        XNextEvent(display, &event);
        vector<FileShow> files = getFilesOnDirectory(navigation.top());
        /* draw or redraw the window */
        if (event.type == Expose) {
            drawFiles(display, &window, s, files);
            drawButtons(display, &window, s, buttons);
        }
        /* Click pressed */
        if (event.type == ButtonPress) {
            //left click
            if(event.xbutton.button == 1) {
                //collision with mouse and a file
                for(int i = 0; i < files.size(); i++) {
                    if(event.xbutton.x >= files[i].x && event.xbutton.x < files[i].x + files[i].width 
                    && event.xbutton.y >= files[i].y && event.xbutton.y < files[i].y + files[i].height) {
                        //pushes navigation to stack
                        navigation.push(navigation.top()+"/"+files[i].name);
                        vector<FileShow> files = getFilesOnDirectory(navigation.top());
                        XClearWindow(display, window);
                        drawFiles(display, &window, s, files);
                        drawButtons(display, &window, s, buttons);
                        changeWindowTitle(display, &window, title, navigation.top());
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
                                changeWindowTitle(display, &window, title, navigation.top());
                            }
                            
                        }
                    }
                }
            }
        }
    }
    /* close connection to server */
    XCloseDisplay(display);

    return 0;
}