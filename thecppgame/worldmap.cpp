#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <random>
#include <cstdlib>
#include "worldmap.h"
#include "robot.h"
#include <mutex>

/*
   ---------------------------------------------------------------------------------------------------------------------
   ------------------------------------- %% Constructor Definitions & Delegations %% -----------------------------------
   ---------------------------------------------------------------------------------------------------------------------
*/
World::World(std::pair<int, int> dimensions,int obstacleCount, int robotCount): Game{},
    worldMapDimensions{dimensions}, numberOfObstacles{obstacleCount}, numberOfRobots{robotCount}
{
    std::cout<<"Initializing the World!!"<<std::endl;
    // Map creation variables initialization
    // Determine the lowest dimension of the map to compute distributionRange
    distributionRange = worldMapDimensions.first < worldMapDimensions.second ? worldMapDimensions.first : worldMapDimensions.second;

    // Initialize obstacleLocations and worldMap
    createMap();
    //
}
// Constructor Delegations
World::World(): World{{5,5}, 2, 1}{}
World::World(std::pair<int, int> dimensions): World{dimensions, 2, 1}{}
World::World(std::pair<int, int> dimensions,int obstacleCount): World{dimensions, obstacleCount, 1}{}
World::~World(){};

// Private Functions
void World::createMap()
{
    /* Map Creation::
    Example of a 5 x 5 Map is as follows:

    *******
    *---X-*
    *-A---*
    *-----*
    *-X---*
    *---X-*
    *******

    Where,
    1) * Denotes boundary
    2) - Denotes empty space
    3) X denotes obstacle
    4) Capital alphabets (A, B, C,...) denote the Robots

    */

    // Initialize the map variables
    int length = worldMapDimensions.first;
    int breadth = worldMapDimensions.second;

    // Map Creation Step - 1: Initiate a 2D Vector
    // Create map empty space template
    std::vector<Arena> buildingBlock(breadth + 2);
    for(auto it = buildingBlock.begin(); it!=buildingBlock.end(); ++it)
    {
        (*it).content = '-';
        (*it).flag = false;
        (*it).previous = {0,0};
        (*it).next = {0,0};
        (*it).trail = '*';
    }
    // Add boundaries to the space
    auto iter = buildingBlock.begin();
    (*iter).content = '*';
    iter = buildingBlock.end();
    --iter;
    (*iter).content = '*';

    // Create Boundary Template
    std::vector<Arena> boundary(breadth + 2);
    for(auto it = boundary.begin(); it!=boundary.end(); ++it)
    {
        (*it).content = '*';
        (*it).flag = false;
        (*it).previous = {0,0};
        (*it).next = {0,0};
        (*it).trail = '*';
    }

    // Add the upper boundary to the map
    worldMap.push_back(boundary);

    // Add the spaces to the map
    for (int i=0; i < (length); ++i)
    {
        worldMap.push_back(buildingBlock);
    }

    // Add lower boundary to the map
    worldMap.push_back(boundary);

    // Generate obstacle location list
    generateObstacleLocations();

    // Add obstacles to the map
    addToMap(obstacleLocations,'X',true);

    // Add collectibles to the map
    numberOfCollectibles = int(numberOfObstacles * 0.15);
    collectibleLocations = generateCoordinates(distributionRange, numberOfCollectibles);
    addToMap(collectibleLocations, '+');

    // Add holes to the map
    numberOfHoles = int ((level + 1) * 0.1 * numberOfObstacles);
    holesLocations = generateCoordinates(distributionRange, numberOfHoles);
    addToMap(holesLocations, 'O');


    if (currentMode == single_auto || currentMode == single_manual)
    {
        // Mark robot destination on the map
        destinationLocation.push_back({worldMapDimensions.first, worldMapDimensions.second});
        addToMap(destinationLocation, '@');

        // Add Robot on the map
        startingLocations.push_back({1,1});
        addToMap(startingLocations, 'a');

    }
    else{
        // Add Robots to the map
        for (auto mapping : robotLocationMapping)
        {
            int robotID = mapping.first;
            std:: pair<int, int> robotLocation = mapping.second;
            (worldMap[robotLocation.first][robotLocation.second]).content = robotID;
        }
    }

}


