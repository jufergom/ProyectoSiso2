#ifndef FILESHOW_H
#define FILESHOW_H

#include <string>

struct FileShow {
    std::string name;
    int x;
    int y;
    unsigned int type; //4 for folder, 8 for file
    int width;
    int height;
};

#endif