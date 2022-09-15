cmake_minimum_required(VERSION 3.16)

# set the project name
project(DontPanic2)

SET(MAPS
   "bestpath.txt"
   "bestpath_missing_elevators.txt"
   "elevator.txt"
   "one_elevator_per_floor.txt"
   "2missing.txt"
   "3missing.txt"
   )

FILE(COPY ${MAPS} DESTINATION ".")

# add the executable
add_executable(dontPanic main.cpp)
