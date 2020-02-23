#include "FileOperator.h"

FileOperator::FileOperator() {

}

void FileOperator::createFile(std::string fileName) {
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    fd = creat(fileName.c_str(), mode);
    close(fd);
}

void FileOperator::makeDirectory(std::string directoryName) {
    mkdir(directoryName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

int FileOperator::removeFile(std::string fileName) {
    return remove(fileName.c_str());
}