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

#include <string>
#include <vector>

struct file_item {
    int file_type;
    char file_size[10];
    char file_name[30];
    string date_modified;
};