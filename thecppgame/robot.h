#ifndef ROBOT_H_
#define ROBOT_H_
#include "game.h"

class World; //Forward Declaration
class Robot: protected Game {
 private:

    std::pair<int, int> startLocation; //default {1,1}
    std::pair<int, int> stopLocation; // init by map dimension
    direction head; //default North
    bool movesClockwise; //default true
    mode operationMode; //default single_auto
    bool isStuck; // default false
    std::pair<int, int> currentLocation; // default {1,1}
    std::vector<std::pair<int, int>> trail;
    static int numberOfRobots; // default - 1
    float robotTimeUnit; // default 250
    std::chrono::time_point< std::chrono::high_resolution_clock> startTime;
    // Methods
    std::pair<int, int> autoNextMove();

 public:
    // Constructors
    Robot();
    Robot(std::pair<int, int> startPosition, std::pair<int, int> stopPosition, direction head, bool motionDirection, mode opMode, float robotTimeUnit);
    // Destructor
    ~Robot();
    // Helper Functions

    //friend bool addRobot(std::pair<int, int> startPosition, direction head, bool motion, bool mode, bool status);
    void run(); //Works on a robot thread instance
    bool nextMove(); // works both for auto and manual mode
    void setStartingLocation(std::pair<int, int> coordinate); // used in manual mode
    bool flipDirection(); // used in manual & auto modes
    std::vector<std::pair<int, int>> shortestPathBFS(World& wmap);
    float getRobotTimeUnit();
    void setRobotTimeUnit(float robotTime);
    //friend bool World::isValidMove(std::pair<int, int> nextPosition);

};

#endif
