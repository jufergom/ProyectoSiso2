#ifndef FILEOPERATOR_H
#define FILEOPERATOR_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "File.h"

class FileOperator {
private:
    File readFile(std::string fileName);
    void writeFile(std::string fileName, File f);
public:
    FileOperator();

    void makeDirectory(std::string directoryName);
    void createFile(std::string fileName);
    int removeFile(std::string fileName);
    void copyFile(std::string copyFrom, std::string copyTo);
};

#endif