#ifndef GAME_H_
#define GAME_H_

enum mode{single_manual, single_auto , multiple_auto};

class Game{
 private:

 protected:
    mode currentMode;
    int activeRobots;

 public:

    Game();
    Game(mode gameMode); // Constructor to be used by World
    bool run();
    bool stop();
    bool pause();
    bool resume();
    void setMode(mode gameMode);
    mode getMode();
    virtual ~Game();

};

#endif
