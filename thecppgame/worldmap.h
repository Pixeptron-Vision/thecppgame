#ifndef WORLD_H_
#define WORLD_H_
#include <vector>
#include <utility>
#include "game.h"

struct Arena{
    char content;
    bool flag;
    std::pair<int, int> previous;
    std::pair<int, int> next;
    char trail;
};

typedef Arena Arena;

class World : protected Game{
 private:
    // Map creation variables
    std::pair<int, int> worldMapDimensions;
    int numberOfObstacles;
    int numberOfRobots;
    int numberOfCollectibles;
    int numberOfHoles;
    int distributionRange;
    std::vector<std::vector<Arena>> worldMap;
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
    void addToMap(std::vector<std::pair<int, int>> coordinateList, char symbol, bool includeTrailMap=false);
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
    void show(int fieldID=0);
    bool addRobot(std::pair<int, int> startPosition, direction head, bool motion, bool mode, bool status);
    bool moveFromTo(std::pair<int, int> currentLocation, std::pair<int, int> nextLocation, char robotID);

    // Getters
    std::pair<int , int> getRobotLocation(int robotID, int steps);
    std::pair<int, int> getWorldDimensions();
    bool getLocationFlag(std::pair<int, int> location);
    char getLocationContent(std::pair<int, int> location);
    char getLocationTrail(std::pair<int, int> location);
    std::pair<int,int> getLocationPrevious(std::pair<int, int> location);
    std::pair<int,int> getLocationNext(std::pair<int, int> location);
    std::vector<std::pair<int, int>> getTraversibleNeighborList(std::pair<int, int> location, direction headDirection=East, bool clockwiseHeadRotation= true);
    std::vector<std::pair<int, int>> getNeighborList(std::pair<int, int> location, direction headDirection=East, bool clockwiseHeadRotation= true);
    std::pair<int, int> getStartLocation(bool generateNewLocation=false);
    std::pair<int, int> getStopLocation(bool generateNewLocation=false);
    // Setters
    void setMode();
    void setObstacleCount(int obstacleCount);
    void resetObstacleLocations();
    void setCollectibleCount(int collectibleCount);
    void resetCollectibleLocations();
    void setHoles(int holeCount);
    void resetHoleLocations();
    void setLocationNext(std::pair<int, int> location,std::pair<int, int> information);
    void setLocationPrevious(std::pair<int, int> location,std::pair<int, int> information);
    void setLocationContent(std::pair<int, int> location,char information);
    void setLocationFlag(std::pair<int, int> location,bool information);
    void setLocationTrail(std::pair<int, int> location,char information);


};


#endif