void World:: addToMap(std::vector<std::pair<int, int>> coordinateList, char symbol, bool includeTrailMap)
{
    for (auto location: coordinateList)
    {
        //std::cout<<"("<<location.first<<", "<<location.second<<") : "<<symbol<<std::endl;
        (worldMap[location.first][location.second]).content = symbol;
        if (includeTrailMap) (worldMap[location.first][location.second]).trail = symbol;
    }

}


std::vector<std::pair<int, int>> World:: getNeighborList(std::pair<int, int> location,direction headDirection, bool clockwiseHeadRotation)
{
    /*
       Generates the list of neighbors (coordinates) for a robot agent in the world based on the direction specified
       Inputs: 1) Current location co-ordinates
               2) Direction of the head of the robot agent
               3) Priority of Head rotation of the robot agent

       Output: List of neighbor location coordinates for the robot
    */

    std::vector<std::pair<int, int>> neighbors;
    //std::cout<<location.first<<", "<<location.second<<"\n"; //optional to check the coordinates
    if (clockwiseHeadRotation)
    {
        // Add the east neighbor
        neighbors.push_back({location.first, location.second + 1});
        // Add the south east neighbor
        neighbors.push_back({location.first + 1, location.second + 1});
        // Add the south neighbor
        neighbors.push_back({location.first + 1, location.second});
        // Add the south west neighbor
        neighbors.push_back({location.first + 1, location.second - 1});
        // Add the west neighbor
        neighbors.push_back({location.first , location.second - 1});
        // Add the north west neighbor
        neighbors.push_back({location.first - 1, location.second - 1});
        // Add the north neighbor
        neighbors.push_back({location.first - 1, location.second });
        // Add the north east neighbor
        neighbors.push_back({location.first - 1, location.second + 1});

    }
    else
    {
        // Add the east neighbor
        neighbors.push_back({location.first, location.second + 1});
        // Add the north east neighbor
        neighbors.push_back({location.first - 1, location.second + 1});
        // Add the north neighbor
        neighbors.push_back({location.first - 1, location.second });
        // Add the north west neighbor
        neighbors.push_back({location.first - 1, location.second - 1});
        // Add the west neighbor
        neighbors.push_back({location.first , location.second - 1});
        // Add the south west neighbor
        neighbors.push_back({location.first + 1, location.second - 1});
        // Add the south neighbor
        neighbors.push_back({location.first + 1, location.second});
        // Add the south east neighbor
        neighbors.push_back({location.first + 1, location.second + 1});

    }
    // Rotate the neighbors list to make sure head points the first neighbor
    switch(headDirection)
    {
    case East:
        {
            return neighbors;
        }
    case SouthEast:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+1,neighbors.end());
            break;
        }
    case South:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+2,neighbors.end());
            break;
        }
    case SouthWest:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+3,neighbors.end());
            break;
        }
    case West:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+4,neighbors.end());
            break;
        }
    case NorthWest:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+5,neighbors.end());
            break;
        }
    case North:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+6,neighbors.end());
            break;
        }
    case NorthEast:
        {
            std::rotate(neighbors.begin(),neighbors.begin()+7,neighbors.end());
            break;
        }
    default:
        {
            // default East
            return neighbors;
        }
    }
    return neighbors;
}


std::vector<std::pair<int, int>> World:: getTraversibleNeighborList(std::pair<int, int> location,direction head, bool clockwiseHeadRotation)
{
    /*
       This method generates the neighbor co-ordinates that could be used to navigate by the robot.
       It first generates all possible neighbor locations according to the direction preferences
       of robot's head and head rotation and then validates each neighbor to eliminate the location
       from the identified list, in case the location belongs to an obstacle or the boundary of the world.

       Inputs: 1) current location coordinates
               2) robot agent head direction
               3) default direction preference for head rotation

       Output: List of valid neighbor coordinates for the robot to traverse
    */
    // Determine the neighbors
    std::vector<std::pair<int, int>> neighbors = getNeighborList(location, head, clockwiseHeadRotation);
    std::vector<std::pair<int, int>> filteredNeighbors;
    // Filer the neighbors
    for(auto it = neighbors.begin(); it!=neighbors.end(); ++it)
    {
        if (!(worldMap[(*it).first][(*it).second].content== '*' ||  worldMap[(*it).first][(*it).second].content== 'X'))
        {
            filteredNeighbors.push_back({(*it).first, (*it).second});
        }
    }
    /*
    std::cout<<"Filtered Neighbors\n";
    for(auto it=filteredNeighbors.begin(); it!=filteredNeighbors.end(); ++it)
    {
        std::cout<<(*it).first<<", "<<(*it).second<<std::endl; // Optional to check the coordinates read
    }
    */
    return filteredNeighbors;
}


