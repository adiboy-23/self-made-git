#!/bin/sh
set -e

# Build if the executable doesn't exist
if [ ! -f "./build/your_program" ]; then
    cmake -B build -S .
    cmake --build ./build
fi

# Run the program
exec ./build/your_program "$@"
