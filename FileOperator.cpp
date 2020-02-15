#include "FileOperator.h"

FileOperator::FileOperator() {

}

void FileOperator::createFile(std::string fileName) {
    int fd;
    fd = open(fileName.c_str(), O_CREAT | O_WRONLY,S_IRUSR); //creates file
    close(fd);
}

void FileOperator::makeDirectory(std::string directoryName) {
    mkdir(directoryName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}