std::vector<std::pair<int, int>> World:: generateCoordinates(int distributionRange, int numberOfCoordinates)
{
    /* Random coordinate generation code taken from stackoverflow
    Reference: https://stackoverflow.com/questions/26086128/generate-different-random-coordinates
    */
    std::vector<std::pair<int, int>> coordinates;
    // Generate random locations in the map for the obstacles
    std::mt19937 eng; // object which produces random bits
    std::random_device r;
    std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
    eng.seed(seed); // seed the bit generator, replaces srand()
    std::uniform_int_distribution<> dist(1, distributionRange); // encapsulates the correct method of turning
                                                                // random bits into random numbers in the range [0, N)
    for(int i = 0; i < numberOfCoordinates; i++)
    {
        int x = dist(eng); // the distribution internally used the engine like rand()
        int y = dist(eng);
        std::pair<int, int> v = {x, y}; // create a coordinate variable for comparison with the ones in the list
        // create function for std::pair comparison
        auto is_equal = [&v](const std::pair<int, int> &i){return (i.first == v.first) && (i.second == v.second);};
        // find if the coordinate has already been generated
        auto check = std::find_if(coordinates.begin(), coordinates.end(), is_equal);
        if (check == std::end(coordinates))
        {
            coordinates.push_back({x, y});
        }
        else{
            // if the coordinate has been generated earlier, reduce the iteration count
            --i;
        }

    }

    return  coordinates;
}




void World::generateObstacleLocations()
{
    obstacleLocations = generateCoordinates(distributionRange, numberOfObstacles);
}


bool World:: isValidMove(std::pair<int, int> nextPosition)
{
    /*
      This method validates the location for a robot to traverse in the world. This is to identify
      the boundaries of the world and the obstacles present in it ensure that robot agent is not
      directed to move on them.

      Inputs: Location co-ordinates
      Output: validation boolean
    */

    // Read the content in the next location
    char content = (worldMap[nextPosition.first][nextPosition.second]).content;
    //std::cout<<"Next content: "<<content<<std::endl;
    // If the next location is an empty space without an obstacle or a robot, return true
    if (content < 88 && content != '*') // ASCII value for 'X' is 88
    {
        // the char value  is not 'X' or boundary and not anything in lower case alphabets
        return true;
    }
    else
    {
        return false;
    }
}



bool World:: moveFromTo(std::pair<int, int> currentLocation, std::pair<int, int> nextLocation, char robotID)
{
    /* This function checks if the next location proposed for the robot agent is appropriate.
    If it is not appropriate, it returns 'X'. Otherwise, it returns the content of next location.
    Inputs:
            1. std::pair<int, int> currentLocation: current coordinate of robot in the worldmap
            2. std::pair<int, int> nextLocation: next coordinate of robot in the world map
            3. char robotID: ID of the robot (a, b, c, etc..)
    Outputs: char content: Content of the next location of the robot
    */
    // Validate the move once again.
    bool validateMove = isValidMove(nextLocation);
    char content = 'X';
    if (validateMove)
    {


        // Store the char in the next location
        content = (worldMap[nextLocation.first][nextLocation.second]).content;
        // Move the robot to the next location
        (worldMap[nextLocation.first][nextLocation.second]).content = robotID;
        // Empty the previous location from where robot has been displaced
        (worldMap[currentLocation.first][currentLocation.second]).content = '-';


        return true;

    }
    return false;
}


// Public Function definitions

/*
   ---------------------------------------------------------------------------------------------------------------------
   -----------------------------------------------------%% Getter Methods %% -------------------------------------------
   ---------------------------------------------------------------------------------------------------------------------
*/

std::pair<int, int> World:: getWorldDimensions()
{
    return worldMapDimensions;
}


