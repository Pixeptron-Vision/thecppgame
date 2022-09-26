#include <iostream>
#include <deque>
#include <vector>
#include <random>
#include <chrono>
#include "worldmap.h"
#include "robot.h"

/*
   ---------------------------------------------------------------------------------------------------------------------
   -------------------------------------------------%% Constructor Definitions %% --------------------------------------
   ---------------------------------------------------------------------------------------------------------------------
*/

Robot:: Robot(std::pair<int, int> startPosition, std::pair<int, int> stopPosition, direction head, bool motionDirection, mode opMode, float robotTimeUnit): Game{},
    startLocation{startPosition}, stopLocation{stopPosition}, head{head}, robotTimeUnit{robotTimeUnit}, movesClockwise{motionDirection}, operationMode{opMode},
    isStuck{isStuck}, currentLocation{startPosition}
{
    // Init the start time and set the key variables
    startTime = std::chrono::high_resolution_clock::now();
    life = 3;
    ID = 'a';
}


Robot:: ~Robot(){}

// Initialize the static variable
int Robot:: numberOfRobots = 0;

/*
   ---------------------------------------------------------------------------------------------------------------------
   -----------------------------------------------------%% Getter Methods %% -------------------------------------------
   ---------------------------------------------------------------------------------------------------------------------
*/

float Robot:: getRobotTimeUnit()
{
    return robotTimeUnit;
}


direction Robot::getHeadDirection()
{
    return head;
}


std::pair<int, int> Robot:: getCurrentLocation()
{
    return currentLocation;
}


/*
   ---------------------------------------------------------------------------------------------------------------------
   -----------------------------------------------------%% Setter Methods %% -------------------------------------------
   ---------------------------------------------------------------------------------------------------------------------
*/


void Robot:: setRobotTimeUnit(float robotTime)
{
    robotTimeUnit = robotTime;
}


void Robot:: setStartingLocation(std::pair<int, int> coordinate)
{
    startLocation.first = coordinate.first;
    startLocation.second = coordinate.second;
}


void Robot::setCurrentLocation(std::pair<int, int> coordinate)
{
    currentLocation.first = coordinate.first;
    currentLocation.second = coordinate.second;
}


std::vector<std::pair<int, int>> Robot:: shortestPathBFS(World& wmap)
{

    // Declare the Queue, vector for storing path and array to store flags
    std::deque<std::pair<int, int>> Q;

    // Init the node flag of the environment
    std::pair<int, int> dimensions = wmap.getWorldDimensions();
    std::vector<bool> temp(dimensions.second + 2, false);
    std::vector<std::vector<bool>> visitedNodeFlags(dimensions.first + 2,temp );  // has to be initialized

    // Set the starting location as visited
    //visitedNodeFlags[startLocation.first][startLocation.second] = true;
    wmap.setLocationFlag(startLocation, true);

    // Initialize the Queue and add starting node to it
    Q.push_back(startLocation);

    // Start the search for path
    while(Q.size() > 0)
    {
        std::pair<int, int> node = Q[0];
        //std::cout<<"Captured the first node off the queue: ("<< node.first<<", "<<node.second<<")\n";
        if (node.first == stopLocation.first && node.second == stopLocation.second)
        {
            // If you come across the destination location, stop search
            std::cout<<"Reached the Destination\n";
            break;
        }

        // get the neighbor list for the node
        std::vector<std::pair<int, int>> neighbors = wmap.getTraversibleNeighborList(node, head);

        for(auto it = neighbors.begin(); it!=neighbors.end(); ++it)
        {
            //std::cout<<"Coordinate processed: "<<visitedNodeFlags[(*it).first][(*it).second]<<" : "<<(*it).first<<", "<<(*it).second<<std::endl; // Optional to check the coordinates read
            if(wmap.getLocationFlag(*it)== false)  // visitedNodeFlags[(*it).first][(*it).second]== false
            {
                // If the node neighbor is not visited, add it to the queue
                //std::cout<<"Added Neighbor: "<<(*it).first<<", "<<(*it).second<<std::endl; // Optional to check the coordinates read
                Q.push_back(*it);
                //visitedNodeFlags[(*it).first][(*it).second]= true;
                wmap.setLocationFlag(*it, true);
                wmap.setLocationPrevious(*it, node);
                wmap.setLocationNext(node, *it);

            }

        }
        Q.pop_front();

    }

    std::vector<std::pair<int, int>> shortestPath;
    std::pair<int,int> current = stopLocation;
    //std::cout<<"Start Location: "<<startLocation.first<<" "<<startLocation.second<<"\n";
    shortestPath.push_back(current);
    while(current.first != startLocation.first || current.second !=startLocation.second)
    {
        char content = wmap.getLocationContent(current);
        if (content == 'O')
        {
            life--;
        }
        else if(content == '+')
        {
            life++;
        }
        current = wmap.getLocationPrevious(current);

        shortestPath.push_back(current);
        //std::cout<<"("<<current.first<<", "<<current.second<<") ";
    }
    return shortestPath;


}


