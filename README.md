# Assignment Collection Robot

## Description
This project examines the efficiency of a Task Motion Planning (TMP) model for controlling the movements of a robot within a 2D environment to collect two assignment reports and deliver them to a submission desk. The robot uses a roadmap with waypoints and an A* algorithm to calculate the optimal path. [Report](httml) for more details.

## Tasks
- Environment Definition: a 6m x 6m 2D environment is defined, with four regions representing student groups, 24 randomly generated waypoints, the robot's initial position, and the location of the submission desk.
- Roadmap Generation: the randomly generated waypoints are connected to form a roadmap. Each waypoint is connected to a maximum of k other waypoints.
- Connecting Points of Interest: the student regions, the robot's initial location, and the submission desk are connected to their nearest nodes in the roadmap.
- Path Planning: an A* algorithm is used to plan the optimal path for the robot to visit two student regions, collect the assignments, and deliver them to the submission desk, minimizing the total path length.

## Tools used
- [PDDL](https://planning.wiki/guide/whatis/pddl) (Planning Domain Definition Language): A standard language for representing planning problems, used to model the coffee shop environment, the robots, and their actions.
- popf-tif planner: a planner that links C++ code with PDDL.
- A* algorithm: an algorithm used to find the shortest path between two nodes.
- External C++ solver: used to execute the A* algorithm.

## Results
- The project successfully implemented a TMP model to control the robot's movements.
- The A* algorithm efficiently identified the optimal path for collecting and delivering the assignments.
- The optimal path was calculated in 0.01 seconds.

## How to test

### Installation

You only need to install the required libraries:
```sh
sudo apt-get install cmake coinor-libclp-dev coinor-libcoinutils-dev coinor-libosi-dev coinor-libcbc-dev coinor-libcgl-dev doxygen bison flex
```

There is no need to compile the `popf3-clp` since it is already compiled in the root folder.

### Usage

To run the program is necessary to be into the folder of the assignment and to open a terminal window; after that just run this:
```sh
./popf3-clp -x ./visits_domain/dom1.pddl ./visits_domain/prob1.pddl ./visits_module/build/libVisits.so ./visits_domain/region
```

<b>IMPORTANT:</b>
You may have to add execution privileges to the following files:
```sh
sudo chmod +x popf3-clp
```

## Collaborators
Thanks to [Claudio Tomaiuolo](https://github.com/ClousTom) and [Teodoro Lima](https://github.com/teolima99) for working with me on this project!
