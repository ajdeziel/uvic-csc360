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
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// Build comparator according to:
// https://stackoverflow.com/questions/15601973/stl-priority-queue-of-c-with-struct
struct Train {
    int train_id;
    int priority;
    int direction;
    int last_train_direction;
    int load_time;
    int cross_time;

    // Only doing based on priority and arrival in file by logic.
    // Still must account for direction, and probably should specify arrival using train_id.
    // Follow Assignment 2 spec, Section 2.2 to construct comparator.
    bool operator<(const Train& rhs) const {
        if (priority > rhs.priority) {
            return priority > rhs.priority;
        } else if (priority < rhs.priority) {
            return priority < rhs.priority;
        } else {
            // If trains have same priority
            if (direction == rhs.direction) {
                if (load_time < rhs.load_time) {
                    return load_time < rhs.load_time;
                } else if (load_time == rhs.load_time) {
                    return train_id < rhs.train_id;
                } else {
                    return load_time > rhs.load_time;
                }
            } else {
                // Pick train that is opposite of last train to cross track
                if (direction == last_train_direction) {
                    return direction > rhs.direction;
                } else {
                    return direction < rhs.direction;
                }
            }
        }
    }
};

enum TrainPriority {
    HIGH = 0,
    LOW = 1,
};

enum TrainDirection {
    EAST = 0,
    WEST = 1,
};


void* start_routine(void* args) {
    Train *train_line = (Train*) args;
    usleep((train_line->load_time) * 100000);
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {

    // Verify arguments passed
    if (argc <= 1) {
        throw std::invalid_argument("No train schedule file has been provided. Please provide a train schedule file before continuing.");
    } else if (argc > 2) {
        throw std::invalid_argument("Too many train schedule files have been provided. Please only provide a single train schedule file before continuing.");
    }

    // Initialization of necessary structures
    std::ifstream file;
    std::priority_queue<Train> train_east_queue;
    std::priority_queue<Train> train_west_queue;
    std::vector<Train> trains;

    // Retrieve train priority states
    TrainPriority tp_high = HIGH;
    TrainPriority tp_low = LOW;
    
    // Retrieve train direction states
    TrainDirection td_east = EAST;
    TrainDirection td_west = WEST;

    // Open train schedule file
    file.open(argv[1], std::ifstream::in);
    cout << "Opening " << argv[1] << "..." << endl;

    int total_train_count = 0;

    for (string line; getline(file, line); ) {
        std::vector<string> read_lines;
        trains.push_back(Train());

        // Tokenize input into vector of strings.
        // Originally designed for Assignment 1
        // Inspired from https://www.geeksforgeeks.org/tokenizing-a-string-cpp
        stringstream inputstream(line);
        string input;

        while(getline(inputstream, input, ' ')) {
            read_lines.push_back(input);
        }

        // Push new train onto trains vector
        trains[total_train_count].train_id = total_train_count;
        trains[total_train_count].load_time = atoi(read_lines[1].c_str());
        trains[total_train_count].cross_time = atoi(read_lines[2].c_str());

        // Verify train priority & direction
        // High priority is defined by 0, low priority by 1.
        if (read_lines[0].compare("E") == 0) {
            // High priority Eastbound
            trains[total_train_count].priority = tp_high;
            trains[total_train_count].direction = td_east;
        } else if (read_lines[0].compare("e") == 0) {
            // Low priority Eastbound
            trains[total_train_count].priority = tp_low;
            trains[total_train_count].direction = td_east;
        } else if (read_lines[0].compare("W") == 0) {
            // High priority Westbound
            trains[total_train_count].priority = tp_high;
            trains[total_train_count].direction = td_west;
        } else if (read_lines[0].compare("w") == 0) {
            // Low priority Westbound
            trains[total_train_count].priority = tp_low;
            trains[total_train_count].direction = td_west;
        }

        read_lines.clear();

        // Increment train counter
        total_train_count++;

        // If end of file is reached, break.
        if(file.eof()) {
            break;
        }
    }

    // Exiting for loop means all lines from input file have been read.
    // Close input file.
    file.close();
    cout << "All trains read from " << argv[1] << "." << endl;

    // Create mutexes to control access to priority queues
    pthread_mutex_t eastbound_mutex;
    pthread_mutex_t westbound_mutex;
    pthread_mutex_init(&eastbound_mutex, NULL);
    pthread_mutex_init(&westbound_mutex, NULL);

    //Create threads for all train threads
    for (unsigned int i = 0; i < trains.size(); i++) {      
        // Create pthread instance
        pthread_t train_pro;

        // Create and join pthreads for each train
        pthread_create(&train_pro, NULL, &start_routine, (void*)&trains.at(i));

        // When done loading, push to respective priority queue
        if (trains[i].direction == td_east) {
            if (train_east_queue.empty()) {
                train_east_queue.push(trains[i]);
            } else {
                pthread_mutex_lock(&eastbound_mutex);
            }
        } else {
            train_west_queue.push(trains[i]);
        }           
    }

    // THIS WORKS!! No need to test anymore.
    // Sample test of train's queue
    // Train sample1 = train_queue.top();
    // cout << sample1.train_id << "," << sample1.priority << "," << sample1.direction << "," << sample1.load_time << "," << sample1.cross_time << endl;
    // train_queue.pop();

    // Train sample2 = train_queue.top();
    // cout << sample2.train_id << "," << sample2.priority << "," << sample2.direction << "," << sample2.load_time << "," << sample2.cross_time << endl;
    // train_queue.pop();

    // Train sample3 = train_queue.top();
    // cout << sample3.train_id << "," << sample3.priority << "," << sample3.direction << "," << sample3.load_time << "," << sample3.cross_time << endl;
    // train_queue.pop();


    // A joke line for me to keep track of total train count during debugging.
    // It stopped me from going crazy while figuring out more difficult code sections.
    cout << "DERP ME HAZ " << total_train_count << " TRAINZ. HJWEHGHUQWWQWQOIUOIDHADHJAJKkjas." << endl;



    // Print input
    // int hours = 0;
    // int minutes = 0;
    // float seconds = 0.0;

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
    
    return EXIT_SUCCESS;
}