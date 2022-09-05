#ifndef UTILS_H_
#define UTILS_H_
#include "game.h"
#include <utility>
std::pair<int, int> askMapDimensions();
int askNumberOfObstacles(std::pair<int, int> mapDimensions);
int askNumberOfIterations();
mode askMode();
#endif
