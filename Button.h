#ifndef BUTTON_H
#define BUTTON_H

#include <string>

struct Button {
    std::string text;
    int x;
    int y;
    int width = 100;
    int height = 30;
};

#endif