std::pair<int, int> World :: getStartLocation(bool generateNewLocation)
{
    if (generateNewLocation)
    {
        char agentID = (worldMap[startingLocations[0].first][startingLocations[0].second]).content;
        //std::cout<<"Robot Agent ID :"<<agentID<<std::endl;
        addToMap(startingLocations, '-');
        // Generate 20 feasible coordinates randomly from the map
        std::vector<std::pair<int, int>> newLocation = generateCoordinates(distributionRange, distributionRange);
        // Amongst the coordinate list, pick a single coordinate randomly
        std::srand((unsigned) std::time(NULL)); // provide a seed value
        int offset = distributionRange/2; // offset changed in comparison to the one for stop location to generate distinct locations
        int range = distributionRange - 1; // same as (the number of elements - 1) in newLocation vector
        int random_index;
        do{
            random_index = offset + (std::rand() % range); // generate the index for random selection from list
            //std::cout<<"Start: "<<newLocation.size()<<", "<<random_index<<std::endl;
        }while(newLocation[random_index].first ==0 || newLocation[random_index].second ==0 || random_index >= newLocation.size());
        startingLocations[0].first = newLocation[random_index].first;
        startingLocations[0].second = newLocation[random_index].second;
        std::cout<<"Location ID Before :"<<(worldMap[startingLocations[0].first][startingLocations[0].second]).content;
        std::cout<<", Location: ("<<startingLocations[0].first<<","<<startingLocations[0].second<<")";
        addToMap(startingLocations, agentID);
        std::cout<<", Location ID After:"<<(worldMap[startingLocations[0].first][startingLocations[0].second]).content<<std::endl;

    }

    return startingLocations[0];
}


std::pair<int, int> World:: getStopLocation(bool generateNewLocation)
{
    if (generateNewLocation)
    {
        addToMap(destinationLocation, '-');
        // Generate 20 feasible coordinates randomly from the map
        std::vector<std::pair<int, int>> newLocation = generateCoordinates(distributionRange, distributionRange);
        // Amongst the coordinate list, pick a single coordinate randomly
        std::srand((unsigned) std::time(NULL)); // provide a seed value
        int offset = 0;
        int range = distributionRange - 1; // same as (the number of elements - 1) in newLocation vector
        int random_index;
        do{
            random_index = offset + (std::rand() % range); // generate the index for random selection from list
            //std::cout<<"Stop: "<<newLocation.size()<<", "<<random_index<<std::endl;
        }while(newLocation[random_index].first ==0 || newLocation[random_index].second ==0 || random_index >= newLocation.size());
        destinationLocation[0].first = newLocation[random_index].first;
        destinationLocation[0].second = newLocation[random_index].second;
        addToMap(destinationLocation, '@');
    }
    return destinationLocation[0];
}


bool World::getLocationFlag(std::pair<int, int> location)
{
    if(location.first >= 0 && location.second >=0
       && location.first < worldMapDimensions.first && location.second < worldMapDimensions.second)
    {
        return (worldMap[location.first][location.second]).flag;
    }
    return false;
}


char World::getLocationContent(std::pair<int, int> location)
{
    //std::cout<<"Next Location: ("<<location.first<<","<<location.second<<")"<<std::endl;
    if(location.first >= 0 && location.second >=0
       && location.first < worldMapDimensions.first && location.second < worldMapDimensions.second)
    {
        return (worldMap[location.first][location.second]).content;
    }
     return '*';
}


char World::getLocationTrail(std::pair<int, int> location)
{
    if(location.first >= 0 && location.second >=0
       && location.first < worldMapDimensions.first && location.second < worldMapDimensions.second)
    {
        return (worldMap[location.first][location.second]).trail;
    }
     return '*';
}


std::pair<int,int> World::getLocationPrevious(std::pair<int, int> location)
{
    if(location.first >= 0 && location.second >=0
       && location.first < worldMapDimensions.first && location.second < worldMapDimensions.second)
    {
        return (worldMap[location.first][location.second]).previous;
    }
    return std::make_pair(1,1);
}


std::pair<int,int> World::getLocationNext(std::pair<int, int> location)
{
    if(location.first >= 0 && location.second >=0
       && location.first < worldMapDimensions.first && location.second < worldMapDimensions.second)
    {
        return (worldMap[location.first][location.second]).next;
    }
    return std::make_pair(1,1);
}


