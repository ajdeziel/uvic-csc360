/*
 * diskinfo.cpp
 * CSC 360 - Spring 2018 - P3 Part 1
 * 
 * Author: AJ Po-Deziel
 * Created on: 2018-03-16
 * 
 * Something goes here
 * 
 * 
 * Byte error (got Endianness wrong): 1900 000
 * Padding error (didn't handle padding well): 0019 0000
 * Both wrong: 19
 */

#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

    
    cout << "Superblock information:";
    cout << "Block size: ";
    cout << "Block count: ";
    cout << "FAT size: ";

    cout << "";

    cout << "FAT blocks: ";
    cout << "Root directory start: ";
    cout << "Root directory blocks: ";

    cout << "";

    cout << "FAT information: ";
    cout << "Free Blocks: ";
    cout << "Reserved Blocks: ";
    cout << "Allocated Blocks: ";



}