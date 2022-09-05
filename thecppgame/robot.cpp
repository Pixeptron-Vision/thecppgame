#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include "robot.h"

Robot::Robot(std::pair<int, int> startPosition, direction head, bool movesClockwise, bool isAutoMode, bool isStuck): Game{},
    location{startPosition}, head{head}, movesClockwise{movesClockwise}, autoMode{isAutoMode}, isStuck{isStuck}
{

}

Robot:: ~Robot(){}
