#ifndef WORLD_H_
#define WORLD_H_
#include <vector>
#include <utility>
#include "game.h"

class World : protected Game{
 private:
    // Map creation variables
    std::pair<int, int> worldMapDimensions;
    int numberOfObstacles;
    int numberOfRobots;
    int numberOfCollectibles;
    int numberOfHoles;
    int distributionRange;
    std::vector<std::vector<char>> worldMap;
    std::vector<std::pair<int, int>> obstacleLocations;
    std::vector<std::pair<int, int>> collectibleLocations;
    std::vector<std::pair<int, int>> holesLocations;
    std::vector<std::pair<int, int>> destinationLocation;
    std::vector<std::pair<int, int>> startingLocations;
    std::vector<std::pair<int, std::pair<int, int>>> robotLocationMapping;

    // Robot Movement variables
    std::vector<bool> robotMapAccess;


    // Map creation supporting Methods
    void createMap();
    void generateObstacleLocations();
    std::vector<std::pair<int, int>> generateCoordinates(int distributionRange, int numberOfCoordinates);
    void addToMap(std::vector<std::pair<int, int>> coordinateList, char symbol);
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
    bool moveFromTo(std::pair<int, int> currentLocation, std::pair<int, int> nextLocation, char robotID);
    std::pair<int , int> getRobotLocation(int robotID, int steps);
    std::pair<int, int> getWorldDimensions();
    char getLocationInformation(std::pair<int, int> location);
    std::vector<std::pair<int, int>> getTraversibleNeighborList(std::pair<int, int> location, bool clockwiseHeadRotation= true);
    std::vector<std::pair<int, int>> getNeighborList(std::pair<int, int> location, bool clockwiseHeadRotation= true);
    std::pair<int, int> getStartLocation();
    std::pair<int, int> getStopLocation();

};


#endif