std::pair<int, int> Robot:: determineNextStep()
{
    /* This function determines the next step for the robot to traverse
       Input: None
       Output: std::pair<int, int> nextLocation : Coordinate of the next location in the specified direction

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
    */

    // Initialize the next location
    std::pair<int, int> nextLocation = std::make_pair(1,1);

    // Set the next location based on robot direction
    switch(head)
    {
    case East:
        {
            nextLocation.first = currentLocation.first;
            nextLocation.second = currentLocation.second + 1;
            break;
        }
    case NorthEast:
        {
            nextLocation.first = currentLocation.first - 1;
            nextLocation.second = currentLocation.second + 1;
            break;
        }
    case North:
        {
            nextLocation.first = currentLocation.first - 1;
            nextLocation.second = currentLocation.second;
            break;
        }
    case NorthWest:
        {
            nextLocation.first = currentLocation.first - 1;
            nextLocation.second = currentLocation.second - 1;
            break;
        }
    case West:
        {
            nextLocation.first = currentLocation.first;
            nextLocation.second = currentLocation.second - 1;
            break;
        }
    case SouthWest:
        {
            nextLocation.first = currentLocation.first + 1;
            nextLocation.second = currentLocation.second - 1;
            break;
        }
    case South:
        {
            nextLocation.first = currentLocation.first + 1;
            nextLocation.second = currentLocation.second;
            break;
        }
    case SouthEast:
        {
            nextLocation.first = currentLocation.first + 1;
            nextLocation.second = currentLocation.second + 1;
            break;
        }
    default:
        {
            // Default = North
            nextLocation.first = currentLocation.first - 1;
            nextLocation.second = currentLocation.second;
            break;
        }
    }
    //std::cout<<"Next Location: ("<<nextLocation.first<<", "<<nextLocation.second<<")"<<std::endl;
    return nextLocation;
}


bool Robot::validateLocation(char content)
{
    /* Returns true if the location content is not 'X', an obstacle or any other robot ID (a,b,c, etc..)*/
    if(content < 88 && content != 42)
    {
        return true;
    }
    else
    {
        return false;
    }

}


void Robot::changeHeadDirection()
{
    /*Changes the head direction of the robot agent based on the orientation setting of the robot*/
    if(movesClockwise)
    {
        switch(head)
        {
        case North:      head = NorthEast; break;
        case NorthEast:  head = East; break;
        case East:       head = SouthEast; break;
        case SouthEast:  head = South; break;
        case South:      head = SouthWest;break;
        case SouthWest:  head = West; break;
        case West:       head = NorthWest; break;
        case NorthWest:  head = North;
        }
    }
    else
    {
        switch(head)
        {
        case North:      head = NorthWest; break;
        case NorthWest:  head = West; break;
        case West:       head = SouthWest; break;
        case SouthWest:  head = South; break;
        case South:      head = SouthEast; break;
        case SouthEast:  head = East; break;
        case East:       head = NorthEast; break;
        case NorthEast:  head = North;
        }
    }
}


void Robot::changeHeadRotation()
{
    movesClockwise = !movesClockwise;
}
