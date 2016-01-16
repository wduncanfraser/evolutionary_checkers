#!/usr/bin/env bash

# Make bin directory and cd
mkdir bin || true
cd bin

# Build project
cmake ..

# Run Tests
make
./test_gameboard
./test_neuralnet