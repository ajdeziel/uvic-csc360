/*
 * disklist.cpp
 * CSC 360 - Spring 2018 - P3 Part 2
 * 
 * Author: AJ Po-Deziel
 * Created on: 2018-03-16
 * 
 * List files for a given directory from a disk image's file system.
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
    
    // Get address in disk image file via memory map
    char* address = (char*)mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fs, 0);

    // Retrieve root directory info from disk image
    Superblock *sb = new Superblock;
    sb->block_size = ntohs(*(uint16_t*) &address[8]);
    sb->root_dir_start = ntohl(*(uint32_t*) &address[22]);
    sb->root_dir_blocks = ntohl(*(uint32_t*) &address[26]);

    // Ensure for loop iterates for correct amount of root directory blocks 
    uint32_t dir_start_byte = sb->block_size * (sb->root_dir_start);
    uint32_t dir_end_byte = (sb->block_size * sb->root_dir_blocks) + dir_start_byte;

    vector<dir_entry_t> directory_items;

    // Determine amount of free/reserved/allocated blocks in FAT table
    for (uint32_t entry = dir_start_byte; entry < dir_end_byte; entry += 64) {
        uint8_t file_status = *(uint8_t*) &address[entry];

        dir_entry_t *fi = new dir_entry_t;
        dir_entry_timedate_t *dt = new dir_entry_timedate_t;

        if (file_status == 0b00000011) {
            fi->status = 'F';
        } else if (file_status == 0b00000111) {
            fi->status = 'D';
        } else {
            continue;
        }

        fi->size = ntohl(*(uint32_t*) &address[entry + 9]);
        
        dt->year = ntohs(*(uint16_t*) &address[entry + 20]);
        dt->month = *(uint8_t*) &address[entry + 22];
        dt->day = *(uint8_t*) &address[entry + 23];
        dt->hour = *(uint8_t*) &address[entry + 24];
        dt->minute = *(uint8_t*) &address[entry + 25];
        dt->second = *(uint8_t*) &address[entry + 26];

        fi->modify_time = *dt;

        strncpy(fi->filename, (char*)(uint8_t*) &address[entry + 27], 31);

        directory_items.push_back(*fi);
    }

    // Print directory items in vector
    for (auto &item : directory_items) {
        printf("%c ", item.status);
        printf("%10d ", item.size);
        printf("%30s ", item.filename);
        printf("%4d/%02d/%02d %02d:%02d:%02d\n", item.modify_time.year, 
                                            item.modify_time.month,
                                            item.modify_time.day,
                                            item.modify_time.hour,
                                            item.modify_time.minute,
                                            item.modify_time.second);
    }

}