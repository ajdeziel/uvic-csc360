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
#include <time.h>
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

#define BILLION 1000000000L

using namespace std;

struct Train {
    // Struct representing each train in input file
    int train_id;
    int priority;
    int direction;
    int last_train_direction;
    int load_time;
    int cross_time;

    // Comparator to order trains in station priority queue
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
            }
        }
    }
};

struct TrainArgs {
    Train train;
    timespec start;
    timespec stop;
    double accum;
    int total_train_count;
};

enum TrainPriority {
    HIGH = 0,
    LOW = 1,
};

enum TrainDirection {
    EAST = 0,
    WEST = 1,
};

// Retrieve train priority states
TrainPriority tp_high = HIGH;
TrainPriority tp_low = LOW;

// Retrieve train direction states
TrainDirection td_east = EAST;
TrainDirection td_west = WEST;

// Create mutexes to control access to priority queues
pthread_mutex_t eastbound_mutex;
pthread_mutex_t westbound_mutex;

// Create mutex to control access to track
pthread_cond_t track_busy;
pthread_mutex_t track_control;

// Train Station Queues
std::priority_queue<Train> train_east_queue;
std::priority_queue<Train> train_west_queue;

void* train_routine(void* args) {

    TrainArgs *train = (TrainArgs*) args;
    Train train_line = train->train;
    timespec start = train->start;
    timespec stop = train->stop;
    double accum = train->accum;
    int total_train_count = train->total_train_count;

    usleep((train_line.load_time) * 100000);    

    // When done loading, push to respective priority queue
    if (train_line.direction == td_east) {
        pthread_mutex_lock(&eastbound_mutex);
        train_east_queue.push(train_line);
        pthread_mutex_unlock(&eastbound_mutex);
        
        // Train is loaded on East and ready to go
        accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
        printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
        cout << "Train " << train_line.train_id << " is ready to go East";        
    } else {
        pthread_mutex_lock(&westbound_mutex);
        train_west_queue.push(train_line);
        pthread_mutex_unlock(&westbound_mutex);

        // Train is loaded on West and ready to go
        accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
        printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
        cout << "Train " << train_line.train_id << " is ready to go West";
    }

    int num_crossed = 0;

    while (num_crossed < total_train_count) {
        string last_direction_allowed;

        if (pthread_mutex_trylock(&track_control) == 0) {
            pthread_mutex_lock(&track_control);
            Train train_top_west = train_west_queue.top();
            Train train_top_east = train_east_queue.top();
            
            // Verify which train gets to cross track
            if (train_top_east.priority > train_top_west.priority) {
                // Eastbound train has higher priority than Westbound train

                pthread_cond_signal(&track_busy);
                
                // Train ON track status
                accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
                printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
                cout << "Train " << train_top_east.train_id << " is ON the main track going East";
                
                usleep(train_top_east.cross_time * 100000);
                
                // Train OFF track status
                accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
                printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
                cout << "Train " << train_top_east.train_id << " is OFF the main track going East";

                train_east_queue.pop();
                last_direction_allowed = "East";
            } else if (train_top_east.priority < train_top_west.priority) {
                // Eastbound train has less priority than Westbound train

                pthread_cond_signal(&track_busy);
                
                // Train ON track status
                accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
                printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
                cout << "Train " << train_top_west.train_id << " is ON the main track going West";

                usleep(train_top_west.cross_time * 100000);
                
                // Train OFF track status
                accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
                printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
                cout << "Train " << train_top_west.train_id << " is OFF the main track going West";

                train_west_queue.pop();
                last_direction_allowed = "West";
            } else {
                // Eastbound train and Westbound train have same priority

                // Which direction did the last train cross in?
                if (last_direction_allowed.compare("East")) {
                    // If East, let Westbound train go.
                    
                    pthread_cond_signal(&track_busy);
                    
                    // Train ON track status
                    accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
                    printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
                    cout << "Train " << train_top_west.train_id << " is ON the main track going West";

                    usleep(train_top_west.cross_time * 100000);
                    
                    // Train OFF track status
                    accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
                    printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
                    cout << "Train " << train_top_west.train_id << " is OFF the main track going West";

                    train_west_queue.pop();
                    last_direction_allowed = "West";
                } else if (last_direction_allowed.compare("West")) {
                    // If West, let Eastbound train go.
                    
                    pthread_cond_signal(&track_busy);
                
                    // Train ON track status
                    accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
                    printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
                    cout << "Train " << train_top_east.train_id << " is ON the main track going East";
                    
                    usleep(train_top_east.cross_time * 100000);
                    
                    // Train OFF track status
                    accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
                    printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
                    cout << "Train " << train_top_east.train_id << " is OFF the main track going East";

                    train_east_queue.pop();
                    last_direction_allowed = "East";
                } else {
                    // If no train has gone, let Eastbound go.
                    
                    pthread_cond_signal(&track_busy);
                
                    // Train ON track status
                    accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
                    printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
                    cout << "Train " << train_top_east.train_id << " is ON the main track going East";
                    
                    usleep(train_top_east.cross_time * 100000);
                    
                    // Train OFF track status
                    accum = (stop.tv_sec - start.tv_sec) + ((stop.tv_nsec - start.tv_nsec) / BILLION);
                    printf("%02d:%02d:%04.1f ", int((accum/60)/60), int(accum/60), accum);
                    cout << "Train " << train_top_east.train_id << " is OFF the main track going East";

                    train_east_queue.pop();
                    last_direction_allowed = "East";
                }
            }
            
            pthread_mutex_unlock(&track_control);
        }        

        num_crossed++;
    }

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
    std::vector<Train> trains;

    // Open train schedule file
    file.open(argv[1], std::ifstream::in);
    cout << "Opening " << argv[1] << "..." << endl;

    int total_train_count = 0;

    for (string line; getline(file, line); ) {
        std::vector<string> read_lines;
        trains.push_back(Train());

        // Tokenize input into vector of strings.
        // Originally designed for CSC 360, Assignment 1 (Simple Shell Interpreter)
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

    // Create train station mutexes
    pthread_mutex_init(&eastbound_mutex, NULL);
    pthread_mutex_init(&westbound_mutex, NULL);

    // Create track control mutex
    pthread_mutex_init(&track_control, NULL);

    //Set up timer
    struct timespec start, stop;
    double accum = 0.0;

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock gettime");
        exit(EXIT_FAILURE);
    }

    if (clock_gettime(CLOCK_REALTIME, &stop) == -1) {
        perror("clock gettime");
        exit(EXIT_FAILURE);
    }

    //Create threads for all train threads
    for (unsigned int i = 0; i < trains.size(); i++) {      
        // Create pthread instance
        pthread_t train_pro;

        TrainArgs args;
        args.train = trains.at(i);
        args.start = start;
        args.stop = stop;
        args.accum = accum;
        args.total_train_count = total_train_count;

        // Create and join pthreads for each train
        pthread_create(&train_pro, NULL, &train_routine, (void*)&args);
    }

    //Destroy station queue mutexes
    pthread_mutex_destroy(&eastbound_mutex);
    pthread_mutex_destroy(&westbound_mutex);

    // Destroy track control mutex
    pthread_cond_destroy(&track_busy);
    pthread_mutex_destroy(&track_control);
    
    return EXIT_SUCCESS;
}