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
    life = 3;
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


direction Robot::getHeadDirection()
{
    return head;
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
