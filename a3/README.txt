*******************************************************************************

README.txt
CSC 360 - Spring 2018
Programming Assignment 3 (p3)

Author: AJ Po-Deziel
Created on: 2018-03-30

*******************************************************************************



Files Included
-------------------------------------------------------------------------------
- diskinfo.cpp
- disklist.cpp
- diskget.cpp
- diskput.cpp
- README.txt



A few notes to read before deployment
-------------------------------------------------------------------------------
- Particular notes about specific implementation and design choices are 
  provided throughout each of the programs as comments, with explanations
  about what each section of code does.

- disklist, diskget, and diskput only deal with the root directory.
  Any attempt to pass in subdirectories will cause unexpected behaviour
  in each of the expected programs.

- The disk image file must be passed in as an argument. Failure to do so
  will cause the program to unexpected behaviour (i.e. segmentation fault).



Deployment Instructions
-------------------------------------------------------------------------------
1. Open Terminal on UNIX OS, or PowerShell/Command Prompt on Windows OS.

2. Naviagte to directory containing submission files.

3. Compile submission files with the following command:

        make

4. To run each file, use the following respective format for each program:

        For diskinfo:
            ./diskinfo [disk image file]

        For disklist:
            ./disklist [disk image file]

        For diskget:
            ./diskget [disk image file] [file copying from] [file copying to in current directory]

        For diskput:
            ./diskput [disk image file] [file copying from in current directory] [file copying to]


   Where [disk image file] is the disk image provided that provides the file system being analyzed.
