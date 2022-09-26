#ifndef ROBOT_H_
#define ROBOT_H_
#include "game.h"

class World; //Forward Declaration
class Robot: protected Game {
 private:
    // Variables
    std::pair<int, int> startLocation; //default {1,1}
    std::pair<int, int> stopLocation; // init by map dimension
    direction head; //default North
    float robotTimeUnit; // default 250
    bool movesClockwise; //default true
    mode operationMode; //default single_auto
    bool isStuck; // default false
    std::pair<int, int> currentLocation; // default {1,1}
    std::vector<std::pair<int, int>> trail;
    static int numberOfRobots; // default - 1
    std::chrono::time_point< std::chrono::high_resolution_clock> startTime;

 public:
    // Constructors
    Robot();
    Robot(std::pair<int, int> startPosition, std::pair<int, int> stopPosition, direction head, bool motionDirection, mode opMode, float robotTimeUnit);
    // Destructor
    ~Robot();
    int life;
    char ID;

    // Helper Functions
    std::pair<int, int> determineNextStep(); // used in multiple auto mode
    bool validateLocation(char content);
    void changeHeadDirection();
    void changeHeadRotation(); // used in manual & auto modes
    std::vector<std::pair<int, int>> shortestPathBFS(World& wmap); // used in single auto mode

    // Getter Functions
    std::pair<int, int> getCurrentLocation(); // used in manual mode
    float getRobotTimeUnit();
    direction getHeadDirection();

    // Setter Functions
    void setStartingLocation(std::pair<int, int> coordinate); // used in manual mode
    void setCurrentLocation(std::pair<int, int> coordinate); // used in manual mode
    void setRobotTimeUnit(float robotTime);

};

#endif
