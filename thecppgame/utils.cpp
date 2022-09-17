#include <iostream>
#include <thread>
#include "utils.h"
#include "worldmap.h"
#include "game.h"
#include "utility"
// C libraries to control console input
#include <unistd.h>
#include <cstdlib>

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
    int obstacleLimit = mapDimensions.first * mapDimensions.second * 0.3;
    do {
        std::cout<<"Enter the number of obstacles in the  map >>\n";
        std::cout<<"Note: 0 < Max. obstacles <= "<<obstacleLimit<<std::endl;
        std::cin>>obstacleCount;
        if(obstacleCount > 0 && obstacleCount <= obstacleLimit)
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
    std::cout<<"Enter: 6 for E, 9 for NE, 8 for N, 7 for NW, 4 for W, 1 for SW, 2 for S, 3 for SE >>"<<std::endl;
    std::cin>>headDirection;
    switch(headDirection)
    {
    case 6:
        {
            return East;
        }
    case 9:
        {
            return NorthEast;
        }
    case 8:
        {
            return North;
        }
    case 7:
        {
            return NorthWest;
        }
    case 4:
        {
            return West;
        }
    case 1:
        {
            return SouthWest;
        }
    case 2:
        {
            return South;
        }
    case 3:
        {
            return SouthEast;
        }
    default:
        {
            return North;
        }
    }

}


std::pair<int, int> askUserForNextStep(World & wmap, std::pair<int, int> currentLocation)
{

    int xCoordinate = currentLocation.first;
    int yCoordinate = currentLocation.second;
    char content;
    do
    {
        direction input = askHeadDirection();
        if (input == East)
        {
            yCoordinate +=1;
        }
        else if(input == SouthEast)
        {
            xCoordinate +=1;
            yCoordinate +=1;
        }
        else if(input == South)
        {
            xCoordinate +=1;
        }
        else if(input == SouthWest)
        {
            xCoordinate +=1;
            yCoordinate -=1;
        }
        else if(input == West)
        {
            yCoordinate -=1;
        }
        else if(input == NorthWest)
        {
            xCoordinate -=1;
            yCoordinate -=1;
        }
        else if(input == North)
        {
            xCoordinate -=1;
        }
        else if(input == NorthEast)
        {
            xCoordinate -=1;
            yCoordinate +=1;
        }

        content = wmap.getLocationContent(std::make_pair(xCoordinate, yCoordinate));
        switch(content)
        {
        case 'X':
            {
                std::cout<<"Oh No!! You've hit an obstacle! Change the direction!!\n";
                xCoordinate = currentLocation.first;
                yCoordinate = currentLocation.second;
                break;
            }
        case 'O':
            {
                std::cout<<"Oops! You've hit a hole!!\n";
                break;
            }
        case '+':
            {
                std::cout<<"Wow! You've hit a bonus!\n";
                break;
            }
        case '*':
            {
                std::cout<<"Oh No!! You've hit the boundary! Change the direction!!\n";
                xCoordinate = currentLocation.first;
                yCoordinate = currentLocation.second;
                break;
            }
        }

    }while(content == 'X' || content == '*');
    return std::make_pair(xCoordinate, yCoordinate);
}


/*
    // Initialization
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    fd_set savefds = readfds;
    // Time out initialization
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    if (select(1, &readfds, NULL, NULL, &timeout))
        {
            std::cin>>headDirection;
            send(headDirection);
        }
    readfds = savefds;
*/
