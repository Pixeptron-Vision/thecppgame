#ifndef UTILS_H_
#define UTILS_H_
#include "game.h"
#include <thread>
#include <utility>

class thread_guard {
private:
	std:: thread & t;
public :
	explicit thread_guard(std::thread & _t) : t(_t) { }  // Constructor is explicit as we don’t want to use it in any implicit conversions
    ~thread_guard()
    {
        if (t.joinable())
        {
            t.join();        //Join when thread obj is destructed due to error or termination
        }
    }

// Since we don’t need to copy the thread_guard object, we may delete copy constructor and //copy assignment operator

	thread_guard( thread_guard  &  ) = delete;
	thread_guard & operator= (thread_guard & ) = delete;
};

std::pair<int, int> askMapDimensions();
int askNumberOfObstacles(std::pair<int, int> mapDimensions);
int askNumberOfIterations();
mode askMode();
void displayList(std::vector<std::pair<int, int>> locationList);

#endif
