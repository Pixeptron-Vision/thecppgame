## Introduction:
Thecppgame is an attempt to demonstrate use of various C++ features. The game comprises a rectangular world map comprising obstacles and robot agents. The robot agents move around the world map avoiding the obstacles based on the navigation rules in the world. The events in the world map are displayed on the command line screen for the game master (user) to witness. In auto mode, the game master only specifies the duration of the game as opposed to the manual mode where the game master has to navigate the robot by responding to the command line prompts. The first version of this project demonstrates skills in System Design, Object oriented programming and Parallel programming. 

## Game:
The game comprises moving robot agents traversing a world map during a game period. A game period is defined as the number of maximum iterations available in the game. There are two modes in the game, auto and manual with two options single agent and multi agent. 

- **Auto Mode:** The auto mode of the game is synonymous to a game simulation. A player (user) may use this mode to learn the game from simulations of the game. In the auto mode, the game has two options, single agent and multiple agents. 

    - **Single Agent:** For a single robot agent in auto mode, the goal of the agent is to survive the game period and win the game by reaching the destination within the game period. 

    - **Multiple Agent:** For a multiple robot agent in auto mode, the goal of the agents is to reach the destination before the game-time. A single agent’s success would be considered as success for the entire team.   

- **Manual Mode:** In the manual mode, the goal of a player is to traverse the robot agent through the map to reach the destination within the game period. 

**Rules:**
The robot agents in the game abide by the following rules: 
- Can move one step in each iteration.
- Every subsequent move of the agent is taken in the direction of the agent’s head.
- Obstacles and boundaries in the direction of the agent’s head will stop the progression of the agent.
- The direction of the agent’s head changes in a predetermined rotation cycle when obstructed. 
- In a clockwise rotation cycle, the angle/direction of head changes in the following sequence: 0° → 315° → 270° → 225° → 180° → 135° → 90° → 45°
- In an anti-clockwise rotation cycle, the angle/direction of the head changes in the following sequence: 0° → 45° → 90° → 135° → 180° → 225° → 270° → 315°
- A robot agent occupying the position of a ‘+’ in the world map increases its life count. 
- A robot agent occupying the position of a hole in the world map will, 1) Get a reduction of one life and 2) Loses the game if the life score of the agent becomes zero by occupying the hole. 
- A robot agent occupying the position of the destination in the world map wins and ends the game. 

**World Map:** The following picture depicts a 5 x 5 world map with robot agents, obstacles, collectibles/life savers and a destination in it. The description for various symbols in the map is as follows: 

| Symbol | Description |

|+ 		 | Life savers |         

|X 		 | Obstacles   |

|O 		 | Holes       |

|@ 		 | Destination |

|a, b,...| Robot Agents|
 
**Play:** The game can be run through the executable “thecppgame.exe” present here on the repository. 

**Algorithms:** The key algorithms used in the game, their utility and impact of the choices are listed below:

- Random Number Generator: The random number generator for this task is taken from stackoverflow reference. 
    - Merits: Provides a simple yet useful way to generate coordinates for obstacles, holes and collectible locations in the world map of the game. 
    - Demerits: Yet to be studied. 

**References:**
- https://stackoverflow.com/questions/26086128/generate-different-random-coordinates
- https://raymii.org/s/articles/Cpp_async_threads_and_user_input.html
- https://www.justsoftwaresolutions.co.uk/threading/multithreading-in-c++0x-part-1-starting-threads.html
- http://www.azillionmonkeys.com/qed/random.html
