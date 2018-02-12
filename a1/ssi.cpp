/*
 * ssi.cpp 
 * Author: AJ Po-Deziel
 * Created on: 2018-01-27
 * 
 * Provides a simple shell interpreter (SSI) from a command line terminal.
 * 
 * SSI performs the following operations:
 * 1) Basic Execution
 * 2) Changing directories
 * 3) Background Execution & Listing
 *
 * Styled according to Google C++ Style Guide
 * Available @ https://google.github.io/styleguide/cppguide.html
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct bg_pro{ 
    pid_t pid; 
    string command; 
    struct bg_pro* next; 
};

char** ConvertStringArgs(std::vector<string> tokens) {
    // Returns char*[] for a vector of string tokens.
    // Converts tokenized string arguments into array of
    // char* (creating char*[] or char**) for use in
    // execvp().

	char **args = new char*[tokens.size() + 1];
	for (int i = 0; i < tokens.size(); i++) {
        char *convert = new char[tokens[i].size() + 1];
		strncpy(convert, tokens[i].c_str(), tokens[i].size() + 1);
        args[i] = convert;
	}
	
    args[tokens.size()] = NULL;
	
    return args;
}


void CheckBgExecution(bg_pro *root, pid_t pid) {
	// Check if background process is currently executing.
	// This will let the shell know if there are processes
	// that still require execution.
	bg_pro *current = root;
	
	if (root->pid == pid) {
		cout << root->pid << " " << root->command << " has terminated." << endl;
		root = root->next;
	} else {
		while (current == NULL) {
			if (current->pid == pid) {
				cout << current->pid << current->command << " has terminated." << endl;
				current = current->next;
			}
		}
	}
}


int main() {
    // Linux user commands
    string exit_condition = "exit";
    string cd_cmd = "cd";
    string bg_cmd = "bg";
    string bglist_cmd = "bglist";

	// Variables to track background processes
	bg_pro *root = new bg_pro;
	bg_pro *tail = new bg_pro;
	int process_count = 0;

    while(true) {
		string input;
	    std::vector<string> tokenized;

        // Request user input
        cout << "SSI: " << getcwd(NULL, 0) << " > ";
        getline(cin, input);		
		
		//Check for child processes
		if (process_count > 0) {
			pid_t pid_terminate = waitpid(-1, NULL, WNOHANG);
			
			if (pid_terminate > 0){
				CheckBgExecution(root, pid_terminate);
			}
		}

        // Evaluate if exit condition is met
        if (exit_condition.compare(input) == 0
            || exit_condition.compare(input) == 10) {
            return EXIT_SUCCESS;
        }		

        // Tokenize input into vector of strings.
        // Inspired from https://www.geeksforgeeks.org/tokenizing-a-string-cpp
        stringstream inputstream(input);
        string input_item;

        while(getline(inputstream, input_item, ' ')) {
            tokenized.push_back(input_item);
        }

        // Go back to start of while loop if nothing is inputted.
        if (tokenized.size() < 1) {
            continue;
        }
		
        // Retrieve user command inputted
		string cmd_input = tokenized[0];

		// 3.3 Background Execution
		//
		// 3.3.1 Command Execution and Storage
		// Execute commands submitted in the background when passed 
		// command input "bg". Store processes in bg_pro struct if
		// it cannot be executed right away.
        if (cmd_input.compare(bg_cmd) == 0) {
            char **args = ConvertStringArgs(tokenized);
            
            pid_t pid = fork();
			
			//If child, execute command
            if (pid == 0) {
				execvp(args[0], args);
            } else {
                // Parent process
                bg_pro *process = new bg_pro;
                process->pid = pid;
                process->command = input;
                process->next = NULL;

                // If no processes exist, start at root.
				// Otherwise, add to linked list.
                if (process_count == 0) {
                    root = process;
					tail = process;
					process_count++;
                } else {
					tail->next = process;
					tail = process;
					process_count++;
                }
            }
        } 
		// 3.3.2 bglist
		// Print out all processes currently waiting to be executed
		// in the background.
		else if (cmd_input.compare(bglist_cmd) == 0) {
            bg_pro *current = root;
			
			while (current != NULL) {
                cout << current->pid << ": " << getcwd(NULL, 0) << " " << current->command << endl;
				current = current->next;
            }
        }		
		// 3.2 Changing Directories: Verify if Linux command is cd.
		// If true, change directories according to argument passed.
		else if (cmd_input.compare(cd_cmd) == 0
            || cmd_input.compare(cd_cmd) == 10) {
			string path_return;

            // No path provided or "~"" implies HOME directory requested.
            // Otherwise, change directory to path provided.
			if (tokenized[1].empty() || tokenized[1].compare("~") == 0) {
				path_return = getenv("HOME");
			} else {
				path_return = tokenized[1];
			}
			
			chdir(path_return.c_str());
			continue;
        }
		//3.1 Basic Execution
		else {
			pid_t pid = fork();
			
			if (pid == 0) {
				char **args = ConvertStringArgs(tokenized);
				execvp(args[0], args);
			} else {
                waitpid(pid, NULL, 0);
            }
            continue;
		}
		
    }
}