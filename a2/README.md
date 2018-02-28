# CSC 360 - Assignment 2
### Multi-Threading with Train Scheduling

##### Authored by AJ Po-Deziel
##### Created on 2018-02-28

## Functionality
Two stations with a single track to allow crossing between them requires
scheduling all the various trains attempting to access it. However, each train
can be travelling in a different direction and may possess a different level
of priority over others.

This program is an exercise in multi-threading.

## Dependencies
* C++11 or higher

## Deployment Instructions
1. Ensure all dependencies are prior to starting.

2. Open Terminal (macOS/Linux/*nix systems) or PowerShell/Command Prompt (Windows).

3. Navigate to directory containing submission folder. Once there, unzip/extract
   all files from compressed folder.

4. Enter directory containing extracted items.

5. To compile the program and create an executable file to run the program,
   use the following command:
   ```
   make
   ```

6. To run the program, use the following command:
   ```
   ./mts [train_schedule]
   ```
   Where **[train_schedule]** is a .txt file containing a list of trains.
   Each train is listed line-by-line with the format below:
   > direction with priority, loading time (in deciseconds), crossing time (in deciseconds)

   An example of a line representing a train in the schedule file would look like such:
   > e 10 5

   or

   > W 7 10

   Train direction is indicated by either **e**ast or **w**est, and priority
   by either a capital letter (**high priority**) or a lowercase letter (**low priority**)*.

   ##### Low priority direction symbols
   > e/w

   ##### High priority direction symbols
   > E/W

   **PLEASE NOTE**: Failure to utilize this format will cause **_unexpected behaviour_**
   in the program's output.

7. The program will automatically terminate and exit successfully upon completion.

