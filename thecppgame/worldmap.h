#ifndef WORLD_H_
#define WORLD_H_
#include <vector>
#include <utility>
#include "game.h"
#include "robot.h"


class World : protected Game{
 private:
    // Map creation variables
    std::pair<int, int> worldMapDimensions;
    int numberOfObstacles;
    int numberOfRobots;
    std::vector<std::vector<char>> worldMap;
    std::vector<std::pair<int, int>> obstacleLocations;
    std::vector<std::pair<int, std::pair<int, int>>> robotLocationMapping;

    // Robot Movement variables
    std::vector<bool> robotMapAccess;


    // Map creation supporting Methods
    void createMap();
    void generateObstacleLocations();
    std::vector<std::pair<int, int>> generateCoordinates(int distributionRange, int numberOfCoordinates);
    // Robot Movement Methods
    bool isValidMove(std::pair<int, int> nextPosition);


 public:

    // Constructors
    World();
    World(std::pair<int, int> dimensions);
    World(std::pair<int, int> dimensions,int obstacleCount);
    World(std::pair<int, int> dimensions,int obstacleCount, int robotCount);
    World(std::pair<int, int> dimensions,int obstacleCount, int robotCount, mode gameMode);

    // Destructor
    ~World();

    // Helper Functions
    void show();
    void setMode();
    bool addRobot(std::pair<int, int> startPosition, direction head, bool motion, bool mode, bool status);
    bool moveRobot(std::pair<int, int> currentPosition,direction way );
    std::pair<int , int> getRobotLocation(int robotID, int steps);
    void generateMap();

};


#endif
