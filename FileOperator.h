#ifndef FILEOPERATOR_H
#define FILEOPERATOR_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

class FileOperator {
public:
    FileOperator();

    void makeDirectory(std::string directoryName);
    void createFile(std::string fileName);
    int removeFile(std::string fileName);
};

#endif