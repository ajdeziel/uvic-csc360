/*
 * disklist.cpp
 * CSC 360 - Spring 2018 - P3 Part 2
 * 
 * Author: AJ Po-Deziel
 * Created on: 2018-03-16
 * 
 * Something goes here
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <ctime>
#include <fstream>
#include <iostream>

using namespace std;

struct Superblock {
    unsigned int block_size;        // Block size of file system
    unsigned int block_count;       // Number of blocks in file system
    unsigned int FAT_start_block;   // Block where FAT table starts
    unsigned int FAT_block_count;   // Number of blocks in FAT table
    unsigned int root_dir_start;    // Block where root directory starts
    unsigned int root_dir_blocks;   // Number of blocks for root directory
    unsigned int free_blocks;       // Number of free blocks
    unsigned int reserved_blocks;   // Number of reserved blocks
    unsigned int allocated_blocks;  // Number of allocated blocks
};

struct file_item {
    char file_type[1];
    char file_size[10];
    char file_name[30];
    time_t date_modified;
};

int main(int argc, char* argv[]) {
    int fs = open(argv[1], O_RDWR);
    struct stat buffer;
    fstat(fs, &buffer);

    string directory = argv[2];

    if (directory.compare("/") != 0) {
        cout << "This program currently only handles root directories. " << 
                "Do not enter any subdirectories as an argument." << endl;
        return EXIT_FAILURE;
    }
}