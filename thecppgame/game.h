#ifndef GAME_H_
#define GAME_H_
#include <vector>
#include <chrono>
enum mode{single_manual, single_auto , multiple_auto};
enum direction{East, NorthEast, North, NorthWest, West, SouthWest, South, SouthEast, NoWhere};
// Directions Reference:
//
//      (135?)     (90?)    (45?)
//      NORTH-WEST NORTH  NORTH-EAST
//               \   |   /
//                \  |  /
//                 \ | /
//                  \|/
//    (180?)WEST --------- EAST (0?)
//                  /|\
//                 / | \
//                /  |  \
//               /   |   \
//     SOUTH-WEST  SOUTH  SOUTH-EAST
//      (225?)     (270?)   (315?)
// Define and implement the following class
class World; //Forward Declaration
class Robot; //Forward Declaration
class Game{
 private:

 protected:
    mode currentMode;
    int activeRobots; // Not used yet
    bool startGame; // Not used yet
    bool quitGame; // Not used yet
    bool pauseGame; // Not used yet
    bool resumeGame; // Not used Yet
    bool gameEnd; // Used as flag to determine game termination
    std::chrono::time_point<std::chrono::high_resolution_clock> gameStartTime;
    std::chrono::milliseconds gameTimeLimit;
    std::chrono::milliseconds gameTimeDuration;
 public:
    int level;
    bool openThreads;
    Game();
    Game(mode gameMode); // Constructor to be used by World
    virtual ~Game();

    bool run(); // function to start the game
    std::chrono::milliseconds determineGameTime(std::pair<int, int> dimensions, float robotTimeUnit=250, float agilityFactor=1.1);
    bool spawnRobots(World &wmap, int numberOfRobots=1,direction head = North, float robotTimeUnit=250 );
    void runRobotAuto(Robot agent, World wmap);
    void runRobotManual(Robot agent, World wmap);
    void runRobotsMultiple(Robot agent, World& wmap);
    bool timeOut();
    bool stop(); // not in use yet
    bool pause(); // not in use yet
    bool resume(); // not in use yet
    void setMode(mode gameMode); // not in use yet
    bool playSingleAuto(World & wmap);
    bool playSingleManual(World & wmap); // not in use yet
    bool playMultipleAuto(World &wmap); // not in use yet


};

#endif
