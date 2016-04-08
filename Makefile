FLAGS = -O3 -std=c++11 -I./nanoflann/include --memory-init-file 0 -Wall -s TOTAL_MEMORY=2130706431

default_target: fisheye

fisheye:
	em++ $(FLAGS) --bind -o fast_fisheye.em.js fast_fisheye.cpp
