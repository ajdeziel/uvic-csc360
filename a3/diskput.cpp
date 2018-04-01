/*
 * diskput.cpp
 * CSC 360 - Spring 2018 - P3 Part 4
 * 
 * Author: AJ Po-Deziel
 * Created on: 2018-03-16
 * 
 * Copy a file from your local machine, 
 * and then write it to a disk image's file system.
 * 
 * Code stylized using Google's C++ Style Guide.
 * Available @ https://google.github.io/styleguide/cppguide.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

// Superblock info
struct __attribute__((__packed__)) superblock_t {
    uint8_t     fs_id [8];
    uint16_t    block_size;                 // Block size of file system
    uint32_t    file_system_block_count;    // Number of blocks in file system
    uint32_t    FAT_start_block;            // Block where FAT table starts
    uint32_t    FAT_block_count;            // Number of blocks in FAT table
    uint32_t    root_dir_start_block;       // Block where root directory starts
    uint32_t    root_dir_block_count;       // Number of blocks for root directory
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

    char *file_disk = argv[1];   // Disk image file
    char *file_linux = argv[2];  // File to copy from local machine
    char *file_fs = argv[3];     // File to write to disk image's file system
        
    // Retrieve size of file to copy (in bytes)
    FILE *file_local;
    file_local = fopen(file_linux, "r");

    // Verify if file on local machine exists
    if (access(file_linux, F_OK) == -1) {
        cout << "File not found." << endl;
        return EXIT_FAILURE;
    }

    // Get size of local machine file
    fseek(file_local, 0, SEEK_END);
    unsigned int local_file_size = ftell(file_local);

    fseek(file_local, 0, SEEK_SET);
    char *file_data;

    // Read content of local machine file into char array
    fread(&file_data, 512, local_file_size, file_local);

    // Get address in disk image file via memory map
    char* address = (char*)mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fs, 0);

    // Convert from Big Endian to Little Endian
    // Store values into Superblock struct
    superblock_t *sb = new superblock_t;
    sb->block_size = ntohs(*(uint16_t*) &address[8]);
    sb->file_system_block_count = ntohl(*(uint32_t*) &address[10]);
    sb->FAT_start_block = ntohl(*(uint32_t*) &address[14]);
    sb->FAT_block_count = ntohl(*(uint32_t*) &address[18]);
    sb->root_dir_start_block = ntohl(*(uint32_t*) &address[22]);
    sb->root_dir_block_count = ntohl(*(uint32_t*) &address[26]);

    // Initialize struct to create entry in file system on disk image
    // dir_entry_timedate_t *file_date_to_copy = new dir_entry_timedate_t;
    dir_entry_t *file_to_copy = new dir_entry_t;

    // Number of blocks required for file
    file_to_copy->size = local_file_size;
    file_to_copy->block_count = (local_file_size / sb->block_size) + 1;

    // Get start/end FAT values for iteration over FAT table
    uint32_t FAT_start_byte = 512;
    uint32_t FAT_end_byte = (sb->block_size * sb->FAT_block_count) + FAT_start_byte;
    
    FILE *file_disk_fs;
    file_disk_fs = fopen(file_disk, "w");

    // Find available blocks, and write to them
    for (uint32_t entry = FAT_start_byte; entry < FAT_end_byte; entry += 4) {
        uint32_t current_entry = ntohl(*(uint32_t*) &address[entry]);

        if (current_entry == 0x00000000) {
            strncpy(file_to_copy->filename, file_fs, 31);
            file_to_copy->starting_block = current_entry;
            file_to_copy->block_count = (local_file_size / 512) + 1;
            file_to_copy->size = local_file_size;

            // Write file data to disk
            char *file_data = (char*)(uint8_t*) &address[current_entry * (sb->block_size)];
            fwrite(file_data, sb->block_size, sizeof(file_data), file_disk_fs);

            // Write file info to FAT table entry
            (*(uint32_t*) &address[entry]) = 0b00000011;
            (*(uint32_t*) &address[entry + 1]) = file_to_copy->starting_block;
            (*(uint32_t*) &address[entry + 5]) = file_to_copy->block_count;
            (*(uint32_t*) &address[entry + 9]) = file_to_copy->size;
        } else {
            continue;
        }
    }
}