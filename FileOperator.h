#ifndef FILEOPERATOR_H
#define FILEOPERATOR_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <experimental/filesystem>
#include "File.h"

namespace fs = std::experimental::filesystem;

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
    // Recursively copies all files and folders from src to target and overwrites existing files in target.
    void copyRecursive(const fs::path& src, const fs::path& target) noexcept;
    void moveFile(std::string moveFrom, std::string moveTo);
    void moveFolder(std::string moveFrom, std::string moveTo);
    void createSymLink(std::string from, std::string to);
    void createHardLink(std::string from, std::string to);
};

#endif