*******************************************************************************
README.txt

Author: AJ Po-Deziel
UVic ID: V00815622
Created on: 2018-01-27

CSC 360 - Assignment 1
Documentation for Simple Shell Interpreter (SSI)
*******************************************************************************


***********
Prequisites
***********

To run the Simple Shell Interpreter, a compiler (e.g. g++) supporting C++11 
or higher will be required.

**NOTE**: 	Currently, the program does compile and run on UVic's CSC 
			Linux server (linux.csc.uvic.ca).


**********************
How to compile and run
**********************

	1) 	Unzip compressed submission file. All files related to the program
		that are required to run it are included inside.
		
	2)	Ensure that the g++ compiler installed supports C++11 or higher. If
		not, please update the compiler to a compatible version.
	
	3)	Open Terminal (Linux OS/macOS) or PowerShell/Command Prompt (Windows).
	
	4)	Navigate in command line window to submission folder.
	
	5)	Compile program by executing the following command:
		
			make
		
		If successful, a new command line will then appear.

		
	6)	To run the program, execute the following command:
	
			./ssi
			
		This will launch the "shell-within-a-shell" program. Begin executing
		commands as you normally would within any terminal.
		
	7)	To exit the Simple Shell Interpreter, wait till the program requests
		user input (i.e. terminal awaits command prompt). Once this occurs,
		type the following command:
		
			exit
		
		This will allow the program to close and exit successfully.