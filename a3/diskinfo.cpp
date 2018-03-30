/*
 * diskinfo.cpp
 * CSC 360 - Spring 2018 - P3 Part 1
 * 
 * Author: AJ Po-Deziel
 * Created on: 2018-03-16
 * 
 * Retrieve file system information from a disk image file.
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


int main(int argc, char* argv[]) {

    int fs = open(argv[1], O_RDWR);
    struct stat buffer;
    fstat(fs, &buffer);

    // Get address in disk image file via memory map
    char* address = (char*)mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fs, 0);

    // Convert from Big Endian to Little Endian
    // Store values into Superblock struct
    Superblock *sb = new Superblock;
    sb->block_size = ntohs(*(uint16_t*) &address[8]);
    sb->block_count = ntohl(*(uint32_t*) &address[10]);
    sb->FAT_start_block = ntohl(*(uint32_t*) &address[14]);
    sb->FAT_block_count = ntohl(*(uint32_t*) &address[18]);
    sb->root_dir_start = ntohl(*(uint32_t*) &address[22]);
    sb->root_dir_blocks = ntohl(*(uint32_t*) &address[26]);

    // Provide remaining FAT info attributes w/initial values
    sb->free_blocks = 0;
    sb->reserved_blocks = 0;
    sb->allocated_blocks = 0;

    // Ensure for loop iterates for correct amount of FAT entries
    uint32_t FAT_start_byte = 512; // + (sb->block_size * (sb->FAT_start_block - 1));
    uint32_t FAT_end_byte = (sb->block_size * sb->FAT_block_count) + FAT_start_byte;

    // Determine amount of free/reserved/allocated blocks in FAT table
    for (uint32_t entry = FAT_start_byte; entry < FAT_end_byte; entry += 4) {
        uint32_t current_entry = ntohl(*(uint32_t*) &address[entry]);

        if (current_entry == 0x00000000) {
            sb->free_blocks++;
        } else if (current_entry == 0x00000001) {
            sb->reserved_blocks++;
        } else {
            sb->allocated_blocks++;
        }
    }

    // Output - File System Info
    cout << "Superblock information:" << endl;
    cout << "Block size: " << sb->block_size << endl;
    cout << "Block count: " << sb->block_count << endl;
    cout << "FAT starts: " << sb->FAT_start_block << endl;
    cout << "FAT blocks: " << sb->FAT_block_count << endl;
    cout << "Root directory start: " << sb->root_dir_start << endl;
    cout << "Root directory blocks: " << sb->root_dir_blocks << endl;

    cout << "" << endl;

    cout << "FAT information: " << endl;
    cout << "Free Blocks: " << sb->free_blocks << endl;
    cout << "Reserved Blocks: " << sb->reserved_blocks << endl;
    cout << "Allocated Blocks: " << sb->allocated_blocks << endl;


    return EXIT_SUCCESS;
}