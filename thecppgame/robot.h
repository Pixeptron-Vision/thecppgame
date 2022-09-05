#ifndef ROBOT_H_
#define ROBOT_H_
enum direction{East, NorthEast, North, NorthWest, West, SouthWest, South, SouthEast};
#include "worldmap.h"
#include "game.h"
// Directions Reference:
//
//      (135°)     (90°)    (45°)
//      NORTH-WEST NORTH  NORTH-EAST
//               \   |   /
//                \  |  /
//                 \ | /
//                  \|/
//    (180°)WEST --------- EAST (0°)
//                  /|\
//                 / | \
//                /  |  \
//               /   |   \
//     SOUTH-WEST  SOUTH  SOUTH-EAST
//      (225°)     (270°)   (315°)
// Define and implement the following class
class Robot: protected Game {
 private:
    std::pair<int, int> location;
    std::vector<std::pair<int, int>> trail;
    std::pair<int, int> autoNextMove();
    direction head; //default 0
    bool autoMode; //default true
    bool isStuck; // default false
    bool movesClockwise; //default true
 public:
    Robot();
    Robot(std::pair<int, int> startPosition, direction head, bool motion, bool mode, bool status);
    ~Robot();
    friend bool addRobot(std::pair<int, int> startPosition, direction head, bool motion, bool mode, bool status);
    // Helper Functions
    void run(); //Creates a multithreading instance
    bool nextMove(); // works both for auto and manual mode
    void setStartingLocation(std::pair<int, int> coordinate); // used in manual mode
    bool flipDirection(); // used in manual & auto modes
    //friend bool World::isValidMove(std::pair<int, int> nextPosition);


};

#endif
