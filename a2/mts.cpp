/* CSC 360 - Assignment 2
 * Multi-Thread Scheduling
 * 
 * Author: AJ Po-Deziel
 * Created on: 2018-02-15
 * 
 * This program performs the scheduling of trains over a central shared piece
 * of track, where train information is sourced from a provided text file.
 * 
 * Styled according to Google C++ Style Guide
 * Available @ https://google.github.io/styleguide/cppguide.html
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <exception>
#include <ifstream>
#include <queue>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Train{
    pthread_t train_thread;
    string priority;
    string direction;
    int load_time;
    int cross_time;
};

int main(int argc, char **argv[]) {

    if (argc <= 1) {
        throw "No train schedule file has been provided. Please provide a train schedule file before continuing.";
    } else if (argc > 2) {
        throw "Too many train schedule files have been provided. Please only provide a single train schedule file before continuing.";
    }

    std::ifstream file;
    std::priority_queue<Train> train_queue;
    std::vector<Train> trains;
    file.open(argv[1], std::ifstream::in);

    int count = 0;

    for (string line; getline(file, line); ) {
        std::vector<string> read_lines;
        pthread train_pro;

        // Tokenize input into vector of strings.
        // Originally designed for Assignment 1
        // Inspired from https://www.geeksforgeeks.org/tokenizing-a-string-cpp
        stringstream inputstream(line);
        string input;

        while(getline(inputstream, input, ' ')) {
            read_lines.push_back(input);
        }

        // Refer to www.cplusplus.com/forum/general/37962
        // Create train in priority queue
        Train *train_line = new Train;
        train_line->train_thread = pthread_create(&train_pro, NULL, );
        train_line->load_time = stoi(read_lines[1]);
        train_line->cross_time = stoi(read_lines[2]);

        // Verify train priority & direction
        if (read_lines[0].compare("E") == 0) {
            // High priority Eastbound
            train_line->priority = "High";
            train_line->direction = "East";
        } else if (read_lines[0].compare("e") == 0) {
            // Low priority Eastbound
            train_line->priority = "Low";
            train_line->direction = "East";
        } else if (read_lines[0].compare("W") == 0) {
            // High priority Westbound
            train_line->priority = "High";
            train_line->direction = "West";
        } else if (read_lines[0].compare("w") == 0) {
            // Low priority Westbound
            train_line->priority = "Low";
            train_line->direction = "West";
        }

        // Push onto trains vector
        trains.push_back(train_line);
        read_lines.clear();

        // Increment train counter
        count++;
    }

    // Exiting for loop means all lines from input file have been read.
    // Close input file
    file.close();

    //TODO: Write custom comparator for priority queue

    // Print input
    int hours = 0;
    int minutes = 0;
    float seconds = 0.0;

    // TODO: Train Schedule Output
//    printf("%02d:%02d:%04.1f ", hours, minutes, seconds);
//    cout << "Train " << train_id << " is ready to go East";
//    cout << "Train " << train_id << " is ready to go West";
//
//    // ON Track status messages
//    cout << "Train " << train_id << " is " << train_status << " the main track going East";
//    cout << "Train " << train_id << " is " << train_status << " the main track going West";
//
//    // OFF Track status messages
//    cout << "Train " << train_id << " is " << train_status << " the main track after going East";
//    cout << "Train " << train_id << " is " << train_status << " the main track after going West";



}