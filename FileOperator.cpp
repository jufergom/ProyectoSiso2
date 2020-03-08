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

File FileOperator::readFile(std::string fileName) {
    int fd = -1;
    ssize_t bytes_read = -1;
    File f;
    fd = open(fileName.c_str(), O_RDONLY);
    
    if(fd == -1) {
        //open failed
        f.size = 0;
        return f;
    }
    // Get size.
    off_t size = lseek(fd, 0, SEEK_END); // You should check for an error return in real code
    // Seek back to the beginning.
    lseek(fd, 0, SEEK_SET);
    // Allocate enough to hold the whole contents plus a '\0' char.
    f.buffer = new char[size + 1];

    bytes_read = read(fd, f.buffer, size); 
    f.size = size;

    close(fd);
    return f;
}

void FileOperator::writeFile(std::string fileName, File f) {
    int fd;
    fd = open(fileName.c_str(), O_CREAT | O_WRONLY,S_IRUSR); //creates file
    
    write(fd, f.buffer, f.size);
    close(fd);
}

void FileOperator::copyFile(std::string copyFrom, std::string copyTo) {
    File f;
    f = readFile(copyFrom);
    if(f.size != -1) 
        writeFile(copyTo, f);
}

void FileOperator::copyRecursive(const fs::path& src, const fs::path& target) noexcept {
    try {
        fs::copy(src, target, fs::copy_options::overwrite_existing | fs::copy_options::recursive);
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }
}

void FileOperator::moveFile(std::string moveFrom, std::string moveTo) {
    rename(moveFrom.c_str(), moveTo.c_str());
}

void FileOperator::moveFolder(std::string moveFrom, std::string moveTo) {
    fs::rename(moveFrom, moveTo);
}