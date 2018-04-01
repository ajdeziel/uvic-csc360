/*
 * diskget.cpp
 * CSC 360 - Spring 2018 - P3 Part 3
 * 
 * Author: AJ Po-Deziel
 * Created on: 2018-03-16
 * 
 * Copy a file from a disk image's file system, 
 * and then write it to your local machine.
 * 
 * Code stylized using Google's C++ Style Guide.
 * Available @ https://google.github.io/styleguide/cppguide.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// Data structure for storing superblock info
struct Superblock {
    unsigned int block_size;        // Block size of file system
    unsigned int block_count;       // Number of blocks in file system
    unsigned int FAT_start_block;   // Block where FAT table starts
    unsigned int FAT_blocks;        // Number of blocks in FAT table
    unsigned int root_dir_start;    // Block where root directory starts
    unsigned int root_dir_blocks;   // Number of blocks for root directory
    unsigned int free_blocks;       // Number of free blocks
    unsigned int reserved_blocks;   // Number of reserved blocks
    unsigned int allocated_blocks;  // Number of allocated blocks
};

// Date & Time entry for file/directory
struct __attribute__((__packed__)) dir_entry_timedate_t {
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
};

// Directory entry
struct __attribute__((__packed__)) dir_entry_t {
    uint8_t                       status;
    uint32_t                      starting_block;
    uint32_t                      block_count;
    uint32_t                      size;
    struct   dir_entry_timedate_t modify_time;
    struct   dir_entry_timedate_t create_time;
    char                          filename[31];
    uint8_t                       unused[6];
};


int main(int argc, char* argv[]) {
    int fs = open(argv[1], O_RDWR);
    struct stat buffer;
    fstat(fs, &buffer);

    char *file_fs = argv[2];        // File copying from file system
    char *file_linux = argv[3];     // File writing on local machine
    
    // Get address in disk image file via memory map
    char* address = (char*)mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fs, 0);

    // Retrieve root directory info from disk image
    Superblock *sb = new Superblock;
    sb->block_size = ntohs(*(uint16_t*) &address[8]);
    sb->block_count = ntohl(*(uint32_t*) &address[10]);
    sb->FAT_start_block = ntohl(*(uint32_t*) &address[14]);
    sb->FAT_blocks = ntohl(*(uint32_t*) &address[18]);
    sb->root_dir_start = ntohl(*(uint32_t*) &address[22]);
    sb->root_dir_blocks = ntohl(*(uint32_t*) &address[26]);

    // Ensure for loop iterates for correct amount of root directory blocks 
    uint32_t dir_start_byte = sb->block_size * (sb->root_dir_start);
    uint32_t dir_end_byte = (sb->block_size * sb->root_dir_blocks) + dir_start_byte;

    // Boolean check for if file is found in file system
    bool file_found = false;

    vector<dir_entry_t> files;

    // Determine amount of free/reserved/allocated blocks in FAT table
    for (uint32_t entry = dir_start_byte; entry < dir_end_byte; entry += 64) {
        char file_disk[31];
        strncpy(file_disk, (char*)(uint8_t*) &address[entry + 27], 31);
        
        // If file match is encountered, retrieve info.
        // Otherwise, continue.
        if (strcmp(file_disk, file_fs) == 0) {
            file_found = true;
            uint8_t file_status = *(uint8_t*) &address[entry];

            dir_entry_t *fi = new dir_entry_t;
            dir_entry_timedate_t *dt = new dir_entry_timedate_t;

            // Determine if file or directory
            if (file_status == 0b00000011) {
                fi->status = 'F';
            } else if (file_status == 0b00000111) {
                fi->status = 'D';
            } else {
                continue;
            }

            // Get location of file data
            fi->starting_block = ntohl(*(uint32_t*) &address[entry + 1]);
            fi->block_count = ntohl(*(uint32_t*) &address[entry + 5]);
            
            fi->size = ntohl(*(uint32_t*) &address[entry + 9]);
            
            // Get last date modified
            dt->year = ntohs(*(uint16_t*) &address[entry + 20]);
            dt->month = *(uint8_t*) &address[entry + 22];
            dt->day = *(uint8_t*) &address[entry + 23];
            dt->hour = *(uint8_t*) &address[entry + 24];
            dt->minute = *(uint8_t*) &address[entry + 25];
            dt->second = *(uint8_t*) &address[entry + 26];

            fi->modify_time = *dt;

            strncpy(fi->filename, (char*)(uint8_t*) &address[entry + 27], 31);
            files.push_back(*fi);    
            break;
        } else {
            continue;
        }
    }

    // If file is not found, exit program.
    if (file_found == false) {
        cout << "File not found." << endl;
        return EXIT_FAILURE;
    }

    // Iterate through files vector to access file found
    for (auto &item : files) {
        FILE* file_linux_open;
        file_linux_open = fopen(file_linux, "w");

        // Copy file data from file system over to file on local machine
        char *file_data = (char*)(uint8_t*) &address[item.starting_block * (sb->block_size)];
        fwrite(file_data, sb->block_size, sizeof(file_data), file_linux_open);

        fclose(file_linux_open);
        break;
    }

    return EXIT_SUCCESS;
}