/*
   ---------------------------------------------------------------------------------------------------------------------
   -----------------------------------------------------%% Setter Methods %% -------------------------------------------
   ---------------------------------------------------------------------------------------------------------------------
*/


void World::setLocationNext(std::pair<int, int> location,std::pair<int, int> information)
{
    (worldMap[location.first][location.second]).next.first = information.first;
    (worldMap[location.first][location.second]).next.second = information.second;
}


void World::setLocationPrevious(std::pair<int, int> location,std::pair<int, int> information)
{
    (worldMap[location.first][location.second]).previous.first = information.first;
    (worldMap[location.first][location.second]).previous.second = information.second;
}


void World::setLocationContent(std::pair<int, int> location,char information)
{
    (worldMap[location.first][location.second]).content = information;
}


void World::setLocationTrail(std::pair<int, int> location,char information)
{
    (worldMap[location.first][location.second]).trail = information;
}


void World::setLocationFlag(std::pair<int, int> location,bool information)
{
    (worldMap[location.first][location.second]).flag = information;
}


void World::setObstacleCount(int obstacleCount)
{
    numberOfObstacles = obstacleCount;
}


void World::setCollectibleCount(int collectibleCount)
{
    numberOfCollectibles = collectibleCount;
}


void World::setHoles(int holesCount)
{
    numberOfHoles = holesCount;
}


void World::resetCollectibleLocations()
{
    // Clear obstacle Locations
    addToMap(collectibleLocations, '-');
    // Add collectibles to the map
    numberOfCollectibles = int(numberOfObstacles * 0.15);
    // Generate new set of collectibles as per the updated count
    collectibleLocations = generateCoordinates(distributionRange, numberOfCollectibles);
    // Add the generated collectibles to the map
    addToMap(collectibleLocations, '+');
    // Make sure that holes don't replace start and end points
    addToMap(destinationLocation, '@');
    addToMap(startingLocations, 'a');
}


void World::resetHoleLocations()
{
    // Clear hOLE Locations
    addToMap(holesLocations, '-');
    // update number of holes on the map
    numberOfHoles = int ((level + 1) * 0.1 * numberOfObstacles);
    // Generate new set of hole locations
    holesLocations = generateCoordinates(distributionRange, numberOfHoles);
    // Add generated holes to the map
    addToMap(holesLocations, 'O');
    // Make sure that holes don't replace start and end points
    addToMap(destinationLocation, '@');
    addToMap(startingLocations, 'a');
}


void World::resetObstacleLocations()
{
    // Clear obstacle Locations
    addToMap(obstacleLocations, '-',true);
    // Generate new set of obstacles as per the updated obstacle count
    generateObstacleLocations();
    // Add the generated obstacles to the map
    addToMap(obstacleLocations, 'X',true);
    // Make sure that holes don't replace start and end points
    addToMap(destinationLocation, '@');
    addToMap(startingLocations, 'a');
}


void World::show(int fieldID)
{
    /*This function displays the wolrd contents as per the filedID
      Inputs: int filedID : The id of the filed to be display in each location of the world
      Outputs: None
    */
    // Print

    for (auto &row : worldMap)
    {
        for(auto &col : row)
        {
            switch(fieldID)
            {
                case 0:
                    {
                        std::cout<<" "<<col.content<<" ";
                        break;
                    }
                case 1:
                    {
                        std::cout<<" "<<col.flag<<" ";
                        break;
                    }
                case 2:
                    {
                        std::cout<<"("<<col.previous.first<<", "<<col.previous.second<<")";
                        break;
                    }
                case 3:
                    {
                        std::cout<<"("<<col.next.first<<", "<<col.next.second<<")";
                        break;
                    }
                case 4:
                    {
                        std::cout<<" "<<col.trail<<" ";
                        break;
                    }
            }

        }
        std::cout<<"\n";
    }
    std::cout<<"\n";
}


void World::dissolveRobot(std::pair<int, int>robotLocation)
{
    /*
      This function removes the Robot ID from the worldmap and assigns a gap to the robot's location
    */
    (worldMap[robotLocation.first][robotLocation.second]).content = '-';
}
