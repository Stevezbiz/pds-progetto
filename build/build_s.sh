#!/bin/sh
cmake --install ..
cmake ..
cmake --build . --target server
