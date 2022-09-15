#include <iostream>
#include <thread>
#include "utils.h"
#include "game.h"
#include "utility"

std::pair<int, int> askMapDimensions()
{
    int length = 0;
    int breadth = 0;
    bool lengthReceived = false;
    bool breadthReceived = false;
    // Take length input from user
    do{
        std::cout<<"Enter the Map Length dimension >>\nNote: The Map Length must be an integer < 16 \n"<<std::endl;
        std::cin>>length;
        if(length > 0 && length <= 15 )
        {
            lengthReceived = true;
        }
        else
        {
            std::cout<<"Wrong dimension value received: "<<length<<std::endl;
        }
    }while(!lengthReceived);
    // Take breadth input from user
    do{
        std::cout<<"Enter the Map Breadth dimension >>\nNote: The Map Breadth must be an integer < 16 \n"<<std::endl;
        std::cin>>breadth;
        if(breadth > 0 && breadth <= 15 )
        {
            breadthReceived = true;
        }
        else
        {
            std::cout<<"Wrong dimension value received: "<<breadth<<std::endl;
        }
    }while(!breadthReceived);
    // Create a dimension pair and return
    std::pair<int, int> mapDimensions(length, breadth);
    return mapDimensions;
}


int askNumberOfObstacles(std::pair<int, int> mapDimensions)
{
    int obstacleCount = 0;
    bool validObstacleCountReceived = false;
    do {
        std::cout<<"Enter the number of obstacles in the  map >>\n";
        std::cout<<"Note: 0 < Max. obstacles < "<<mapDimensions.first * mapDimensions.second<<std::endl;
        std::cin>>obstacleCount;
        if(obstacleCount > 0 && obstacleCount < mapDimensions.first * mapDimensions.second)
        {
            validObstacleCountReceived = true;
        }
        else
        {
            std::cout<<"Wrong number received for the number of obstacles: "<<obstacleCount<<std::endl;
        }
    }while(!validObstacleCountReceived);


    return obstacleCount;
}


int askNumberOfIterations()
{
    int numberOfIterations = 0;
    bool validIterationCountReceived = false;
    do {
        std::cout<<"Enter the number of games suggested >>\n";
        std::cout<<"Note: Number of games > 0"<<std::endl;
        std::cin>>numberOfIterations;
        if(numberOfIterations > 0 )
        {
            validIterationCountReceived = true;
        }
        else
        {
            std::cout<<"Wrong number received for the number of iterations: "<<numberOfIterations<<std::endl;
        }
    }while(!validIterationCountReceived );


    return numberOfIterations;

}


mode askMode()
{
    int modeInput;
    mode gameMode;
    bool validModeSelection = false;
    do {
        std::cout<<"Enter the number of Game Mode\n  Single Player Auto - Press 0  \n  Single Player Manual - Press 5 \n";
        std::cout<<"  Multi Agent Auto - Press 3\n";
        std::cin>> modeInput;
        switch(modeInput)
        {
        case 0:
            {
                validModeSelection = true;
                gameMode = single_auto;
                break;
            }
        case 5:
            {
                validModeSelection = true;
                gameMode = single_manual;
                break;
            }
        case 3:
            {
                validModeSelection = true;
                gameMode = multiple_auto;
                break;
            }
        default:
            {
                std::cout<<"Wrong Mode input selected!! Please try again! \n";
            }
        }
    }while(!validModeSelection);

    return gameMode;
}


void displayList(std::vector<std::pair<int, int>> locationList)
{
    std::cout<<"Displaying the Trail\n";
    for(auto it=locationList.begin(); it!=locationList.end(); ++it)
    {
        std::cout<<"("<<(*it).first<<","<<(*it).second<<") --> ";
    }
    std::cout<<"\n";

}


direction askHeadDirection()
{
    int headDirection;
    std::cout<<"Enter Robot Head Direction (Default: North) "<<std::endl;
    std::cout<<"Enter: 0 for E, 1 for NE, 2 for N, 3 for NW, 4 for W, 5 for SW, 6 for S, 7 for SE >>"<<std::endl;
    std::cin>>headDirection;
    switch(headDirection)
    {
    case 0:
        {
            return East;
        }
    case 1:
        {
            return NorthEast;
        }
    case 2:
        {
            return North;
        }
    case 3:
        {
            return NorthWest;
        }
    case 4:
        {
            return West;
        }
    case 5:
        {
            return SouthWest;
        }
    case 6:
        {
            return South;
        }
    case 7:
        {
            return SouthEast;
        }
    default:
        {
            return North;
        }
    }

}
