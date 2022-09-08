#include <iostream>
#include <deque>
#include <vector>
#include <random>
#include <chrono>
#include "worldmap.h"
#include "robot.h"

Robot:: Robot(std::pair<int, int> startPosition, std::pair<int, int> stopPosition, direction head, bool motionDirection, mode opMode, float robotTimeUnit): Game{},
    startLocation{startPosition}, stopLocation{stopPosition}, head{head}, robotTimeUnit{robotTimeUnit}, movesClockwise{motionDirection}, operationMode{opMode},
    isStuck{isStuck}, currentLocation{startPosition}
{
    // Init the start time
    startTime = std::chrono::high_resolution_clock::now();
}


Robot:: ~Robot(){}

int Robot:: numberOfRobots = 0;


float Robot:: getRobotTimeUnit()
{
    return robotTimeUnit;
}


void Robot:: setRobotTimeUnit(float robotTime)
{
    robotTimeUnit = robotTime;
}


void Robot:: run()
{

}

std::vector<std::pair<int, int>> Robot:: shortestPathBFS(World wmap)
{

    // Declare the Queue, vector for storing path and array to store flags
    std::deque<std::pair<int, int>> Q;
    std::vector<std::pair<int, int>> forwardPath;
    std::vector<std::pair<int, int>> backwardPath;
    // Init the node flag of the environment
    std::pair<int, int> dimensions = wmap.getWorldDimensions();
    std::vector<bool> temp(dimensions.second, false);
    std::vector<std::vector<bool>> visitedNodeFlags(dimensions.first,temp );  // has to be initialized
    // Set the starting location as visited
    visitedNodeFlags[startLocation.first][startLocation.second] = true;
    // Initialize the Queue and add starting node to it
    Q.push_back(startLocation);
    // Start the search for path
    while(Q.size() > 0)
    {
        std::pair<int, int> node = Q[0];
        if (node.first == stopLocation.first && node.second == stopLocation.second)
        {
            // If you come across the destination location, stop search
            forwardPath.push_back(node);
            break;
        }
        // get the neighbor list for the node
        std::vector<std::pair<int, int>> neighbors = wmap.getTraversibleNeighborList(node);
        for(auto it = neighbors.begin(); it!=neighbors.end(); ++it)
        {
            if(visitedNodeFlags[(*it).first][(*it).second]== false)
            {
                // If the node neighbor is not visited, add it to the queue
                Q.push_back(*it);
                visitedNodeFlags[(*it).first][(*it).second]= true;
                backwardPath.push_back(*it);

            }

        }
        Q.pop_front();
        forwardPath.push_back(node);

    }


    return backwardPath;
    /*

    int index =finish;
    deque<int>shortestPath;
    shortestPath.push_back(finish);
    int shortestPathLength = 0;
    while(index!=start)
    {
        shortestPathLength++;
        index = previousNode[index];
        shortestPath.push_front(index);
    }
    cout<<"Shortest Path Length:- "<<shortestPathLength<<endl;
    return shortestPath;

    */


}
