#!/bin/bash

g++ -std=c++17 -O0 -g -Wall -Wextra -Winit-self -Wno-missing-field-initializers -pthread -o pipe.elf main.cpp
